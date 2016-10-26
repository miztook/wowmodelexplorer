#include "stdafx.h"
#include "CGLES2DrawServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2HardwareBufferServices.h"

CGLES2DrawServices::CGLES2DrawServices()
{
	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	HWBufferServices = static_cast<CGLES2HardwareBufferServices*>(g_Engine->getHardwareBufferServices());

	MaxImageBatch = min_(MAX_IMAGE_BATCH_COUNT(), IHardwareBufferServices::MAX_QUADS());

	Line2DVertexLimit = MAX_2DLINE_BATCH_COUNT() * 2;
	Line3DVertexLimit = MAX_3DLINE_BATCH_COUNT() * 2;
	ImageVertexLimit = MaxImageBatch * 4;
	VertexLimit = MAX_VERTEX_COUNT();
	IndexLimit = MAX_INDEX_COUNT();

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
	Vertices = new SVertex_PC[VertexLimit];
	Indices = new u16[IndexLimit];

	VBLine2D = new IVertexBuffer(false);
	VBLine2D->set(Line2DVertices, EST_PC, Line2DVertexLimit, EMM_DYNAMIC);
	VBLine3D = new IVertexBuffer(false);
	VBLine3D->set(Line3DVertices, EST_PC, Line3DVertexLimit, EMM_DYNAMIC);
	VBImage = new IVertexBuffer(false);
	VBImage->set(ImageVertices, EST_PCT, ImageVertexLimit, EMM_DYNAMIC);
	VB3D = new IVertexBuffer(false);
	VB3D->set(Vertices, EST_PC, VertexLimit, EMM_DYNAMIC);
	IB3D = new IIndexBuffer(false);
	IB3D->set(Indices, EIT_16BIT, IndexLimit, EMM_DYNAMIC);

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
}

CGLES2DrawServices::~CGLES2DrawServices()
{
	HWBufferServices->destroyHardwareBuffer(IB3D);
	HWBufferServices->destroyHardwareBuffer(VB3D);
	HWBufferServices->destroyHardwareBuffer(VBImage);
	HWBufferServices->destroyHardwareBuffer(VBLine3D);
	HWBufferServices->destroyHardwareBuffer(VBLine2D);

	//buffer
	delete IB3D;
	delete VB3D;
	delete VBImage;
	delete VBLine3D;
	delete VBLine2D;

	//vertices, indices
	delete[] Indices;
	delete[] Vertices;
	delete[] ImageVertices;
	delete[] Line3DVertices;
	delete[] Line2DVertices;
}

void CGLES2DrawServices::add2DLine( const line2di& line, SColor color )
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	SVertex_PC v0, v1;
	v0.Pos.set((f32)line.start.X, (f32)line.start.Y, 0);
	v1.Pos.set((f32)line.end.X, (f32)line.end.Y, 0);
	v0.Color = v1.Color = color;

	Line2DVertices[Line2DVertexCount++] = v0;
	Line2DVertices[Line2DVertexCount++] = v1;
}

void CGLES2DrawServices::add2DRect( const recti& rect, SColor color )
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	SVertex_PC v0, v1, v2, v3;
	v0.Pos.set((f32)rect.UpperLeftCorner.X, (f32)rect.UpperLeftCorner.Y, 0);
	v1.Pos.set((f32)rect.LowerRightCorner.X, (f32)rect.UpperLeftCorner.Y, 0);
	v2.Pos.set((f32)rect.LowerRightCorner.X, (f32)rect.LowerRightCorner.Y, 0);
	v3.Pos.set((f32)rect.UpperLeftCorner.X, (f32)rect.LowerRightCorner.Y, 0);
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

void CGLES2DrawServices::flushAll2DLines()
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

	S2DBlendParam blendParam = {0};

	Driver->draw2DMode(bufferParam, EPT_LINES, Line2DVertexCount/2, drawParam, blendParam);

	Line2DVertexCount = 0;
}

