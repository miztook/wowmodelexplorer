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

void CSkySceneNode::tick( uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible )
{
	const ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
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
	uint32_t vcount = (HORIZ_RES + 1) * (VERT_RES + 1);
	uint32_t icount = 3 * (2*VERT_RES -1) * HORIZ_RES;

	SVertex_PC* vertices = new SVertex_PC[vcount];
	uint16_t* indices = new uint16_t[icount];
	fillSkyDomeMeshVI(vertices, vcount, indices, icount);

	SBufferParam bufferParam = {0};

	bufferParam.vType = EVT_PC;
	bufferParam.vbuffer0 = new CVertexBuffer(true);
	bufferParam.vbuffer0->set(vertices, EST_PC, vcount, EMM_DYNAMIC);

	bufferParam.ibuffer = new CIndexBuffer(true);
	bufferParam.ibuffer->set(indices, EIT_16BIT, icount, EMM_STATIC);

	SkyDomeMesh = new CMesh(bufferParam, EPT_TRIANGLES, icount/3, aabbox3df(-SkyRadius, -SkyRadius, -SkyRadius, SkyRadius, SkyRadius, SkyRadius));
}


bool CSkySceneNode::fillSkyDomeMeshVI(  SVertex_PC* gVertices, uint32_t vcount, uint16_t* indices, uint32_t icount )
{
	uint32_t vLimit = (HORIZ_RES + 1) * (VERT_RES + 1);
	uint32_t iLimit = 3 * (2*VERT_RES -1) * HORIZ_RES;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//
	vcount = 0;
	icount = 0;

	float azimuth;
	uint32_t k;

	const float azimuth_step = (PI * 2.f) / HORIZ_RES;
	const float elevations[] = { PI/2, PI/6, PI/12, PI/18, PI/36, 0, -PI/6, -PI/2};
	const uint32_t skycolors[] = {2, 3, 4, 5, 6, 7, 7, 7};

	for (k = 0, azimuth = 0; k <= HORIZ_RES; ++k)
	{
		const float sinA = sinf(azimuth);
		const float cosA = cosf(azimuth);
		for (uint32_t j = 0; j <= VERT_RES; ++j)
		{
			float elevation = elevations[j];
			vector3df v = MapEnvironment->ColorSet[skycolors[j]];
			SColorf color;
			color.set(v.X, v.Y, v.Z);

			const float cosEr = SkyRadius * cosf(elevation);
			gVertices[vcount].Pos.set(cosEr*sinA, SkyRadius*sinf(elevation), cosEr*cosA);
			gVertices[vcount].Color = color.toSColor();

			++vcount;
		}
		azimuth += azimuth_step;
	}

	for (k = 0; k < HORIZ_RES; ++k)
	{
		indices[icount++] = (uint16_t)(VERT_RES + 2 + (VERT_RES + 1)*k);
		indices[icount++] = (uint16_t)(1 + (VERT_RES + 1)*k);
		indices[icount++] = (uint16_t)(0 + (VERT_RES + 1)*k);

		for (uint32_t j = 1; j < VERT_RES; ++j)
		{
			indices[icount++] = (uint16_t)(VERT_RES + 2 + (VERT_RES + 1)*k + j);
			indices[icount++] = (uint16_t)(1 + (VERT_RES + 1)*k + j);
			indices[icount++] = (uint16_t)(0 + (VERT_RES + 1)*k + j);

			indices[icount++] = (uint16_t)(VERT_RES + 1 + (VERT_RES + 1)*k + j);
			indices[icount++] = (uint16_t)(VERT_RES + 2 + (VERT_RES + 1)*k + j);
			indices[icount++] = (uint16_t)(0 + (VERT_RES + 1)*k + j);
		}
	}

	ASSERT( vcount == (HORIZ_RES + 1) * (VERT_RES + 1));
	ASSERT( icount == 3 * (2*VERT_RES -1) * HORIZ_RES);

	return true;
}

void CSkySceneNode::updateMeshColor()
{
	const uint32_t skycolors[] = {2, 3, 4, 5, 6, 7, 7, 7};
	SVertex_PC* vertices = (SVertex_PC*)SkyDomeMesh->BufferParam.vbuffer0->Vertices;

	uint32_t vcount = 0;
	for (uint32_t k = 0; k <= HORIZ_RES; ++k)
	{
		for (uint32_t j = 0; j <= VERT_RES; ++j)
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