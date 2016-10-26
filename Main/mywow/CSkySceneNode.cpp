#include "stdafx.h"
#include "CSkySceneNode.h"
#include "mywow.h"
#include "CMapEnvironment.h"
#include "CMesh.h"
#include "CSceneRenderServices.h"

#define  HORIZ_RES	32
#define  VERT_RES  7

CSkySceneNode::CSkySceneNode(CMapEnvironment* mapEnvironment)
	: MapEnvironment(mapEnvironment)
{
	Material.MaterialType = EMT_LINE;
	Material.Lighting = false;
	Material.Cull = ECM_BACK;
//	Material.Wireframe = true;

	SkyRadius = 2500.0f;

	createSkyDomeMesh();
}

CSkySceneNode::~CSkySceneNode()
{
	delete SkyDomeMesh;
}

void CSkySceneNode::registerSceneNode( bool frustumcheck, int sequence )
{
	//update();

	if (!Visible)
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);
}

void CSkySceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	WorldMatrix.setTranslation(cam->getPosition());

	MapEnvironment->computeSkyLights(cam->getPosition(), 0);
	g_Engine->getSceneEnvironment()->OutDoorLightInfo.fogColor = MapEnvironment->ColorSet[FOG_COLOR];

	updateMeshColor();
}

void CSkySceneNode::render() const
{
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());
	SRenderUnit unit = {0};

	unit.bufferParam = SkyDomeMesh->BufferParam;
	unit.primType = SkyDomeMesh->PrimType;
	unit.drawParam.numVertices = SkyDomeMesh->BufferParam.vbuffer0->Size;
	unit.primCount = SkyDomeMesh->PrimCount;
	unit.sceneNode = this;
	unit.material = Material;

	unit.matWorld = &WorldMatrix;

	sceneRenderServices->addRenderUnit(&unit, ERT_SKY);
}

void CSkySceneNode::createSkyDomeMesh()
{
	u32 vcount = (HORIZ_RES + 1) * (VERT_RES + 1);
	u32 icount = 3 * (2*VERT_RES -1) * HORIZ_RES;

	SVertex_PC* vertices = new SVertex_PC[vcount];
	u16* indices = new u16[icount];
	fillSkyDomeMeshVI(vertices, vcount, indices, icount);

	SBufferParam bufferParam = {0};

	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = new IVertexBuffer(true);
	bufferParam.vbuffer0->set(vertices, EST_PC, vcount, EMM_DYNAMIC);

	bufferParam.ibuffer = new IIndexBuffer(true);
	bufferParam.ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SkyDomeMesh = new CMesh(bufferParam, EPT_TRIANGLES, icount/3, aabbox3df(-SkyRadius, -SkyRadius, -SkyRadius, SkyRadius, SkyRadius, SkyRadius));
}


bool CSkySceneNode::fillSkyDomeMeshVI(  SVertex_PC* gVertices, u32 vcount, u16* indices, u32 icount )
{
	u32 vLimit = (HORIZ_RES + 1) * (VERT_RES + 1);
	u32 iLimit = 3 * (2*VERT_RES -1) * HORIZ_RES;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//
	vcount = 0;
	icount = 0;

	f32 azimuth;
	u32 k;

	const f32 azimuth_step = (PI * 2.f) / HORIZ_RES;
	const f32 elevations[] = { PI/2, PI/6, PI/12, PI/18, PI/36, 0, -PI/6, -PI/2};
	const u32 skycolors[] = {2, 3, 4, 5, 6, 7, 7, 7};

	for (k = 0, azimuth = 0; k <= HORIZ_RES; ++k)
	{
		const f32 sinA = sinf(azimuth);
		const f32 cosA = cosf(azimuth);
		for (u32 j = 0; j <= VERT_RES; ++j)
		{
			f32 elevation = elevations[j];
			vector3df v = MapEnvironment->ColorSet[skycolors[j]];
			SColorf color;
			color.set(v.X, v.Y, v.Z);

			const f32 cosEr = SkyRadius * cosf(elevation);
			gVertices[vcount].Pos.set(cosEr*sinA, SkyRadius*sinf(elevation), cosEr*cosA);
			gVertices[vcount].Color = color.toSColor();

			++vcount;
		}
		azimuth += azimuth_step;
	}

	for (k = 0; k < HORIZ_RES; ++k)
	{
		indices[icount++] = (u16)(VERT_RES + 2 + (VERT_RES + 1)*k);
		indices[icount++] = (u16)(1 + (VERT_RES + 1)*k);
		indices[icount++] = (u16)(0 + (VERT_RES + 1)*k);

		for (u32 j = 1; j < VERT_RES; ++j)
		{
			indices[icount++] = (u16)(VERT_RES + 2 + (VERT_RES + 1)*k + j);
			indices[icount++] = (u16)(1 + (VERT_RES + 1)*k + j);
			indices[icount++] = (u16)(0 + (VERT_RES + 1)*k + j);

			indices[icount++] = (u16)(VERT_RES + 1 + (VERT_RES + 1)*k + j);
			indices[icount++] = (u16)(VERT_RES + 2 + (VERT_RES + 1)*k + j);
			indices[icount++] = (u16)(0 + (VERT_RES + 1)*k + j);
		}
	}

	ASSERT( vcount == (HORIZ_RES + 1) * (VERT_RES + 1));
	ASSERT( icount == 3 * (2*VERT_RES -1) * HORIZ_RES);

	return true;
}

void CSkySceneNode::updateMeshColor()
{
	const u32 skycolors[] = {2, 3, 4, 5, 6, 7, 7, 7};
	SVertex_PC* vertices = (SVertex_PC*)SkyDomeMesh->BufferParam.vbuffer0->Vertices;

	u32 vcount = 0;
	for (u32 k = 0; k <= HORIZ_RES; ++k)
	{
		for (u32 j = 0; j <= VERT_RES; ++j)
		{
			vector3df v = MapEnvironment->ColorSet[skycolors[j]];
			SColorf color;
			color.set(v.X, v.Y, v.Z);

			vertices[vcount].Color = color.toSColor();

			++vcount;
		}
	}

	SkyDomeMesh->updateVertexBuffer(0);
}