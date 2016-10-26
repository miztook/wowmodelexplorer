#include "stdafx.h"
#include "COpenGLDrawServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLHardwareBufferServices.h"

COpenGLDrawServices::COpenGLDrawServices()
{
	Driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	HWBufferServices = static_cast<COpenGLHardwareBufferServices*>(g_Engine->getHardwareBufferServices());

	MaxImageBatch = min_(MAX_IMAGE_BATCH_COUNT(), IHardwareBufferServices::MAX_QUADS());

	Line2DVertexLimit = MAX_2DLINE_BATCH_COUNT() * 2;
	Line3DVertexLimit = MAX_3DLINE_BATCH_COUNT() * 2;
	ImageVertexLimit = MaxImageBatch * 4;
	VertexLimit = MAX_VERTEX_COUNT();
	IndexLimit = MAX_INDEX_COUNT();
	VertexLimit2D = MAX_VERTEX_2D_COUNT();
	IndexLimit2D = MAX_INDEX_2D_COUNT();

	//line material
	LineMaterial.MaterialType = EMT_LINE;
	LineMaterial.Cull = ECM_NONE;
	LineMaterial.Lighting = false;
	LineMaterial.ZWriteEnable = false;
	LineMaterial.ZBuffer = ECFN_ALWAYS;
	LineMaterial.AntiAliasing = EAAM_LINE_SMOOTH;

	Line2DVertices = new SVertex_PC[Line2DVertexLimit];
	Line3DVertices = new SVertex_PC[Line3DVertexLimit];
	ImageVertices = new SVertex_PCT[ImageVertexLimit];
	Vertices3D = new SVertex_PC[VertexLimit];
	Indices3D = new u16[IndexLimit];
	Vertices2D = new SVertex_PCT[VertexLimit2D];
	Indices2D = new u16[IndexLimit2D];

	VBLine2D = new IVertexBuffer(false);
	VBLine2D->set(Line2DVertices, EST_PC, Line2DVertexLimit, EMM_DYNAMIC);
	VBLine3D = new IVertexBuffer(false);
	VBLine3D->set(Line3DVertices, EST_PC, Line3DVertexLimit, EMM_DYNAMIC);
	VBImage = new IVertexBuffer(false);
	VBImage->set(ImageVertices, EST_PCT, ImageVertexLimit, EMM_DYNAMIC);
	VB3D = new IVertexBuffer(false);
	VB3D->set(Vertices3D, EST_PC, VertexLimit, EMM_DYNAMIC);
	IB3D = new IIndexBuffer(false);
	IB3D->set(Indices3D, EIT_16BIT, IndexLimit, EMM_DYNAMIC);
	VB2D = new IVertexBuffer(false);
	VB2D->set(Vertices2D, EST_PCT, VertexLimit2D, EMM_DYNAMIC);
	IB2D = new IIndexBuffer(false);
	IB2D->set(Indices2D, EIT_16BIT, IndexLimit2D, EMM_DYNAMIC);

	Line2DVertexCount = 0;
	Line3DVertexCount = 0;

	CurrentIndex3DCount = 0;
	CurrentVertex3DCount = 0;

	//hw buffer
	HWBufferServices->createHardwareBuffer(VBLine2D);
	HWBufferServices->createHardwareBuffer(VBLine3D);
	HWBufferServices->createHardwareBuffer(VBImage);
	HWBufferServices->createHardwareBuffer(VB3D);
	HWBufferServices->createHardwareBuffer(IB3D);
	HWBufferServices->createHardwareBuffer(VB2D);
	HWBufferServices->createHardwareBuffer(IB2D);
}

COpenGLDrawServices::~COpenGLDrawServices()
{
	HWBufferServices->destroyHardwareBuffer(IB2D);
	HWBufferServices->destroyHardwareBuffer(VB2D);
	HWBufferServices->destroyHardwareBuffer(IB3D);
	HWBufferServices->destroyHardwareBuffer(VB3D);
	HWBufferServices->destroyHardwareBuffer(VBImage);
	HWBufferServices->destroyHardwareBuffer(VBLine3D);
	HWBufferServices->destroyHardwareBuffer(VBLine2D);

	//buffer
	delete IB2D;
	delete VB2D;
	delete IB3D;
	delete VB3D;
	delete VBImage;
	delete VBLine3D;
	delete VBLine2D;

	//vertices, indices
	delete[] Indices2D;
	delete[] Vertices2D;
	delete[] Indices3D;
	delete[] Vertices3D;
	delete[] ImageVertices;
	delete[] Line3DVertices;
	delete[] Line2DVertices;
}

