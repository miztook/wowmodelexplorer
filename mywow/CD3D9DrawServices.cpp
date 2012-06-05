#include "stdafx.h"
#include "CD3D9DrawServices.h"
#include "mywow.h"
#include "CD3D9Driver.h"
#include "CD3D9HardwareBufferServices.h"

static u16 imageIndices[6] = {0,1,2, 0,2,3};

CD3D9DrawServices::CD3D9DrawServices()
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	HWBufferServices = static_cast<CD3D9HardwareBufferServices*>(g_Engine->getHardwareBufferServices());

	Line2DVertexLimit = MAX_2DLINE_BATCH_COUNT * 2;
	Line3DVertexLimit = MAX_3DLINE_BATCH_COUNT * 2;
	ImageVertexLimit = MAX_IMAGE_BATCH_COUNT * 4;
	ImageIndexLimit = MAX_IMAGE_BATCH_COUNT * 6;

	LineMaterial.MaterialType = EMT_ONECOLOR;
	LineMaterial.Lighting = false;
	LineMaterial.ZWriteEnable = false;
	LineMaterial.ZBuffer = ECFN_ALWAYS;
	LineMaterial.AntiAliasing = true;

	Line2DVertices = (S3DVertexBasicColor*)Hunk_AllocateTempMemory(sizeof(S3DVertexBasicColor) *  Line2DVertexLimit);
	Line3DVertices = (S3DVertexBasicColor*)Hunk_AllocateTempMemory(sizeof(S3DVertexBasicColor) *  Line3DVertexLimit);
	ImageVertices = (S3DVertexBasicTex*)Hunk_AllocateTempMemory(sizeof(S3DVertexBasicTex) * ImageVertexLimit);
	ImageIndices = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * ImageIndexLimit);
	for (u16 i=0; i<MAX_IMAGE_BATCH_COUNT; ++i)
	{
		ImageIndices[i*6 + 0] = i*4 + 0;
		ImageIndices[i*6 + 1] = i*4 + 1;
		ImageIndices[i*6 + 2] = i*4 + 2;

		ImageIndices[i*6 + 3] = i*4 + 0;
		ImageIndices[i*6 + 4] = i*4 + 2;
		ImageIndices[i*6 + 5] = i*4 + 3;
	}

	VBLine2D = new IVertexBuffer(false);
	VBLine2D->set(Line2DVertices, EVT_BASICCOLOR, Line2DVertexLimit, EMM_DYNAMIC);
	VBLine3D = new IVertexBuffer(false);
	VBLine3D->set(Line3DVertices, EVT_BASICCOLOR, Line3DVertexLimit, EMM_DYNAMIC);
	VBImage = new IVertexBuffer(false);
	VBImage->set(ImageVertices, EVT_BASICTEX, ImageVertexLimit, EMM_DYNAMIC);
	IBImage = new IIndexBuffer(false);
	IBImage->set(ImageIndices, EIT_16BIT, ImageIndexLimit, EMM_STATIC);

	Line2DVertexCount = 0;
	Line3DVertexCount = 0;

	CurrentImageCount = 0;

	//hw buffer
 	HWBufferServices->createHardwareBuffer(VBLine2D);
 	HWBufferServices->createHardwareBuffer(VBLine3D);
 	HWBufferServices->createHardwareBuffer(VBImage);
 	HWBufferServices->createHardwareBuffer(IBImage);
	HWBufferServices->updateHardwareBuffer(IBImage, 0, MAX_IMAGE_BATCH_COUNT*6);
}

CD3D9DrawServices::~CD3D9DrawServices()
{
 	HWBufferServices->destroyHardwareBuffer(IBImage);
 	HWBufferServices->destroyHardwareBuffer(VBImage);
 	HWBufferServices->destroyHardwareBuffer(VBLine3D);
	HWBufferServices->destroyHardwareBuffer(VBLine2D);

	delete IBImage;
	delete VBImage;
	delete VBLine3D;
	delete VBLine2D;

	Hunk_FreeTempMemory(ImageIndices);
	Hunk_FreeTempMemory(ImageVertices);
	Hunk_FreeTempMemory(Line3DVertices);
	Hunk_FreeTempMemory(Line2DVertices);
}

void CD3D9DrawServices::add2DLine(line2di line, SColor color)
{
	if (Line2DVertexCount >= Line2DVertexLimit -2)
		return;//flushAll2DLines();

	S3DVertexBasicColor v0, v1;
	v0.Pos.set((f32)line.start.X, (f32)line.start.Y, 0);
	v1.Pos.set((f32)line.end.X, (f32)line.end.Y, 0);
	v0.Color = v1.Color = color;

	Line2DVertices[Line2DVertexCount++] = v0;
	Line2DVertices[Line2DVertexCount++] = v1;
}

void CD3D9DrawServices::flushAll2DLines()
{
	if (Line2DVertexCount == 0)
		return;

	HWBufferServices->updateHardwareBuffer(VBLine2D, 0, Line2DVertexCount);

	Driver->draw2DMode(VBLine2D, EPT_LINES, Line2DVertexCount/2, 0, 0, false, false, false);

	Line2DVertexCount = 0;
}

