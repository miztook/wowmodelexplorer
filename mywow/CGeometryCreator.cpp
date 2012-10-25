#include "stdafx.h"
#include "CGeometryCreator.h"
#include "mywow.h"

void CGeometryCreator::getCubeMeshVICount( u32& vcount, u32& icount )
{
	vcount = 36;
	icount = 12;
}

bool CGeometryCreator::fillCubeMeshVI( SGTVertex_PC1T* vertices, u32 vcount, u16* indices, u32 icount, const vector3df& size, SColor color /*= SColor()*/ )
{
	u32 vLimit, iLimit;
	getCubeMeshVICount(vLimit, iLimit);

	if (vcount < vLimit || icount < iLimit)
		return false;

	const u16 u[36] = {  0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
		7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7 };

	memcpy_s(indices, sizeof(u16)*36, u, sizeof(u));

	vertices[0].set(vector3df(0,0,0), color, vector2df(0, 1));
	vertices[1].set(vector3df(1,0,0), color, vector2df(1, 1));
	vertices[2].set(vector3df(1,1,0), color, vector2df(1, 0));
	vertices[3].set(vector3df(0,1,0), color, vector2df(0, 0));
	vertices[4].set(vector3df(1,0,1), color, vector2df(0, 1));
	vertices[5].set(vector3df(1,1,1), color, vector2df(0, 0));
	vertices[6].set(vector3df(0,1,1), color, vector2df(1, 0));
	vertices[7].set(vector3df(0,0,1), color, vector2df(1, 1));
	vertices[8].set(vector3df(0,1,1), color, vector2df(0, 1));
	vertices[9].set(vector3df(0,1,0), color, vector2df(1, 1));
	vertices[10].set(vector3df(1,0,1), color, vector2df(1, 0));
	vertices[11].set(vector3df(1,0,0), color, vector2df(0,0));

	for (u32 i=0; i<12; ++i)
	{
		vertices[i].Pos -= vector3df(0.5f, 0.5f, 0.5f);
		vertices[i].Pos *= size;
	}

	return false;
}

void CGeometryCreator::getSphereMeshVICount( u32& vcount, u32& icount, u32 polyCountX, u32 polyCountY )
{
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent u16 overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level

	vcount = polyCountXPitch * polyCountY + 2;
	icount = (polyCountX * polyCountY) * 6;
}

bool CGeometryCreator::fillSphereMeshVI( SGTVertex_PC1T* vertices, u32 vcount, u16* indices, u32 icount, f32 radius /*= 5.f*/, u32 polyCountX /*= 16*/, u32 polyCountY /*= 16*/, SColor color /*= SColor()*/ )
{
	u32 vLimit, iLimit;
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent u16 overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level

	vLimit = polyCountXPitch * polyCountY + 2;
	iLimit = (polyCountX * polyCountY) * 6;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//¼ÆÊý
	vcount = 0;
	icount = 0;

	u32 level = 0;

	for (u32 p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			u32 curr = level + p2;
			indices[icount++] = (u16)(curr + polyCountXPitch);
			indices[icount++] = (u16)(curr);
			indices[icount++] = (u16)(curr + 1);
			indices[icount++] = (u16)(curr + polyCountXPitch);
			indices[icount++] = (u16)(curr+1);
			indices[icount++] = (u16)(curr + 1 + polyCountXPitch);
		}

		// the connectors from front to end
		indices[icount++] = (u16)(level + polyCountX - 1 + polyCountXPitch);
		indices[icount++] = (u16)(level + polyCountX - 1);
		indices[icount++] = (u16)(level + polyCountX);

		indices[icount++] = (u16)(level + polyCountX - 1 + polyCountXPitch);
		indices[icount++] = (u16)(level + polyCountX);
		indices[icount++] = (u16)(level + polyCountX + polyCountXPitch);
		level += polyCountXPitch;
	}

	const u32 polyCountSq = polyCountXPitch * polyCountY; // top point
	const u32 polyCountSq1 = polyCountSq + 1; // bottom point
	const u32 polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		indices[icount++] = (u16)(polyCountSq);
		indices[icount++] = (u16)(p2 + 1);
		indices[icount++] = (u16)(p2);

		// create triangles which are at the bottom of the sphere

		indices[icount++] = (u16)(polyCountSqM1 + p2);
		indices[icount++] = (u16)(polyCountSqM1 + p2 + 1);
		indices[icount++] = (u16)(polyCountSq1);
	}

	// create final triangle which is at the top of the sphere
	indices[icount++] = (u16)(polyCountSq);
	indices[icount++] = (u16)(polyCountX);
	indices[icount++] = (u16)(polyCountX-1);

	// create final triangle which is at the bottom of the sphere
	indices[icount++] = (u16)(polyCountSqM1 + polyCountX - 1);
	indices[icount++] = (u16)(polyCountSqM1);
	indices[icount++] = (u16)(polyCountSq1);

	// calculate the angle which separates all points in a circle
	const f32 AngleX = 2 * PI / polyCountX;
	const f32 AngleY = PI / polyCountY;

	f32 axz;

	// we don't start at 0.

	f32 ay = 0;//AngleY / 2;

	for (u32 y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const f32 sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position

			const vector3df pos(static_cast<f32>(radius * cos(axz) * sinay),
				static_cast<f32>(radius * cos(ay)),
				static_cast<f32>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = acos(clamp_((f32)(normal.X/sinay), -1.0f, 1.0f)) * 0.5f *reciprocal_(PI);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = vertices[vcount-polyCountXPitch].TCoords.X;

			vertices[vcount++].set(vector3df(pos.X, pos.Y, pos.Z),
				color,
				vector2df( tu, ay*reciprocal_(PI) ));

			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		vertices[vcount] = vertices[vcount-polyCountX];
		vertices[vcount].TCoords.X=1.0f;
		++vcount;
	}

	// the vertex at the top of the sphere
	vertices[vcount++].set(vector3df(0.0f,radius,0.0f), color, vector2df(0.5f, 0.0f));

	// the vertex at the bottom of the sphere
	vertices[vcount++].set(vector3df(0.0f,-radius,0.0f), color, vector2df(0.5f, 1.0f));

	_ASSERT(polyCountXPitch * polyCountY + 2 == vcount);
	_ASSERT((polyCountX * polyCountY) * 6 == icount);

	return true;
}