void COpenGLDrawServices::add2DLine( const line2di& line, SColor color )
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	SVertex_PC v0, v1;
	v0.Pos.set((float)line.start.X, (float)line.start.Y, 0);
	v1.Pos.set((float)line.end.X, (float)line.end.Y, 0);
	v0.Color = v1.Color = color;

	Line2DVertices[Line2DVertexCount++] = v0;
	Line2DVertices[Line2DVertexCount++] = v1;
}

void COpenGLDrawServices::add2DRect( const recti& rect, SColor color )
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	SVertex_PC v0, v1, v2, v3;
	v0.Pos.set((float)rect.UpperLeftCorner.X, (float)rect.UpperLeftCorner.Y, 0);
	v1.Pos.set((float)rect.LowerRightCorner.X, (float)rect.UpperLeftCorner.Y, 0);
	v2.Pos.set((float)rect.LowerRightCorner.X, (float)rect.LowerRightCorner.Y, 0);
	v3.Pos.set((float)rect.UpperLeftCorner.X, (float)rect.LowerRightCorner.Y, 0);
	v0.Color = v1.Color = v2.Color = v3.Color = color;

	Line2DVertices[Line2DVertexCount++] = v0;
	Line2DVertices[Line2DVertexCount++] = v1;
	Line2DVertices[Line2DVertexCount++] = v1;
	Line2DVertices[Line2DVertexCount++] = v2;
	Line2DVertices[Line2DVertexCount++] = v2;
	Line2DVertices[Line2DVertexCount++] = v3;
	Line2DVertices[Line2DVertexCount++] = v3;
	Line2DVertices[Line2DVertexCount++] = v0;
}

void COpenGLDrawServices::flushAll2DLines()
{
	if (Line2DVertexCount == 0)
		return;

	HWBufferServices->updateHardwareBuffer(VBLine2D, Line2DVertexCount);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = VBLine2D;

	SDrawParam drawParam = {0};
	drawParam.startIndex = 0;
	drawParam.voffset0 = 0;
	drawParam.numVertices = Line2DVertexCount;

	ITexture* tex = g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite");
	Driver->setTexture(0, tex);

	S2DBlendParam blendParam;

	Driver->draw2DMode(bufferParam, EPT_LINES, Line2DVertexCount/2, drawParam, blendParam);

	Line2DVertexCount = 0;
}

void COpenGLDrawServices::add3DLine( const line3df& line, SColor color )
{
	if (Line3DVertexCount >= Line3DVertexLimit -2)
		return; //flushAll3DLines();

	SVertex_PC v0, v1;
	v0.Pos = line.start;
	v1.Pos = line.end;
	v0.Color = v1.Color = color;

	Line3DVertices[Line3DVertexCount++] = v0;
	Line3DVertices[Line3DVertexCount++] = v1;
}

void COpenGLDrawServices::addAABB( const aabbox3df& box, SColor color )
{
	if (!box.isValid() || Line3DVertexCount >= Line3DVertexLimit - 24)
		return;//flushAll3DLines();

	vector3df points[8];
	u16 indices[24];
	box.getVertices(points, indices, true);

	for (u32 i=0; i<24; ++i)
	{
		u32 index = indices[i];
		Line3DVertices[Line3DVertexCount++].set(points[index], color);
	}
}

void COpenGLDrawServices::add3DBox(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat)
{
	if (Line3DVertexCount >= Line3DVertexLimit - 24)
		return;//flushAll3DLines();

	static u16 aWireIndices[24] = 
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7,
	};

	float fx2 = vExts.X * 2.0f;
	float fy2 = vExts.Y * 2.0f;
	float fz2 = vExts.Z * 2.0f;

	vector3df aVerts[8];

	aVerts[0] = vPos + vDir * vExts.Z + vUp * vExts.Y - vRight * vExts.X;
	aVerts[1] = aVerts[0] + vRight * fx2;
	aVerts[2] = aVerts[1] - vDir * fz2;
	aVerts[3] = aVerts[0] - vDir * fz2;
	aVerts[4] = aVerts[0] - vUp * fy2;
	aVerts[5] = aVerts[4] + vRight * fx2;
	aVerts[6] = aVerts[5] - vDir * fz2;
	aVerts[7] = aVerts[4] - vDir * fz2;

	if (pMat)
	{
		for (int i=0; i < 8; i++)
			(*pMat).transformVect(aVerts[i]);
	}

	for (u32 i=0; i<24; ++i)
	{
		u32 index = aWireIndices[i];
		Line3DVertices[Line3DVertexCount++].set(aVerts[index], color);
	}
}