void CD3D9DrawServices::add3DLine( line3df line, SColor color )
{
	if (Line3DVertexCount >= Line3DVertexLimit -2)
		return; //flushAll3DLines();

	S3DVertexBasicColor v0, v1;
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
	Driver->setTransform(ETS_VIEW, cam->getViewMatrix());
	Driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
	Driver->setMaterial(LineMaterial);

	Driver->draw3DMode(VBLine3D, EPT_LINES, Line3DVertexCount/2, 0, 0);

	Line3DVertexCount = 0;
}

void CD3D9DrawServices::draw2DImage( ITexture* texture, vector2di destPos, bool alphaChannel )
{
	if (!texture)
		return;

	recti rc(0,0,texture->getSize().Width, texture->getSize().Height);
	draw2DImage(texture, destPos, alphaChannel, &rc, SColor(), 1.0f);
}

void CD3D9DrawServices::draw2DImage( ITexture* texture, vector2di destPos, bool useAlphaChannel, const recti* sourceRect, SColor color, f32 scale /*= 1.0f*/ )
{
	if (!texture)
		return;

	vector2di sourcePos = sourceRect->UpperLeftCorner;
	dimension2di sourceSize(sourceRect->getSize());

	rectf tcoords;
	tcoords.UpperLeftCorner.X = ((f32)sourcePos.X) / (f32)texture->getSize().Width;
	tcoords.UpperLeftCorner.Y = ((f32)sourcePos.Y) / (f32)texture->getSize().Height;
	tcoords.LowerRightCorner.X = ((f32)(sourcePos.X + sourceSize.Width)) / (f32)texture->getSize().Width;
	tcoords.LowerRightCorner.Y = ((f32)(sourcePos.Y + sourceSize.Height)) / (f32)texture->getSize().Height;

	const recti poss(destPos.X, destPos.Y, destPos.X+(s32)(sourceSize.Width * scale), destPos.Y+(s32)(sourceSize.Height * scale));

	SingleImageVertices[0].set(
		vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0),
		color,
		vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));

	SingleImageVertices[1].set(
		vector3df((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0),
		color,
		vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));

	SingleImageVertices[2].set(
		vector3df((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0),
		color,
		vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));

	SingleImageVertices[3].set(
		vector3df((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0),
		color,
		vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

	Driver->setTexture(0, texture);
	Driver->setTexture(1, NULL);

	Driver->draw2DModeUP(SingleImageVertices, EVT_BASICTEX, imageIndices, EIT_16BIT, 6, 0, 4, EPT_TRIANGLES, 
		color.getAlpha()<255, true, useAlphaChannel);

}

void CD3D9DrawServices::draw2DImageBatch( ITexture* texture, const vector2di* positions, const recti** sourceRects, u32 batchCount, f32 scale /*= 1.0f*/, SColor color /*= SColor()*/, bool useAlphachannel /*= false*/ )
{
	if (!texture)
		return;

	if (batchCount > MAX_IMAGE_BATCH_COUNT)
		batchCount = MAX_IMAGE_BATCH_COUNT;

	if (CurrentImageCount + batchCount > MAX_IMAGE_BATCH_COUNT)
		CurrentImageCount = 0;

	S3DVertexBasicTex* vertices = &ImageVertices[CurrentImageCount * 4];

	for (u32 i=0; i<batchCount; ++i)
	{
		vector2di destPos = positions[i];
		vector2di sourcePos = sourceRects[i]->UpperLeftCorner;

		dimension2di sourceSize(sourceRects[i]->getSize());

		rectf tcoords;
		tcoords.UpperLeftCorner.X = ((f32)sourcePos.X) / (f32)texture->getSize().Width;
		tcoords.UpperLeftCorner.Y = ((f32)sourcePos.Y) / (f32)texture->getSize().Height;
		tcoords.LowerRightCorner.X = ((f32)(sourcePos.X + sourceSize.Width)) / (f32)texture->getSize().Width;
		tcoords.LowerRightCorner.Y = ((f32)(sourcePos.Y + sourceSize.Height)) / (f32)texture->getSize().Height;

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

	SDrawParam drawParam = {0};
 	drawParam.numVertices = batchCount * 4;
 	drawParam.baseVertIndex = CurrentImageCount * 4;

	Driver->draw2DMode(VBImage, IBImage, EPT_TRIANGLES,
		 batchCount * 2, drawParam,
		true, true, useAlphachannel);

	CurrentImageCount += batchCount;
}

void CD3D9DrawServices::draw3DVerts( vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color, const matrix4& world )
{
	S3DVertexBasicColor* v = (S3DVertexBasicColor*)Hunk_AllocateTempMemory(sizeof(S3DVertexBasicColor) * numverts);

	for (u32 i=0; i<numverts; ++i)
	{
		v[i].Pos = verts[i];
		v[i].Color = color;
	}

	Driver->setTransform(ETS_WORLD, world);
	Driver->setMaterial(LineMaterial);
	Driver->draw3DModeUP(v, EVT_BASICCOLOR, indices, EIT_16BIT, numindices, 0, numverts, EPT_TRIANGLES);

	Hunk_FreeTempMemory(v);
}

