#include "stdafx.h"
#include "CGeometryCreator.h"
#include "mywow.h"

void CGeometryCreator::getCubeMeshVICount( uint32_t& vcount, uint32_t& icount )
{
	vcount = 12;
	icount = 36;
}

bool CGeometryCreator::fillCubeMeshVI( SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, const vector3df& size, SColor color /*= SColor()*/ )
{
	uint32_t vLimit, iLimit;
	getCubeMeshVICount(vLimit, iLimit);

	if (vcount < vLimit || icount < iLimit)
		return false;

	const uint16_t u[36] = {  0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
		7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7 };

	Q_memcpy(indices, sizeof(uint16_t)*36, u, sizeof(u));

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

	for (uint32_t i=0; i<12; ++i)
	{
		vertices[i].Pos -= vector3df(0.5f, 0.5f, 0.5f);
		vertices[i].Pos *= size;
	}

	return false;
}

void CGeometryCreator::getSphereMeshVICount( uint32_t& vcount, uint32_t& icount, uint32_t polyCountX, uint32_t polyCountY )
{
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent uint16_t overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const uint32_t polyCountXPitch = polyCountX+1; // get to same vertex on next level

	vcount = polyCountXPitch * polyCountY + 2;
	icount = (polyCountX * polyCountY) * 6;
}