void COpenGLDrawServices::addSphere(const vector3df& center, float radius, SColor color, u32 hori /* = 10 */, u32 vert /* = 6 */)
{
	u32 polyCountX = min_(hori, 20u);
	u32 polyCountY = min_(vert, 10u);

	const float AngleX = 2 * PI / polyCountX;
	const float AngleY = PI / polyCountY;

	vector3df vpos[11][20];

	float ay = 0;

	for (u32 y = 0; y <= polyCountY; ++y)
	{
		const float sinay = sin(ay);
		float axz = 0;

		// calculate the necessary vertices without the doubled one
		vector3df lastp;
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position
			vector3df pos(static_cast<float>(radius * cos(axz) * sinay),
				static_cast<float>(radius * cos(ay)),
				static_cast<float>(radius * sin(axz) * sinay));
			// for spheres the normal is the position

			vpos[y][xz] = center + pos;

			axz += AngleX;
		}
		ay += AngleY;
	}

	for (u32 y = 0; y <= polyCountY; ++y)
	{	
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			vector3df v1, v2;
			v1 = vpos[y][xz];
			if (y != 0 && y != polyCountY)						//去掉头，底两行
			{
				v2 = (xz+1) < polyCountX ?  vpos[y][xz+1] : vpos[y][0];
				add3DLine(line3df(v1, v2), color);
			}
			if (y != 0)							//第一行和上一行
			{
				v2 = vpos[y-1][xz];
				add3DLine(line3df(v1, v2), color);
			}
		}
	}
}

void COpenGLDrawServices::flushAll3DLines( ICamera* cam )
{
	if (Line3DVertexCount == 0)
		return;

	HWBufferServices->updateHardwareBuffer(VBLine3D, Line3DVertexCount);

	ITexture* tex = g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite");
	if (cam)
	{
		Driver->setTransform_Material_Textures(matrix4::Identity(),
			cam->getViewMatrix(),
			cam->getProjectionMatrix(),
			LineMaterial,
			&tex, 
			1);
	}
	else
	{
		Driver->setTransform_Material_Textures(matrix4::Identity(),
			LineMaterial,
			&tex, 
			1);
	}

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = VBLine3D;

	SDrawParam drawParam = {0};
	drawParam.startIndex = 0;
	drawParam.voffset0 = 0;
	drawParam.numVertices = Line3DVertexCount;

	Driver->draw3DMode(bufferParam, EPT_LINES, Line3DVertexCount/2, drawParam);

	Line3DVertexCount = 0;
}

void COpenGLDrawServices::addAABB_Flat(const aabbox3df& box, SColor color)
{
	if (!box.isValid() || 
		8 + CurrentVertex3DCount >= VertexLimit ||
		36 + CurrentIndex3DCount >= IndexLimit)
		return;

	vector3df points[8];
	u16 indices[36];
	box.getVertices(points, indices, false);

	add3DVertices(points, 8, indices, 36, color);
}

void COpenGLDrawServices::add3DBox_Flat(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat)
{
	if (8 + CurrentVertex3DCount >= VertexLimit ||
		36 + CurrentIndex3DCount >= IndexLimit)
		return;

	static u16 indexTriangle[] =
	{
		0, 1, 3, 3, 1, 2, 
		2, 1, 6, 6, 1, 5, 
		5, 1, 4, 4, 1, 0,
		0, 3, 4, 4, 3, 7, 
		3, 2, 7, 7, 2, 6, 
		7, 6, 4, 4, 6, 5
	};

	float fx2 = vExts.X * 2.0f;
	float fy2 = vExts.Y * 2.0f;
	float fz2 = vExts.Z * 2.0f;

	vector3df aVerts[8];

	aVerts[0] = vPos + vDir * vExts.Z + vUp * vExts.Y - vRight * vExts.X;
	aVerts[1] = aVerts[0] + vRight * fx2;
	aVerts[2] = aVerts[1] - vDir * fz2;
	aVerts[3] = aVerts[0] - vDir * fz2;
	aVerts[4] = aVerts[0] - vUp * fy2;
	aVerts[5] = aVerts[4] + vRight * fx2;
	aVerts[6] = aVerts[5] - vDir * fz2;
	aVerts[7] = aVerts[4] - vDir * fz2;

	if (pMat)
	{
		for (int i=0; i < 8; i++)
			(*pMat).transformVect(aVerts[i]);
	}

	add3DVertices(aVerts, 8, indexTriangle, 36, color);
}

