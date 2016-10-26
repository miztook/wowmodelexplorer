#include "stdafx.h"
#include "CManualMeshServices.h"
#include "mywow.h"
#include "CMesh.h"

bool CManualMeshServices::addMesh( const c8* name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box)
{
	if (MeshMap.find(name) != MeshMap.end())
		return false;

	CMesh* m = new CMesh(bufferParam, primType, primCount, box);
	MeshMap[name] = m;

	return true;
}

IMesh* CManualMeshServices::getMesh( const c8* name )
{
	TMeshMap::const_iterator i = MeshMap.find(name);
	if (i == MeshMap.end())
		return NULL_PTR;

	return i->second;
}

void CManualMeshServices::removeMesh( const c8* name )
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

bool CManualMeshServices::addGridLineMesh( const c8* name, u32 xzCount, f32 gridSize, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	u32 vcount = geo->getGridLineVCount(xzCount);
	SVertex_PC* vertices = new SVertex_PC[vcount];
	geo->fillGridLineMeshV(vertices, vcount, xzCount, gridSize, color);

	IVertexBuffer* vbuffer = new IVertexBuffer(true);
	vbuffer->set(vertices, EST_PC, vcount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = vbuffer;

	return addMesh(name, bufferParam, geo->getPrimType(EGT_GRID), vcount/2, geo->getGridLineAABBox(xzCount,gridSize));
}

bool CManualMeshServices::addDecal( const c8* name, f32 width, f32 height, SColor color )
{
	SVertex_PCT* vertices = new SVertex_PCT[4];
	vertices[0].Pos = vector3df(-width, height, 0);		vertices[0].Color = color;    vertices[0].TCoords.set(0,0);
	vertices[1].Pos = vector3df(width, height, 0);		vertices[1].Color = color;	  vertices[1].TCoords.set(1,0);
	vertices[2].Pos = vector3df(-width, -height, 0);		vertices[2].Color = color;	  vertices[2].TCoords.set(0,1);
	vertices[3].Pos = vector3df(width, -height, 0);		vertices[3].Color = color;	  vertices[3].TCoords.set(1,1);
	
	u16* indices = new u16[6];
	indices[0] = 0; indices[1] = 1; indices[2] = 2; indices[3] = 2; indices[4] = 1; indices[5] = 3;

	IVertexBuffer* vbuffer = new IVertexBuffer(true);
	vbuffer->set(vertices, EST_PCT, 4, EMM_STATIC);

	IIndexBuffer* ibuffer = new IIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, 6, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, 2, aabbox3df(vector3df(-width,-height,0), vector3df(width,height,0)));
}

bool CManualMeshServices::addCube( const c8* name, const vector3df& size, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	u32 vcount = 0;
	u32 icount = 0;
	geo->getCubeMeshVICount(vcount, icount);

	SVertex_PCT* vertices = new SVertex_PCT[vcount];
	u16* indices = new u16[icount];
	geo->fillCubeMeshVI(vertices, vcount, indices, icount, size, color);

	IVertexBuffer* vbuffer = new IVertexBuffer(true);
	vbuffer->set(vertices, EST_PCT, vcount, EMM_STATIC);

	IIndexBuffer* ibuffer = new IIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, icount/3, geo->getCubeMeshAABBox(size));
}

bool CManualMeshServices::addSphere( const c8* name, f32 radius, u32 polyCountX, u32 polyCountY, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	u32 vcount = 0;
	u32 icount = 0;
	geo->getSphereMeshVICount(vcount, icount, polyCountX, polyCountY);

	SVertex_PCT* vertices = new SVertex_PCT[vcount];
	u16* indices = new u16[icount];
	geo->fillSphereMeshVI(vertices, vcount, indices, icount, radius, polyCountX, polyCountY, color);

	IVertexBuffer* vbuffer = new IVertexBuffer(true);
	vbuffer->set(vertices, EST_PCT, vcount, EMM_STATIC);

	IIndexBuffer* ibuffer = new IIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PCT;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, icount/3, geo->getSphereMeshAABBox(radius));
}

bool CManualMeshServices::addSkyDome( const c8* name, u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius, SColor color )
{
	IGeometryCreator* geo = g_Engine->getGeometryCreator();

	u32 vcount = 0;
	u32 icount = 0;
	geo->getSkyDomeMeshVICount(vcount, icount, horiRes, vertRes);

	SVertex_PC* vertices = new SVertex_PC[vcount];
	u16* indices = new u16[icount];
	geo->fillSkyDomeMeshVI(vertices, vcount, indices, icount, horiRes, vertRes, spherePercentage, radius, color);

	IVertexBuffer* vbuffer = new IVertexBuffer(true);
	vbuffer->set(vertices, EST_PC, vcount, EMM_STATIC);

	IIndexBuffer* ibuffer = new IIndexBuffer(true);
	ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SBufferParam bufferParam = {0};
	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = vbuffer;
	bufferParam.ibuffer = ibuffer;

	return addMesh(name, bufferParam, EPT_TRIANGLES, icount/3, geo->getSkyDomeMeshAABBox(horiRes, vertRes, spherePercentage, radius));
}