void CGLES2DrawServices::add3DLine( const line3df& line, SColor color )
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

void CGLES2DrawServices::add3DBox( const aabbox3df& box, SColor color )
{
	if (box.isZero() || Line3DVertexCount >= Line3DVertexLimit - 24)
		return;//flushAll3DLines();

	vector3df points[8];
	box.makePoints(points);

	for (u32 i=0; i<24; ++i)
	{
		u32 index = g_aabboxLineIndex[i];
		Line3DVertices[Line3DVertexCount++].set(points[index], color);
	}
}

void CGLES2DrawServices::addSphere( vector3df center, f32 radius, SColor color, u32 hori /*= 10*/, u32 vert /*= 6*/ )
{
	u32 polyCountX = min_(hori, 20u);
	u32 polyCountY = min_(vert, 10u);

	const f32 AngleX = 2 * PI / polyCountX;
	const f32 AngleY = PI / polyCountY;

	vector3df vpos[21][10];

	f32 ay = 0;

	for (u32 y = 0; y <= polyCountY; ++y)
	{
		const f32 sinay = sin(ay);
		f32 axz = 0;

		// calculate the necessary vertices without the doubled one
		vector3df lastp;
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position
			vector3df pos(static_cast<f32>(radius * cos(axz) * sinay),
				static_cast<f32>(radius * cos(ay)),
				static_cast<f32>(radius * sin(axz) * sinay));
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

void CGLES2DrawServices::flushAll3DLines( ICamera* cam )
{
	if (Line3DVertexCount == 0)
		return;

	HWBufferServices->updateHardwareBuffer(VBLine3D, Line3DVertexCount);

	if (cam)
	{
		Driver->setTransform_Material_Textures(matrix4::Identity(),
			cam->getViewMatrix(),
			cam->getProjectionMatrix(),
			LineMaterial,
			NULL, 
			0);
	}
	else
	{
		Driver->setTransform_Material_Textures(matrix4::Identity(),
			LineMaterial,
			NULL, 
			0);
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

void CGLES2DrawServices::add3DVertices( vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color )
{
	if (numverts + CurrentVertex3DCount >= VertexLimit ||
		numindices + CurrentIndex3DCount >= IndexLimit)
		return;

	for(u32 i=0; i<numverts; ++i)
	{
		Vertices[CurrentVertex3DCount + i].Pos = verts[i];
		Vertices[CurrentVertex3DCount + i].Color = color;
	}

	for (u32 i=0; i<numindices; ++i)
	{
		Indices[CurrentIndex3DCount + i] = indices[i] + (u16)CurrentVertex3DCount;
	}

	CurrentVertex3DCount += numverts;
	CurrentIndex3DCount += numindices;
}

void CGLES2DrawServices::add3DVertices( vector3df* verts, u32 numverts, SColor color )
{
	if (numverts + CurrentVertex3DCount >= VertexLimit ||
		numverts + CurrentIndex3DCount >= IndexLimit)
		return;

	for(u32 i=0; i<numverts; ++i)
	{
		Vertices[CurrentVertex3DCount + i].Pos = verts[i];
		Vertices[CurrentVertex3DCount + i].Color = color;

		Indices[CurrentIndex3DCount + i] = i + (u16)CurrentVertex3DCount;
	}

	CurrentVertex3DCount += numverts;
	CurrentIndex3DCount += numverts;
}

void CGLES2DrawServices::flushAll3DVertices( ICamera* cam )
{
	if (!CurrentVertex3DCount || !CurrentIndex3DCount)
		return;

	HWBufferServices->updateHardwareBuffer(VB3D, CurrentVertex3DCount);
	HWBufferServices->updateHardwareBuffer(IB3D, CurrentIndex3DCount);

	Driver->setTransform_Material_Textures(matrix4::Identity(),
		cam->getViewMatrix(),
		cam->getProjectionMatrix(),
		LineMaterial,
		NULL, 
		0);

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

void CGLES2DrawServices::draw2DColor( const recti& rect, SColor color, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, f32 scale /*= 1.0f*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
{
	vector2di destPos = rect.UpperLeftCorner;
	recti rc(0,0,rect.getWidth(), rect.getHeight());

	draw2DImage(NULL, destPos, &rc, color, uvcoords, scale, false, srcBlend, destBlend);
}

void CGLES2DrawServices::draw2DImage( ITexture* texture, vector2di destPos, const recti* sourceRect /*= NULL*/, SColor color /*= SColor()*/, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, f32 scale /*= 1.0f*/, bool useAlphaChannel /*= false*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
{
	draw2DImageBatch(texture, &destPos, sourceRect ? &sourceRect : NULL, 1, color, uvcoords, scale, false, srcBlend, destBlend);
}

void CGLES2DrawServices::draw2DImageBatch( ITexture* texture, const vector2di* positions, const recti* sourceRects[], u32 batchCount, SColor color /*= SColor()*/, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, f32 scale /*= 1.0f*/, bool useAlphachannel /*= false*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
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
		dimension2di sourceSize = sourceRects ? sourceRects[i]->getSize() : dimension2di((s32)texSize.Width, (s32)texSize.Height);

		//tcoords
		rectf tcoords;
		f32 x0, x1, y0, y1;

		if (texture)
		{
			ASSERT(texSize.getArea() > 0);

			x0 = ((f32)sourcePos.X) / (f32)texSize.Width;
			x1 = ((f32)sourcePos.Y) / (f32)texSize.Height;
			y0 = ((f32)(sourcePos.X + sourceSize.Width)) / (f32)texSize.Width;
			y1 = ((f32)(sourcePos.Y + sourceSize.Height)) / (f32)texSize.Height;
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
				tcoords.UpperLeftCorner.X = x0;
				tcoords.UpperLeftCorner.Y = y1;
				tcoords.LowerRightCorner.X = y0;
				tcoords.LowerRightCorner.Y = x1;
			}
			break;
		case ERU_10_01:
			{
				tcoords.UpperLeftCorner.X = y0;
				tcoords.UpperLeftCorner.Y = x1;
				tcoords.LowerRightCorner.X = x0;
				tcoords.LowerRightCorner.Y = y1;
			}
			break;
		case ERU_11_00:
			{
				tcoords.UpperLeftCorner.X = y0;
				tcoords.UpperLeftCorner.Y = y1;
				tcoords.LowerRightCorner.X = x0;
				tcoords.LowerRightCorner.Y = x1;
			}
			break;
		case  ERU_00_11:
		default:
			{
				tcoords.UpperLeftCorner.X = x0;
				tcoords.UpperLeftCorner.Y = x1;
				tcoords.LowerRightCorner.X = y0;
				tcoords.LowerRightCorner.Y = y1;
			}
			break;
		}	

		//vertices
		const recti poss(destPos.X, destPos.Y, destPos.X+(s32)(sourceSize.Width * scale), destPos.Y+(s32)(sourceSize.Height * scale));

		vertices[ i*4 ].set(
			vector3df((f32)poss.UpperLeftCorner.X + 0.5f, (f32)poss.UpperLeftCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));

		vertices[ i*4+1 ].set(
			vector3df((f32)poss.LowerRightCorner.X + 0.5f, (f32)poss.UpperLeftCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));

		vertices[ i*4+2 ].set(
			vector3df((f32)poss.LowerRightCorner.X + 0.5f, (f32)poss.LowerRightCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));

		vertices[ i*4+3 ].set(
			vector3df((f32)poss.UpperLeftCorner.X + 0.5f, (f32)poss.LowerRightCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	S2DBlendParam blendParam = {0};
	blendParam.alpha = color.getAlpha() < 255;
	blendParam.alphaChannel = useAlphachannel;
	blendParam.srcBlend = srcBlend;
	blendParam.destBlend = destBlend;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam);

//	CurrentImageCount += batchCount;
}

void CGLES2DrawServices::draw2DImageRect( ITexture* texture, const recti* destRect, const recti* sourceRect /*= NULL*/, SColor color /*= SColor()*/, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, bool useAlphaChannel /*= false*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
{
	draw2DImageRectBatch(texture, &destRect, sourceRect? &sourceRect : NULL, 1, color, uvcoords, useAlphaChannel, srcBlend, destBlend);
}

void CGLES2DrawServices::draw2DImageRectBatch( ITexture* texture, const recti* destRects[], const recti* sourceRects[], u32 batchCount, SColor color /*= SColor()*/, E_RECT_UVCOORDS uvcoords /*= ERU_00_11*/, bool useAlphachannel /*= false*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
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
		const recti poss = *destRects[i];

		rectf tcoords;
		f32 x0, x1, y0, y1;

		if (texture)
		{
			ASSERT(texSize.getArea() > 0);

			vector2di sourcePos = sourceRects ? sourceRects[i]->UpperLeftCorner : vector2di(0,0);
			dimension2di sourceSize = sourceRects ? sourceRects[i]->getSize() : dimension2di((s32)texSize.Width, (s32)texSize.Height);

			x0 = ((f32)sourcePos.X) / (f32)texSize.Width;
			x1 = ((f32)sourcePos.Y) / (f32)texSize.Height;
			y0 = ((f32)(sourcePos.X + sourceSize.Width)) / (f32)texSize.Width;
			y1 = ((f32)(sourcePos.Y + sourceSize.Height)) / (f32)texSize.Height;
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
				tcoords.UpperLeftCorner.X = x0;
				tcoords.UpperLeftCorner.Y = y1;
				tcoords.LowerRightCorner.X = y0;
				tcoords.LowerRightCorner.Y = x1;
			}
			break;
		case ERU_10_01:
			{
				tcoords.UpperLeftCorner.X = y0;
				tcoords.UpperLeftCorner.Y = x1;
				tcoords.LowerRightCorner.X = x0;
				tcoords.LowerRightCorner.Y = y1;
			}
			break;
		case ERU_11_00:
			{
				tcoords.UpperLeftCorner.X = y0;
				tcoords.UpperLeftCorner.Y = y1;
				tcoords.LowerRightCorner.X = x0;
				tcoords.LowerRightCorner.Y = x1;
			}
			break;
		case  ERU_00_11:
		default:
			{
				tcoords.UpperLeftCorner.X = x0;
				tcoords.UpperLeftCorner.Y = x1;
				tcoords.LowerRightCorner.X = y0;
				tcoords.LowerRightCorner.Y = y1;
			}
			break;
		}	

		vertices[ i*4 ].set(
			vector3df((f32)poss.UpperLeftCorner.X + 0.5f, (f32)poss.UpperLeftCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));

		vertices[ i*4+1 ].set(
			vector3df((f32)poss.LowerRightCorner.X + 0.5f, (f32)poss.UpperLeftCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));

		vertices[ i*4+2 ].set(
			vector3df((f32)poss.LowerRightCorner.X + 0.5f, (f32)poss.LowerRightCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));

		vertices[ i*4+3 ].set(
			vector3df((f32)poss.UpperLeftCorner.X + 0.5f, (f32)poss.LowerRightCorner.Y + 0.5f, 0),
			color,
			vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = HWBufferServices->getStaticIndexBufferQuadList();

	SDrawParam drawParam = {0};
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	S2DBlendParam blendParam = {0};
	blendParam.alpha = color.getAlpha() < 255;
	blendParam.alphaChannel = useAlphachannel;
	blendParam.srcBlend = srcBlend;
	blendParam.destBlend = destBlend;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		batchCount * 2, drawParam,
		blendParam);

//	CurrentImageCount += batchCount;
}

#endif