void COpenGLDrawServices::addSphere_Flat(const vector3df& center, float radius, SColor color, u32 hori /* = 10 */, u32 vert /* = 6 */)
{
	u32 polyCountX = min_(hori, 20u);
	u32 polyCountY = min_(vert, 10u);

	const float AngleX = 2 * PI / polyCountX;
	const float AngleY = PI / polyCountY;

	vector3df vpos[11][21];

	float ay = 0;

	for (u32 y = 0; y <= polyCountY; ++y)
	{
		const float sinay = sin(ay);
		float axz = 0;

		// calculate the necessary vertices without the doubled one
		vector3df lastp;
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position
			vector3df pos(static_cast<float>(radius * cos(axz) * sinay),
				static_cast<float>(radius * cos(ay)),
				static_cast<float>(radius * sin(axz) * sinay));
			// for spheres the normal is the position

			vpos[y][xz] = center + pos;

			axz += AngleX;
		}
		ay += AngleY;
	}

	static u16 indices[] = { 0, 1, 2, 1, 3, 2};

	for (u32 y = 0; y <= polyCountY; ++y)
	{	
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			vector3df v[4];
			v[0] = vpos[y][xz];
			if (y != 0)						
			{
				if (y == 1)
				{					
					v[1] = vpos[y-1][xz];
					v[2] = (xz+1) < polyCountX ?  vpos[y][xz+1] : vpos[y][0];

					add3DVertices(v, 3, color);
				}
				else if(y == polyCountY)
				{
					v[1] = vpos[y-1][xz];
					v[2] = (xz+1) < polyCountX ?  vpos[y-1][xz+1] : vpos[y-1][0];

					add3DVertices(v, 3, color);
				}
				else			//去掉头，底两行
				{
					v[1] = (xz+1) < polyCountX ?  vpos[y][xz+1] : vpos[y][0];
					v[2] = vpos[y-1][xz];
					v[3] = (xz+1) < polyCountX ?  vpos[y-1][xz+1] : vpos[y-1][0];

					add3DVertices(v, 4, indices, 6, color);
				}
			}
		}
	}
}

void COpenGLDrawServices::add3DVertices( vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color )
{
	if (numverts + CurrentVertex3DCount >= VertexLimit ||
		numindices + CurrentIndex3DCount >= IndexLimit)
		return;

	for(u32 i=0; i<numverts; ++i)
	{
		Vertices3D[CurrentVertex3DCount + i].Pos = verts[i];
		Vertices3D[CurrentVertex3DCount + i].Color = color;
	}

	for (u32 i=0; i<numindices; ++i)
	{
		Indices3D[CurrentIndex3DCount + i] = indices[i] + (u16)CurrentVertex3DCount;
	}

	CurrentVertex3DCount += numverts;
	CurrentIndex3DCount += numindices;
}

void COpenGLDrawServices::add3DVertices( vector3df* verts, u32 numverts, SColor color )
{
	if (numverts + CurrentVertex3DCount >= VertexLimit ||
		numverts + CurrentIndex3DCount >= IndexLimit)
		return;

	for(u32 i=0; i<numverts; ++i)
	{
		Vertices3D[CurrentVertex3DCount + i].Pos = verts[i];
		Vertices3D[CurrentVertex3DCount + i].Color = color;

		Indices3D[CurrentIndex3DCount + i] = i + (u16)CurrentVertex3DCount;
	}

	CurrentVertex3DCount += numverts;
	CurrentIndex3DCount += numverts;
}

void COpenGLDrawServices::flushAll3DVertices( ICamera* cam )
{
	if (!CurrentVertex3DCount || !CurrentIndex3DCount)
		return;

	HWBufferServices->updateHardwareBuffer(VB3D, CurrentVertex3DCount);
	HWBufferServices->updateHardwareBuffer(IB3D, CurrentIndex3DCount);

	Driver->setTransform(ETS_WORLD, matrix4::Identity());
	if (cam)
	{
		Driver->setTransform(ETS_VIEW, cam->getViewMatrix());
		Driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
	}
	Driver->setMaterial(LineMaterial);

	ITexture* tex = g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite");
	Driver->setTexture(0, tex);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = VB3D;
	bufferParam.ibuffer = IB3D;

	SDrawParam drawParam = {0};
	drawParam.numVertices = CurrentVertex3DCount;
	drawParam.baseVertIndex = 0;

	Driver->draw3DMode(bufferParam, EPT_TRIANGLES, CurrentIndex3DCount/3, drawParam);

	CurrentVertex3DCount = CurrentIndex3DCount = 0;
}