bool CGeometryCreator::fillSphereMeshVI( SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, float radius /*= 5.f*/, uint32_t polyCountX /*= 16*/, uint32_t polyCountY /*= 16*/, SColor color /*= SColor()*/ )
{
	uint32_t vLimit, iLimit;
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent uint16_t overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const uint32_t polyCountXPitch = polyCountX+1; // get to same vertex on next level

	vLimit = polyCountXPitch * polyCountY + 2;
	iLimit = (polyCountX * polyCountY) * 6;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//¼ÆÊý
	vcount = 0;
	icount = 0;

	uint32_t level = 0;

	for (uint32_t p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (uint32_t p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			uint32_t curr = level + p2;
			indices[icount++] = (uint16_t)(curr + polyCountXPitch);
			indices[icount++] = (uint16_t)(curr);
			indices[icount++] = (uint16_t)(curr + 1);
			indices[icount++] = (uint16_t)(curr + polyCountXPitch);
			indices[icount++] = (uint16_t)(curr+1);
			indices[icount++] = (uint16_t)(curr + 1 + polyCountXPitch);
		}

		// the connectors from front to end
		indices[icount++] = (uint16_t)(level + polyCountX - 1 + polyCountXPitch);
		indices[icount++] = (uint16_t)(level + polyCountX - 1);
		indices[icount++] = (uint16_t)(level + polyCountX);

		indices[icount++] = (uint16_t)(level + polyCountX - 1 + polyCountXPitch);
		indices[icount++] = (uint16_t)(level + polyCountX);
		indices[icount++] = (uint16_t)(level + polyCountX + polyCountXPitch);
		level += polyCountXPitch;
	}

	const uint32_t polyCountSq = polyCountXPitch * polyCountY; // top point
	const uint32_t polyCountSq1 = polyCountSq + 1; // bottom point
	const uint32_t polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (uint32_t p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		indices[icount++] = (uint16_t)(polyCountSq);
		indices[icount++] = (uint16_t)(p2 + 1);
		indices[icount++] = (uint16_t)(p2);

		// create triangles which are at the bottom of the sphere

		indices[icount++] = (uint16_t)(polyCountSqM1 + p2);
		indices[icount++] = (uint16_t)(polyCountSqM1 + p2 + 1);
		indices[icount++] = (uint16_t)(polyCountSq1);
	}

	// create final triangle which is at the top of the sphere
	indices[icount++] = (uint16_t)(polyCountSq);
	indices[icount++] = (uint16_t)(polyCountX);
	indices[icount++] = (uint16_t)(polyCountX-1);

	// create final triangle which is at the bottom of the sphere
	indices[icount++] = (uint16_t)(polyCountSqM1 + polyCountX - 1);
	indices[icount++] = (uint16_t)(polyCountSqM1);
	indices[icount++] = (uint16_t)(polyCountSq1);

	// calculate the angle which separates all points in a circle
	const float AngleX = 2 * PI / polyCountX;
	const float AngleY = PI / polyCountY;

	float axz;

	// we don't start at 0.

	float ay = 0;//AngleY / 2;

	for (uint32_t y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const float sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (uint32_t xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position

			const vector3df pos(static_cast<float>(radius * cos(axz) * sinay),
				static_cast<float>(radius * cos(ay)),
				static_cast<float>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			float tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = acos(clamp_((float)(normal.X/sinay), -1.0f, 1.0f)) * 0.5f *reciprocal_(PI);
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

	ASSERT(polyCountXPitch * polyCountY + 2 == vcount);
	ASSERT((polyCountX * polyCountY) * 6 == icount);

	return true;
}

void CGeometryCreator::getSkyDomeMeshVICount( uint32_t& vcount, uint32_t& icount, uint32_t horiRes, uint32_t vertRes )
{
	vcount = (horiRes + 1) * (vertRes + 1);
	icount = 3 * (2*vertRes -1) * horiRes;
}

bool CGeometryCreator::fillSkyDomeMeshVI( SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, uint32_t horiRes, uint32_t vertRes, float texturePercentage, float spherePercentage, float radius, SColor color /*= SColor()*/ )
{
	uint32_t vLimit = (horiRes + 1) * (vertRes + 1);
	uint32_t iLimit = 3 * (2*vertRes -1) * horiRes;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//
	vcount = 0;
	icount = 0;

	float azimuth;
	uint32_t k;

	const float azimuth_step = (PI * 2.f) / horiRes;
	if (spherePercentage < 0.f)
		spherePercentage = -spherePercentage;
	if (spherePercentage > 2.f)
		spherePercentage = 2.f;
	const float elevation_step = spherePercentage * PI * 0.5f / (float)vertRes;

	const float tcV = texturePercentage / vertRes;
	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		float elevation = PI/2;
		const float tcU = (float)k / (float)horiRes;
		const float sinA = sinf(azimuth);
		const float cosA = cosf(azimuth);
		for (uint32_t j = 0; j <= vertRes; ++j)
		{
			const float cosEr = radius * cosf(elevation);
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
		indices[icount++] = (uint16_t)(vertRes + 2 + (vertRes + 1)*k);
		indices[icount++] = (uint16_t)(1 + (vertRes + 1)*k);
		indices[icount++] = (uint16_t)(0 + (vertRes + 1)*k);

		for (uint32_t j = 1; j < vertRes; ++j)
		{
			indices[icount++] = (uint16_t)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(1 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(0 + (vertRes + 1)*k + j);

			indices[icount++] = (uint16_t)(vertRes + 1 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(0 + (vertRes + 1)*k + j);
		}
	}

	ASSERT( vcount == (horiRes + 1) * (vertRes + 1));
	ASSERT( icount == 3 * (2*vertRes -1) * horiRes);

	return true;
}

bool CGeometryCreator::fillSkyDomeMeshVI( SVertex_PC* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius, SColor color/*=SColor()*/ )
{
	uint32_t vLimit = (horiRes + 1) * (vertRes + 1);
	uint32_t iLimit = 3 * (2*vertRes -1) * horiRes;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//
	vcount = 0;
	icount = 0;

	float azimuth;
	uint32_t k;

	const float azimuth_step = (PI * 2.f) / horiRes;
	if (spherePercentage < 0.f)
		spherePercentage = -spherePercentage;
	if (spherePercentage > 2.f)
		spherePercentage = 2.f;
	const float elevation_step = spherePercentage * PI * 0.5f / (float)vertRes;

	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		float elevation = PI/2;
		const float sinA = sinf(azimuth);
		const float cosA = cosf(azimuth);
		for (uint32_t j = 0; j <= vertRes; ++j)
		{
			const float cosEr = radius * cosf(elevation);
			vertices[vcount].Pos.set(cosEr*sinA, radius*sinf(elevation), cosEr*cosA);
			vertices[vcount].Color = color;

			++vcount;

			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	for (k = 0; k < horiRes; ++k)
	{
		indices[icount++] = (uint16_t)(vertRes + 2 + (vertRes + 1)*k);
		indices[icount++] = (uint16_t)(1 + (vertRes + 1)*k);
		indices[icount++] = (uint16_t)(0 + (vertRes + 1)*k);

		for (uint32_t j = 1; j < vertRes; ++j)
		{
			indices[icount++] = (uint16_t)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(1 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(0 + (vertRes + 1)*k + j);

			indices[icount++] = (uint16_t)(vertRes + 1 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(vertRes + 2 + (vertRes + 1)*k + j);
			indices[icount++] = (uint16_t)(0 + (vertRes + 1)*k + j);
		}
	}

	ASSERT( vcount == (horiRes + 1) * (vertRes + 1));
	ASSERT( icount == 3 * (2*vertRes -1) * horiRes);

	return true;
}

aabbox3df CGeometryCreator::getSkyDomeMeshAABBox( uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius )
{
	//
	aabbox3df box;

	float azimuth;
	uint32_t k;

	const float azimuth_step = (PI * 2.f) / horiRes;
	if (spherePercentage < 0.f)
		spherePercentage = -spherePercentage;
	if (spherePercentage > 2.f)
		spherePercentage = 2.f;
	const float elevation_step = spherePercentage * PI * 0.5f / (float)vertRes;

	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		float elevation = PI/2;
		const float sinA = sinf(azimuth);
		const float cosA = cosf(azimuth);
		for (uint32_t j = 0; j <= vertRes; ++j)
		{
			const float cosEr = radius * cosf(elevation);
			box.addInternalPoint(vector3df(cosEr*sinA, radius*sinf(elevation), cosEr*cosA));

			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	return box;
}


bool CGeometryCreator::fillGridLineMeshV( SVertex_PC* vertices, uint32_t vcount, uint32_t xzCount, float gridSize, SColor color )
{
	uint32_t vLimit = (2*xzCount+1) * 4;
	if (vcount < vLimit)
		return false;

	//
	vcount = 0;

	float halfWidth = xzCount * gridSize;

	//x
	vertices[vcount++].set(vector3df(-halfWidth, 0, 0), color);
	vertices[vcount++].set(vector3df(halfWidth, 0, 0), color);

	for (uint32_t i=1; i<=xzCount; ++i)
	{
		float offset = i * gridSize;
		vertices[vcount++].set(vector3df(-halfWidth, 0, offset) , color);
		vertices[vcount++].set(vector3df(halfWidth, 0, offset) , color);
		vertices[vcount++].set(vector3df(-halfWidth, 0, -offset) , color);
		vertices[vcount++].set(vector3df(halfWidth, 0, -offset) , color);
	}

	//z
	vertices[vcount++].set(vector3df(0, 0, -halfWidth), color);
	vertices[vcount++].set(vector3df(0, 0, halfWidth), color);

	for (uint32_t i=1; i<=xzCount; ++i)
	{
		float offset = i * gridSize;
		vertices[vcount++].set(vector3df(offset, 0, -halfWidth) , color);
		vertices[vcount++].set(vector3df(offset, 0, halfWidth) , color);
		vertices[vcount++].set(vector3df(-offset, 0, -halfWidth) , color);
		vertices[vcount++].set(vector3df(-offset, 0, halfWidth) , color);
	}

	ASSERT(vcount == vLimit);

	return true;
}

