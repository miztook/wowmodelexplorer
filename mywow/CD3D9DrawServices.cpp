#include "stdafx.h"
#include "CD3D9DrawServices.h"
#include "mywow.h"
#include "CD3D9Driver.h"
#include "CD3D9HardwareBufferServices.h"

CD3D9DrawServices::CD3D9DrawServices()
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	HWBufferServices = static_cast<CD3D9HardwareBufferServices*>(g_Engine->getHardwareBufferServices());

	Line2DVertexLimit = MAX_2DLINE_BATCH_COUNT * 2;
	Line3DVertexLimit = MAX_3DLINE_BATCH_COUNT * 2;
	ImageVertexLimit = MAX_IMAGE_BATCH_COUNT * 4;
	ImageIndexLimit = MAX_IMAGE_BATCH_COUNT * 6;
	VertexLimit = MAX_VERTEX_COUNT;
	IndexLimit = MAX_INDEX_COUNT;

	//line material
	LineMaterial.MaterialType = EMT_ONECOLOR;
	LineMaterial.Lighting = false;
	LineMaterial.ZWriteEnable = false;
	LineMaterial.ZBuffer = ECFN_ALWAYS;
	LineMaterial.AntiAliasing = EAAM_LINE_SMOOTH;

	Line2DVertices = new SGVertex_PC[Line2DVertexLimit];
	Line3DVertices = new SGVertex_PC[Line3DVertexLimit];
	ImageVertices = new SGTVertex_PC1T[ImageVertexLimit];
	ImageIndices = new u16[ImageIndexLimit];
	for (u16 i=0; i<MAX_IMAGE_BATCH_COUNT; ++i)
	{
		ImageIndices[i*6 + 0] = i*4 + 0;
		ImageIndices[i*6 + 1] = i*4 + 1;
		ImageIndices[i*6 + 2] = i*4 + 2;

		ImageIndices[i*6 + 3] = i*4 + 0;
		ImageIndices[i*6 + 4] = i*4 + 2;
		ImageIndices[i*6 + 5] = i*4 + 3;
	}
	Vertices = new SGVertex_PC[VertexLimit];
	Indices = new u16[IndexLimit];

	VBLine2D = new IVertexBuffer();
	VBLine2D->set(Line2DVertices, EST_G_PC, Line2DVertexLimit, EMM_DYNAMIC);
	VBLine3D = new IVertexBuffer();
	VBLine3D->set(Line3DVertices, EST_G_PC, Line3DVertexLimit, EMM_DYNAMIC);
	VBImage = new IVertexBuffer();
	VBImage->set(ImageVertices, EST_GT_PC_1T, ImageVertexLimit, EMM_DYNAMIC);
	IBImage = new IIndexBuffer();
	IBImage->set(ImageIndices, EIT_16BIT, ImageIndexLimit, EMM_STATIC);
	VB3D = new IVertexBuffer();
	VB3D->set(Vertices, EST_G_PC, VertexLimit, EMM_DYNAMIC);
	IB3D = new IIndexBuffer();
	IB3D->set(Indices, EIT_16BIT, IndexLimit, EMM_DYNAMIC);

	Line2DVertexCount = 0;
	Line3DVertexCount = 0;

	CurrentImageCount = 0;
	CurrentIndex3DCount = 0;
	CurrentVertex3DCount = 0;

	//hw buffer
 	HWBufferServices->createHardwareBuffer(VBLine2D);
 	HWBufferServices->createHardwareBuffer(VBLine3D);
 	HWBufferServices->createHardwareBuffer(VBImage);
 	HWBufferServices->createHardwareBuffer(IBImage);
	HWBufferServices->updateHardwareBuffer(IBImage, 0, MAX_IMAGE_BATCH_COUNT*6);
	HWBufferServices->createHardwareBuffer(VB3D);
	HWBufferServices->createHardwareBuffer(IB3D);
}

CD3D9DrawServices::~CD3D9DrawServices()
{
	HWBufferServices->destroyHardwareBuffer(IB3D);
	HWBufferServices->destroyHardwareBuffer(VB3D);
 	HWBufferServices->destroyHardwareBuffer(IBImage);
 	HWBufferServices->destroyHardwareBuffer(VBImage);
 	HWBufferServices->destroyHardwareBuffer(VBLine3D);
	HWBufferServices->destroyHardwareBuffer(VBLine2D);

	delete IB3D;
	delete VB3D;
	delete IBImage;
	delete VBImage;
	delete VBLine3D;
	delete VBLine2D;
}

