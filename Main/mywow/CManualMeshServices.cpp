#include "stdafx.h"
#include "CManualMeshServices.h"
#include "mywow.h"
#include "CMesh.h"

bool CManualMeshServices::addMesh( const char* name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box)
{
	if (MeshMap.find(name) != MeshMap.end())
		return false;

	CMesh* m = new CMesh(bufferParam, primType, primCount, box);
	MeshMap[name] = m;

	return true;
}

IMesh* CManualMeshServices::getMesh( const char* name )
{
	TMeshMap::const_iterator i = MeshMap.find(name);
	if (i == MeshMap.end())
		return nullptr;

	return i->second;
}

void CManualMeshServices::removeMesh( const char* name )
{
	TMeshMap::const_iterator i = MeshMap.find(name);
	if (i == MeshMap.end())
		return;

	delete i->second;

	MeshMap.erase(name);
}

void CManualMeshServices::clear()
{
	for (TMeshMap::const_iterator itr = MeshMap.begin(); itr != MeshMap.end(); ++itr)
	{
		delete itr->second;
	}

	MeshMap.clear();
}

CManualMeshServices::~CManualMeshServices()
{
	clear();
}

bool CManualMeshServices::addGridLineMesh( const char* name, uint32_t xzCount, float gridSize, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	uint32_t vcount = geo->getGridLineVCount(xzCount);
	SVertex_PC* vertices = new SVertex_PC[vcount];
	geo->fillGridLineMeshV(vertices, vcount, xzCount, gridSize, color);

	CVertexBuffer* vbuffer = new CVertexBuffer(true);
	vbuffer->set(vertices, EST_PC, vcount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = vbuffer;

	return addMesh(name, bufferParam, geo->getPrimType(EGT_GRID), vcount/2, geo->getGridLineAABBox(xzCount,gridSize));
}

bool CManualMeshServices::addDecal( const char* name, float width, float height, SColor color )
{
	SVertex_PCT* vertices = new SVertex_PCT[4];
	vertices[0].Pos = vector3df(-width, height, 0);		vertices[0].Color = color;    vertices[0].TCoords.set(0,0);
	vertices[1].Pos = vector3df(width, height, 0);		vertices[1].Color = color;	  vertices[1].TCoords.set(1,0);
	vertices[2].Pos = vector3df(-width, -height, 0);		vertices[2].Color = color;	  vertices[2].TCoords.set(0,1);
	vertices[3].Pos = vector3df(width, -height, 0);		vertices[3].Color = color;	  vertices[3].TCoords.set(1,1);
	
	uint16_t* indices = new uint16_t[6];
	indices[0] = 0; indices[1] = 1; indices[2] = 2; indices[3] = 2; indices[4] = 1; indices[5] = 3;

	CVertexBuffer* vbuffer = new CVertexBuffer(true);
	vbuffer->set(vertices, EST_PCT, 4, EMM_STATIC);

	CIndexBuffer* ibuffer = new CIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, 6, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, 2, aabbox3df(vector3df(-width,-height,0), vector3df(width,height,0)));
}

bool CManualMeshServices::addCube( const char* name, const vector3df& size, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	uint32_t vcount = 0;
	uint32_t icount = 0;
	geo->getCubeMeshVICount(vcount, icount);

	SVertex_PCT* vertices = new SVertex_PCT[vcount];
	uint16_t* indices = new uint16_t[icount];
	geo->fillCubeMeshVI(vertices, vcount, indices, icount, size, color);

	CVertexBuffer* vbuffer = new CVertexBuffer(true);
	vbuffer->set(vertices, EST_PCT, vcount, EMM_STATIC);

	CIndexBuffer* ibuffer = new CIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, icount/3, geo->getCubeMeshAABBox(size));
}

bool CManualMeshServices::addSphere( const char* name, float radius, uint32_t polyCountX, uint32_t polyCountY, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	uint32_t vcount = 0;
	uint32_t icount = 0;
	geo->getSphereMeshVICount(vcount, icount, polyCountX, polyCountY);

	SVertex_PCT* vertices = new SVertex_PCT[vcount];
	uint16_t* indices = new uint16_t[icount];
	geo->fillSphereMeshVI(vertices, vcount, indices, icount, radius, polyCountX, polyCountY, color);

	CVertexBuffer* vbuffer = new CVertexBuffer(true);
	vbuffer->set(vertices, EST_PCT, vcount, EMM_STATIC);

	CIndexBuffer* ibuffer = new CIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, icount/3, geo->getSphereMeshAABBox(radius));
}

bool CManualMeshServices::addSkyDome( const char* name, uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	uint32_t vcount = 0;
	uint32_t icount = 0;
	geo->getSkyDomeMeshVICount(vcount, icount, horiRes, vertRes);

	SVertex_PC* vertices = new SVertex_PC[vcount];
	uint16_t* indices = new uint16_t[icount];
	geo->fillSkyDomeMeshVI(vertices, vcount, indices, icount, horiRes, vertRes, spherePercentage, radius, color);

	CVertexBuffer* vbuffer = new CVertexBuffer(true);
	vbuffer->set(vertices, EST_PC, vcount, EMM_STATIC);

	CIndexBuffer* ibuffer = new CIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, icount/3, geo->getSkyDomeMeshAABBox(horiRes, vertRes, spherePercentage, radius));
}