void CGeometryCreator::getSkyDomeMeshVICount( u32& vcount, u32& icount, u32 horiRes, u32 vertRes )
{
	vcount = (horiRes + 1) * (vertRes + 1);
	icount = 3 * (2*vertRes -1) * horiRes;
}

bool CGeometryCreator::fillSkyDomeMeshVI( SGTVertex_PC1T* vertices, u32 vcount, u16* indices, u32 icount, u32 horiRes, u32 vertRes, f32 texturePercentage, f32 spherePercentage, f32 radius, SColor color /*= SColor()*/ )
{
	u32 vLimit = (horiRes + 1) * (vertRes + 1);
	u32 iLimit = 3 * (2*vertRes -1) * horiRes;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//
	vcount = 0;
	icount = 0;

	f32 azimuth;
	u32 k;

	const f32 azimuth_step = (PI * 2.f) / horiRes;
	if (spherePercentage < 0.f)
		spherePercentage = -spherePercentage;
	if (spherePercentage > 2.f)
		spherePercentage = 2.f;
	const f32 elevation_step = spherePercentage * PI * 0.5f / (f32)vertRes;

	const f32 tcV = texturePercentage / vertRes;
	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		f32 elevation = PI/2;
		const f32 tcU = (f32)k / (f32)horiRes;
		const f32 sinA = sinf(azimuth);
		const f32 cosA = cosf(azimuth);
		for (u32 j = 0; j <= vertRes; ++j)
		{
			const f32 cosEr = radius * cosf(elevation);
			vertices[vcount].Pos.set(cosEr*sinA, radius*sinf(elevation), cosEr*cosA);
			vertices[vcount].TCoords.set(tcU, j*tcV);
			vertices[vcount].Color = color;

			++vcount;

			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	for (k = 0; k < horiRes; ++k)
	{
		indices[icount++] = (u16)(vertRes + 2 + (vertRes + 1)*k);
		indices[icount++] = (u16)(1 + (vertRes + 1)*k);
		indices[icount++] = (u16)(0 + (vertRes + 1)*k);

		for (u32 j = 1; j < vertRes; ++j)
		{
			indices[icount++] = (u16)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(1 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(0 + (vertRes + 1)*k + j);

			indices[icount++] = (u16)(vertRes + 1 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(0 + (vertRes + 1)*k + j);
		}
	}

	_ASSERT( vcount == (horiRes + 1) * (vertRes + 1));
	_ASSERT( icount == 3 * (2*vertRes -1) * horiRes);

	return true;
}

bool CGeometryCreator::fillSkyDomeMeshVI( SGVertex_PC* vertices, u32 vcount, u16* indices, u32 icount, u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius, SColor color/*=SColor()*/ )
{
	u32 vLimit = (horiRes + 1) * (vertRes + 1);
	u32 iLimit = 3 * (2*vertRes -1) * horiRes;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//
	vcount = 0;
	icount = 0;

	f32 azimuth;
	u32 k;

	const f32 azimuth_step = (PI * 2.f) / horiRes;
	if (spherePercentage < 0.f)
		spherePercentage = -spherePercentage;
	if (spherePercentage > 2.f)
		spherePercentage = 2.f;
	const f32 elevation_step = spherePercentage * PI * 0.5f / (f32)vertRes;

	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		f32 elevation = PI/2;
		const f32 tcU = (f32)k / (f32)horiRes;
		const f32 sinA = sinf(azimuth);
		const f32 cosA = cosf(azimuth);
		for (u32 j = 0; j <= vertRes; ++j)
		{
			const f32 cosEr = radius * cosf(elevation);
			vertices[vcount].Pos.set(cosEr*sinA, radius*sinf(elevation), cosEr*cosA);
			vertices[vcount].Color = color;

			++vcount;

			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	for (k = 0; k < horiRes; ++k)
	{
		indices[icount++] = (u16)(vertRes + 2 + (vertRes + 1)*k);
		indices[icount++] = (u16)(1 + (vertRes + 1)*k);
		indices[icount++] = (u16)(0 + (vertRes + 1)*k);

		for (u32 j = 1; j < vertRes; ++j)
		{
			indices[icount++] = (u16)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(1 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(0 + (vertRes + 1)*k + j);

			indices[icount++] = (u16)(vertRes + 1 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (u16)(0 + (vertRes + 1)*k + j);
		}
	}

	_ASSERT( vcount == (horiRes + 1) * (vertRes + 1));
	_ASSERT( icount == 3 * (2*vertRes -1) * horiRes);

	return true;
}

aabbox3df CGeometryCreator::getSkyDomeMeshAABBox( u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius )
{
	u32 vLimit = (horiRes + 1) * (vertRes + 1);

	//
	aabbox3df box;

	f32 azimuth;
	u32 k;

	const f32 azimuth_step = (PI * 2.f) / horiRes;
	if (spherePercentage < 0.f)
		spherePercentage = -spherePercentage;
	if (spherePercentage > 2.f)
		spherePercentage = 2.f;
	const f32 elevation_step = spherePercentage * PI * 0.5f / (f32)vertRes;

	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		f32 elevation = PI/2;
		const f32 tcU = (f32)k / (f32)horiRes;
		const f32 sinA = sinf(azimuth);
		const f32 cosA = cosf(azimuth);
		for (u32 j = 0; j <= vertRes; ++j)
		{
			const f32 cosEr = radius * cosf(elevation);
			box.addInternalPoint(vector3df(cosEr*sinA, radius*sinf(elevation), cosEr*cosA));

			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	return box;
}


bool CGeometryCreator::fillGridLineMeshV( SGVertex_PC* vertices, u32 vcount, u32 xzCount, f32 gridSize, SColor color )
{
	u32 vLimit = (2*xzCount+1) * 4;
	if (vcount < vLimit)
		return false;

	//
	vcount = 0;

	f32 halfWidth = xzCount * gridSize;

	//x
	vertices[vcount++].set(vector3df(-halfWidth, 0, 0), color);
	vertices[vcount++].set(vector3df(halfWidth, 0, 0), color);

	for (u32 i=1; i<=xzCount; ++i)
	{
		f32 offset = i * gridSize;
		vertices[vcount++].set(vector3df(-halfWidth, 0, offset) , color);
		vertices[vcount++].set(vector3df(halfWidth, 0, offset) , color);
		vertices[vcount++].set(vector3df(-halfWidth, 0, -offset) , color);
		vertices[vcount++].set(vector3df(halfWidth, 0, -offset) , color);
	}

	//z
	vertices[vcount++].set(vector3df(0, 0, -halfWidth), color);
	vertices[vcount++].set(vector3df(0, 0, halfWidth), color);

	for (u32 i=1; i<=xzCount; ++i)
	{
		f32 offset = i * gridSize;
		vertices[vcount++].set(vector3df(offset, 0, -halfWidth) , color);
		vertices[vcount++].set(vector3df(offset, 0, halfWidth) , color);
		vertices[vcount++].set(vector3df(-offset, 0, -halfWidth) , color);
		vertices[vcount++].set(vector3df(-offset, 0, halfWidth) , color);
	}

	_ASSERT(vcount == vLimit);

	return true;
}