void COpenGLDrawServices::draw2DColor(const recti& rect, SColor color, float scale /* = 1.0f */, E_BLEND_FACTOR srcBlend/* =EBF_ONE */, E_BLEND_FACTOR destBlend/* =EBF_ONE_MINUS_SRC_ALPHA */)
{
	vector2di destPos = rect.UpperLeftCorner;
	recti rc(0,0,rect.getWidth(), rect.getHeight());

	S2DBlendParam blendParam(color.getAlpha() < 255, false, srcBlend, destBlend);
	ITexture* tex = g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite");
	draw2DImage(tex, destPos, &rc, color, ERU_00_11, scale, blendParam);
}

void COpenGLDrawServices::draw2DImage(ITexture* texture, vector2di destPos, const recti* sourceRect /* = NULL_PTR */, SColor color /* = SColor() */, E_RECT_UVCOORDS uvcoords /* = ERU_00_11 */, float scale /* = 1.0f */, const S2DBlendParam& blendParam /* = S2DBlendParam::OpaqueSource() */)
{
	draw2DImageBatch(texture, &destPos, sourceRect ? &sourceRect : NULL_PTR, 1, color, uvcoords, scale, blendParam);
}

void COpenGLDrawServices::draw2DImageBatch(ITexture* texture, const vector2di* positions, const recti* sourceRects[], u32 batchCount, SColor color /* = SColor() */, E_RECT_UVCOORDS uvcoords /* = ERU_00_11 */, float scale /* = 1.0f */, const S2DBlendParam& blendParam /* = S2DBlendParam::OpaqueSource() */)
{
	ASSERT(texture || sourceRects);
	if (!texture && !sourceRects)
		return;

	if (batchCount > MaxImageBatch)
		batchCount = MaxImageBatch;

	SVertex_PCT* vertices = &ImageVertices[0];
	const dimension2du texSize = texture ? texture->getSize() : dimension2du(0, 0);

	for (u32 i=0; i<batchCount; ++i)
	{
		vector2di destPos = positions[i];
		vector2di sourcePos = sourceRects ? sourceRects[i]->UpperLeftCorner : vector2di(0,0);
		dimension2di sourceSize = sourceRects ? dimension2di(sourceRects[i]->getWidth(), sourceRects[i]->getHeight()) : dimension2di((int)texSize.Width, (int)texSize.Height);

		//tcoords
		rectf tcoords;
		float x0, x1, y0, y1;

		if (texture)
		{
			ASSERT(texSize.getArea() > 0);

			x0 = ((float)sourcePos.X) / (float)texSize.Width;
			x1 = ((float)sourcePos.Y) / (float)texSize.Height;
			y0 = ((float)(sourcePos.X + sourceSize.Width)) / (float)texSize.Width;
			y1 = ((float)(sourcePos.Y + sourceSize.Height)) / (float)texSize.Height;
		}
		else
		{
			x0 = 0.0f;
			x1 = 0.0f;
			y0 = 1.0f;
			y1 = 1.0f;
		}

		switch(uvcoords)
		{
		case ERU_01_10:
			{
				tcoords.set(x0, y1, y0, x1);
			}
			break;
		case ERU_10_01:
			{
				tcoords.set(y0, x1, x0, y1);
			}
			break;
		case ERU_11_00:
			{
				tcoords.set(y0, y1, x0, x1);
			}
			break;
		case  ERU_00_11:
		default:
			{
				tcoords.set(x0, x1, y0, y1);
			}
			break;
		}	

		//vertices
		rectf poss((float)destPos.X, 
			(float)destPos.Y, 
			(float)(destPos.X+(int)(sourceSize.Width * scale)), 
			(float)(destPos.Y+(int)(sourceSize.Height * scale)));

		vertices[ i*4 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.top()));

		vertices[ i*4+1 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.top()));

		vertices[ i*4+2 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.bottom()));

		vertices[ i*4+3 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.bottom()));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL_PTR);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam);
}