void CD3D9DrawServices::add2DLine(line2di line, SColor color)
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	SGVertex_PC v0, v1;
	v0.Pos.set((f32)line.start.X, (f32)line.start.Y, 0);
	v1.Pos.set((f32)line.end.X, (f32)line.end.Y, 0);
	v0.Color = v1.Color = color;

	Line2DVertices[Line2DVertexCount++] = v0;
	Line2DVertices[Line2DVertexCount++] = v1;
}

void CD3D9DrawServices::add2DRect( const recti& rect, SColor color )
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	SGVertex_PC v0, v1, v2, v3;
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

void CD3D9DrawServices::flushAll2DLines()
{
	if (Line2DVertexCount == 0)
		return;

	HWBufferServices->updateHardwareBuffer(VBLine2D, 0, Line2DVertexCount);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_BASICCOLOR;
	bufferParam.vbuffer0 = VBLine2D;

	SDrawParam drawParam = {0};
	drawParam.startIndex = 0;
	drawParam.voffset0 = 0;

	Driver->draw2DMode(bufferParam, EPT_LINES, Line2DVertexCount/2, drawParam, false, false, EBF_ZERO, EBF_ZERO);

	Line2DVertexCount = 0;
}

void CD3D9DrawServices::add3DLine( line3df line, SColor color )
{
	if (Line3DVertexCount >= Line3DVertexLimit -2)
		return; //flushAll3DLines();

	SGVertex_PC v0, v1;
	v0.Pos = line.start;
	v1.Pos = line.end;
	v0.Color = v1.Color = color;

	Line3DVertices[Line3DVertexCount++] = v0;
	Line3DVertices[Line3DVertexCount++] = v1;
}

void CD3D9DrawServices::add3DBox( const aabbox3df& box, SColor color )
{
	if (box.isZero() || Line3DVertexCount >= Line3DVertexLimit - 24)
		return;//flushAll3DLines();

	vector3df points[8];
	box.makePoints(points);

	u32 index = 0;
	for (u32 i=0; i<24; ++i)
	{
		index = g_aabboxLineIndex[i];
		Line3DVertices[Line3DVertexCount++].set(points[index], color);
	}
}