void COpenGLDrawServices::draw2DImageBatch( ITexture* texture, const vector2di* positions, const recti* sourceRects[], const SColor* colors, u32 batchCount, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, float scale /*= 1.0f*/, const S2DBlendParam& blendParam /*= S2DBlendParam()*/ )
{
	ASSERT(texture || sourceRects);
	if (!texture && !sourceRects)
		return;

	if (batchCount > MaxImageBatch)
		batchCount = MaxImageBatch;

	SVertex_PCT* vertices = &ImageVertices[0];
	const dimension2du texSize = texture ? texture->getSize() : dimension2du(0, 0);

	for (u32 i=0; i<batchCount; ++i)
	{
		vector2di destPos = positions[i];
		vector2di sourcePos = sourceRects ? sourceRects[i]->UpperLeftCorner : vector2di(0,0);
		SColor color = colors[i];
		dimension2di sourceSize = sourceRects ? dimension2di(sourceRects[i]->getWidth(), sourceRects[i]->getHeight()) : dimension2di((int)texSize.Width, (int)texSize.Height);

		//tcoords
		rectf tcoords;
		float x0, x1, y0, y1;

		if (texture)
		{
			ASSERT(texSize.getArea() > 0);

			x0 = ((float)sourcePos.X) / (float)texSize.Width;
			x1 = ((float)sourcePos.Y) / (float)texSize.Height;
			y0 = ((float)(sourcePos.X + sourceSize.Width)) / (float)texSize.Width;
			y1 = ((float)(sourcePos.Y + sourceSize.Height)) / (float)texSize.Height;
		}
		else
		{
			x0 = 0.0f;
			x1 = 0.0f;
			y0 = 1.0f;
			y1 = 1.0f;
		}

		switch(uvcoords)
		{
		case ERU_01_10:
			{
				tcoords.set(x0, y1, y0, x1);
			}
			break;
		case ERU_10_01:
			{
				tcoords.set(y0, x1, x0, y1);
			}
			break;
		case ERU_11_00:
			{
				tcoords.set(y0, y1, x0, x1);
			}
			break;
		case  ERU_00_11:
		default:
			{
				tcoords.set(x0, x1, y0, y1);
			}
			break;
		}	

		//vertices
		rectf poss((float)destPos.X, 
			(float)destPos.Y, 
			(float)(destPos.X+(int)(sourceSize.Width * scale)), 
			(float)(destPos.Y+(int)(sourceSize.Height * scale)));

		vertices[ i*4 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.top()));

		vertices[ i*4+1 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.top()));

		vertices[ i*4+2 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.bottom()));

		vertices[ i*4+3 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.bottom()));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL_PTR);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam);
}

void COpenGLDrawServices::draw2DImageRect(ITexture* texture, const recti* destRect, const recti* sourceRect /* = NULL_PTR */, SColor color /* = SColor() */, E_RECT_UVCOORDS uvcoords /* = ERU_00_11 */, const S2DBlendParam& blendParam /* = S2DBlendParam::OpaqueSource() */)
{
	draw2DImageRectBatch(texture, &destRect, sourceRect? &sourceRect : NULL_PTR, 1, color, uvcoords, blendParam);
}

void COpenGLDrawServices::draw2DImageRectBatch(ITexture* texture, const recti* destRects[], const recti* sourceRects[], u32 batchCount, SColor color /*= SColor()*/, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, const S2DBlendParam& blendParam /*= S2DBlendParam::OpaqueSource()*/)
{
	if (batchCount > MaxImageBatch)
		batchCount = MaxImageBatch;

	SVertex_PCT* vertices = &ImageVertices[0];
	const dimension2du texSize = texture ? texture->getSize() : dimension2du(0, 0);

	for (u32 i=0; i<batchCount; ++i)
	{
		const recti poss = *destRects[i];

		rectf tcoords;
		float x0, x1, y0, y1;

		if (texture)
		{
			ASSERT(texSize.getArea() > 0);

			vector2di sourcePos = sourceRects ? sourceRects[i]->UpperLeftCorner : vector2di(0,0);
			dimension2di sourceSize = sourceRects ? dimension2di(sourceRects[i]->getWidth(), sourceRects[i]->getHeight()) : dimension2di((int)texSize.Width, (int)texSize.Height);

			x0 = ((float)sourcePos.X) / (float)texSize.Width;
			x1 = ((float)sourcePos.Y) / (float)texSize.Height;
			y0 = ((float)(sourcePos.X + sourceSize.Width)) / (float)texSize.Width;
			y1 = ((float)(sourcePos.Y + sourceSize.Height)) / (float)texSize.Height;
		}
		else
		{
			x0 = 0.0f;
			x1 = 0.0f;
			y0 = 1.0f;
			y1 = 1.0f;
		}

		switch(uvcoords)
		{
		case ERU_01_10:
			{
				tcoords.set(x0, y1, y0, x1);
			}
			break;
		case ERU_10_01:
			{
				tcoords.set(y0, x1, x0, y1);
			}
			break;
		case ERU_11_00:
			{
				tcoords.set(y0, y1, x0, x1);
			}
			break;
		case  ERU_00_11:
		default:
			{
				tcoords.set(x0, x1, y0, y1);
			}
			break;
		}	

		vertices[ i*4 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.top()));

		vertices[ i*4+1 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.top()));

		vertices[ i*4+2 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.bottom()));

		vertices[ i*4+3 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.bottom()));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL_PTR);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam);
}

void COpenGLDrawServices::draw2DImageRectBatch(ITexture* texture, const recti* destRects[], const recti* sourceRects[], const SColor* colors, u32 batchCount, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, const S2DBlendParam& blendParam /*= S2DBlendParam::OpaqueSource()*/)
{
	if (batchCount > MaxImageBatch)
		batchCount = MaxImageBatch;

	SVertex_PCT* vertices = &ImageVertices[0];
	const dimension2du texSize = texture ? texture->getSize() : dimension2du(0, 0);

	for (u32 i=0; i<batchCount; ++i)
	{
		const recti poss = *destRects[i];
		const SColor color = colors[i];

		rectf tcoords;
		float x0, x1, y0, y1;

		if (texture)
		{
			ASSERT(texSize.getArea() > 0);

			vector2di sourcePos = sourceRects ? sourceRects[i]->UpperLeftCorner : vector2di(0,0);
			dimension2di sourceSize = sourceRects ? dimension2di(sourceRects[i]->getWidth(), sourceRects[i]->getHeight()) : dimension2di((int)texSize.Width, (int)texSize.Height);

			x0 = ((float)sourcePos.X) / (float)texSize.Width;
			x1 = ((float)sourcePos.Y) / (float)texSize.Height;
			y0 = ((float)(sourcePos.X + sourceSize.Width)) / (float)texSize.Width;
			y1 = ((float)(sourcePos.Y + sourceSize.Height)) / (float)texSize.Height;
		}
		else
		{
			x0 = 0.0f;
			x1 = 0.0f;
			y0 = 1.0f;
			y1 = 1.0f;
		}

		switch(uvcoords)
		{
		case ERU_01_10:
			{
				tcoords.set(x0, y1, y0, x1);
			}
			break;
		case ERU_10_01:
			{
				tcoords.set(y0, x1, x0, y1);
			}
			break;
		case ERU_11_00:
			{
				tcoords.set(y0, y1, x0, x1);
			}
			break;
		case  ERU_00_11:
		default:
			{
				tcoords.set(x0, x1, y0, y1);
			}
			break;
		}	

		vertices[ i*4 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.top()));

		vertices[ i*4+1 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.top() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.top()));

		vertices[ i*4+2 ].set(
			vector3df((float)poss.left() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.left(), tcoords.bottom()));

		vertices[ i*4+3 ].set(
			vector3df((float)poss.right() + 0.5f, (float)poss.bottom() + 0.5f, 0),
			color,
			vector2df(tcoords.right(), tcoords.bottom()));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL_PTR);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam);
}

void COpenGLDrawServices::draw2DSquadBatch(ITexture* texture, const SVertex_PCT* verts, u32 numQuads, const S2DBlendParam& blendParam)
{
	u32 batchCount = numQuads;
	if (batchCount > MaxImageBatch)
	{
		batchCount = MaxImageBatch;
		ASSERT(false);			//exceed
	}

	Q_memcpy(&ImageVertices[0], MaxImageBatch * 4 * sizeof(SVertex_PCT), verts, numQuads * 4 * sizeof(SVertex_PCT));
	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL_PTR);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam, ZTest2DQuadEnable);
}