void CD3D9DrawServices::addSphere( vector3df center, f32 radius, SColor color, u32 hori /*= 10*/, u32 vert /*= 6*/ )
{
	u32 polyCountX = min_(hori, 20u);
	u32 polyCountY = min_(vert, 10u);

	const f32 AngleX = 2 * PI / polyCountX;
	const f32 AngleY = PI / polyCountY;

	vector3df vpos[21][10];

	f32 axz;
	f32 ay = 0;

	for (u32 y = 0; y <= polyCountY; ++y)
	{
		const f32 sinay = sin(ay);
		axz = 0;

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

void CD3D9DrawServices::flushAll3DLines(ICamera* cam)
{
	if (Line3DVertexCount == 0)
		return;

	HWBufferServices->updateHardwareBuffer(VBLine3D, 0, Line3DVertexCount);

	Driver->setTransform(ETS_WORLD, matrix4(true));
	if (cam)
	{
		Driver->setTransform(ETS_VIEW, cam->getViewMatrix());
		Driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
	}
	Driver->setMaterial(LineMaterial);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_BASICCOLOR;
	bufferParam.vbuffer0 = VBLine3D;

	SDrawParam drawParam = {0};
	drawParam.startIndex = 0;
	drawParam.voffset0 = 0;

	Driver->draw3DMode(bufferParam, EPT_LINES, Line3DVertexCount/2, drawParam);

	Line3DVertexCount = 0;
}

void CD3D9DrawServices::draw2DSolid( const recti& rect, SColor color, f32 scale /*= 1.0f*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
{
	vector2di destPos = rect.UpperLeftCorner;
	recti rc(0,0,rect.getWidth(), rect.getHeight());

	draw2DImage(NULL, destPos, false, &rc, color, scale, srcBlend, destBlend);
}

void CD3D9DrawServices::draw2DImage( ITexture* texture, vector2di destPos, bool alphaChannel, f32 scale,E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend )
{
	if (!texture)
		return;

	recti rc(0,0,texture->getSize().Width, texture->getSize().Height);
	draw2DImage(texture, destPos, alphaChannel, &rc, SColor(), scale, srcBlend, destBlend);
}

void CD3D9DrawServices::draw2DImage( ITexture* texture, vector2di destPos, bool useAlphaChannel, const recti* sourceRect, SColor color, f32 scale /*= 1.0f*/, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend )
{
	draw2DImageBatch(texture, &destPos, &sourceRect, 1, scale, color, false, srcBlend, destBlend);
}

void CD3D9DrawServices::draw2DImageBatch( ITexture* texture, const vector2di* positions, const recti** sourceRects, u32 batchCount, f32 scale /*= 1.0f*/, SColor color /*= SColor()*/, bool useAlphachannel /*= false*/, E_BLEND_FACTOR srcBlend/*=EBF_ONE*/, E_BLEND_FACTOR destBlend/*=EBF_ONE_MINUS_SRC_ALPHA*/ )
{
	if (batchCount > MAX_IMAGE_BATCH_COUNT)
		batchCount = MAX_IMAGE_BATCH_COUNT;

	if (CurrentImageCount + batchCount > MAX_IMAGE_BATCH_COUNT)
		CurrentImageCount = 0;

	SGTVertex_PC1T* vertices = &ImageVertices[CurrentImageCount * 4];

	for (u32 i=0; i<batchCount; ++i)
	{
		vector2di destPos = positions[i];
		vector2di sourcePos = sourceRects[i]->UpperLeftCorner;

		dimension2di sourceSize(sourceRects[i]->getSize());

		rectf tcoords;
		if (texture)
		{
			tcoords.UpperLeftCorner.X = ((f32)sourcePos.X) / (f32)texture->getSize().Width;
			tcoords.UpperLeftCorner.Y = ((f32)sourcePos.Y) / (f32)texture->getSize().Height;
			tcoords.LowerRightCorner.X = ((f32)(sourcePos.X + sourceSize.Width)) / (f32)texture->getSize().Width;
			tcoords.LowerRightCorner.Y = ((f32)(sourcePos.Y + sourceSize.Height)) / (f32)texture->getSize().Height;
		}
		else
		{
			tcoords.UpperLeftCorner.X = 0.0f;
			tcoords.UpperLeftCorner.Y = 0.0f;
			tcoords.LowerRightCorner.X = 1.0f;
			tcoords.LowerRightCorner.Y = 1.0f;
		}

		const recti poss(destPos.X, destPos.Y, destPos.X+(s32)(sourceSize.Width * scale), destPos.Y+(s32)(sourceSize.Height * scale));

		vertices[ i*4 ].set(
			vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0),
			color,
			vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));

		vertices[ i*4+1 ].set(
			vector3df((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0),
			color,
			vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));

		vertices[ i*4+2 ].set(
			vector3df((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0),
			color,
			vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));

		vertices[ i*4+3 ].set(
			vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0),
			color,
			vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));
	}

	HWBufferServices->updateHardwareBuffer(VBImage, CurrentImageCount * 4, batchCount * 4);

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_BASICTEX_S;
	bufferParam.vbuffer0 = VBImage;
	bufferParam.ibuffer = IBImage;

	SDrawParam drawParam = {0};
 	drawParam.numVertices = batchCount * 4;
 	drawParam.baseVertIndex = CurrentImageCount * 4;

	Driver->draw2DMode(bufferParam, EPT_TRIANGLES,
		 batchCount * 2, drawParam,
		true, useAlphachannel, srcBlend, destBlend);

	CurrentImageCount += batchCount;
}

void CD3D9DrawServices::add3DVertices( vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color )
{
	if (numverts + CurrentVertex3DCount >= VertexLimit ||
		numindices + CurrentIndex3DCount >= IndexLimit)
		return;

	for(u32 i=0; i<numverts; ++i)
	{
		Vertices[CurrentVertex3DCount + i].Pos = verts[i];
		Vertices[CurrentVertex3DCount + i].Color = color;
	}

	memcpy_s(&Indices[CurrentIndex3DCount], sizeof(u16)*numindices, indices, sizeof(u16)*numindices);

	CurrentVertex3DCount += numverts;
	CurrentIndex3DCount += numindices;
}

void CD3D9DrawServices::flushAll3DVertices( ICamera* cam )
{
	if (!CurrentVertex3DCount || !CurrentIndex3DCount)
		return;

	HWBufferServices->updateHardwareBuffer(VB3D, 0, CurrentVertex3DCount);
	HWBufferServices->updateHardwareBuffer(IB3D, 0, CurrentIndex3DCount);

	Driver->setTransform(ETS_WORLD, matrix4(true));
	if (cam)
	{
		Driver->setTransform(ETS_VIEW, cam->getViewMatrix());
		Driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
	}
	Driver->setMaterial(LineMaterial);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_BASICCOLOR;
	bufferParam.vbuffer0 = VB3D;
	bufferParam.ibuffer = IB3D;

	SDrawParam drawParam = {0};
	drawParam.numVertices = CurrentVertex3DCount;
	drawParam.baseVertIndex = 0;
	
	Driver->draw3DMode(bufferParam, EPT_TRIANGLES, CurrentIndex3DCount/3, drawParam);

	CurrentVertex3DCount = CurrentIndex3DCount = 0;
}