void COpenGLDrawServices::draw2DVertices(ITexture* texture, const SVertex_PCT* verts, u32 numVerts, const u16* indices, u32 numIndices, const S2DBlendParam& blendParam /*= S2DBlendParam::OpaqueSource()*/)
{
	if (numVerts > VertexLimit2D || numIndices > IndexLimit2D)
	{
		ASSERT(false);
		return;
	}

	Q_memcpy(Vertices2D, sizeof(SVertex_PCT) * VertexLimit2D, verts, sizeof(SVertex_PCT) * numVerts);
	Q_memcpy(Indices2D, sizeof(u16) * IndexLimit2D, indices, sizeof(u16) * numIndices);

	HWBufferServices->updateHardwareBuffer(VB2D, numVerts);
	HWBufferServices->updateHardwareBuffer(IB2D, numIndices);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL_PTR);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VB2D;
	bufferParam.ibuffer = IB2D;

	SDrawParam drawParam = {0};
	drawParam.numVertices = numVerts;
	drawParam.baseVertIndex = 0;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		numIndices / 3, drawParam,
		blendParam, ZTest2DQuadEnable);
}

void COpenGLDrawServices::add2DColor( const recti& rect, SColor color, const S2DBlendParam& blendParam )
{
	ITexture* texture = g_Engine->getManualTextureServices()->getManualTexture("$DefaultWhite");

	SQuadDrawBatchKey key(texture, blendParam);
	SQuadBatchDraw& batchDraw = m_2DQuadDrawMap[key];

	{
		SVertex_PCT v;
		v.set(vector3df((float)rect.left(), (float)rect.top(), 0.0f), color, vector2df(0.0f, 0.0f));
		v.Pos += vector3df(0.5f, 0.5f, 0);				//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		v.set(vector3df((float)rect.right(), (float)rect.top(), 0.0f), color, vector2df(1.0f, 0.0f));
		v.Pos += vector3df(0.5f, 0.5f, 0);				//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		v.set(vector3df((float)rect.left(), (float)rect.bottom(), 0.0f), color, vector2df(0.0f, 1.0f));
		v.Pos += vector3df(0.5f, 0.5f, 0);				//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		v.set(vector3df((float)rect.right(), (float)rect.bottom(), 0.0f), color, vector2df(1.0f, 1.0f));
		v.Pos += vector3df(0.5f, 0.5f, 0);				//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;
	}

	if (batchDraw.vertNum >= MaxImageBatch * 4)
	{
		//flush
		draw2DSquadBatch(key.texture, &batchDraw.drawVerts[0], (u32)batchDraw.vertNum / 4, key.blendParam);
		batchDraw.vertNum = 0;
	}
}

void COpenGLDrawServices::add2DQuads( ITexture* texture, const SVertex_PCT* vertices, u32 numQuads, const S2DBlendParam& blendParam )
{
	if (!texture)
		return;

	SQuadDrawBatchKey key(texture, blendParam);
	SQuadBatchDraw& batchDraw = m_2DQuadDrawMap[key];

	const SVertex_PCT* p = vertices;
	for (u32 i=0; i<numQuads; ++i)
	{		
		SVertex_PCT v = *p++;
		v.Pos.X += 0.5f;
		v.Pos.Y += 0.5f;	//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		v = *p++;
		v.Pos.X += 0.5f;
		v.Pos.Y += 0.5f;	//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		v = *p++;
		v.Pos.X += 0.5f;
		v.Pos.Y += 0.5f;	//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		v = *p++;
		v.Pos.X += 0.5f;
		v.Pos.Y += 0.5f;	//2D空间要偏移0.5像素
		batchDraw.drawVerts[batchDraw.vertNum] = v;
		++batchDraw.vertNum;

		if (batchDraw.vertNum >= MaxImageBatch * 4)
		{
			//flush
			draw2DSquadBatch(key.texture, &batchDraw.drawVerts[0], (u32)batchDraw.vertNum / 4, key.blendParam);
			batchDraw.vertNum = 0;
		}
	}
}

void COpenGLDrawServices::flushAll2DQuads()
{
	for (T_2DQuadDrawMap::iterator itr = m_2DQuadDrawMap.begin(); itr != m_2DQuadDrawMap.end(); ++itr)
	{
		const SQuadDrawBatchKey& key = itr->first;
		SQuadBatchDraw& batchDraw = itr->second;

		u32 numQuads = (u32)batchDraw.vertNum / 4;
		if (!numQuads)
			continue;

		draw2DSquadBatch(key.texture, &batchDraw.drawVerts[0], numQuads, key.blendParam);
		batchDraw.vertNum = 0;
	}
	m_2DQuadDrawMap.clear();
}

#endif