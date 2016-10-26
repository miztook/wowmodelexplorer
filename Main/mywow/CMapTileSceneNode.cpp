#include "stdafx.h"
#include "CMapTileSceneNode.h"
#include "mywow.h"
#include "wow_tileScene.h"
#include "CSceneRenderServices.h"
#include "CTerrainServices.h"
#include "CFileADT.h"	
#include "CFileWDT.h"

CMapTileSceneNode::CMapTileSceneNode( IFileWDT* wdt, STile* tile, ISceneNode* parent )
	: IMapTileSceneNode(parent)
{
	FileWDT = static_cast<CFileWDT*>(wdt);

	FileWDT->grab();

	ASSERT(tile);
	ASSERT(!tile->fileAdt);
	bool success = FileWDT->loadADT(tile);
	ASSERT(success);

	Block.tile = tile;

	Material.VertexShader = g_Engine->getDriver()->getShaderServices()->getVertexShader(EVST_TERRAIN);
	Material.MaterialType = EMT_TERRAIN_MULTIPASS;
	Material.Lighting = true;
	Material.Cull = ECM_BACK;
	Material.FogEnable = true;
	//Material.Wireframe = true;

	//scene
	TileScene = new wow_tileScene(this);
}

CMapTileSceneNode::~CMapTileSceneNode()
{
	delete TileScene;

	bool success = FileWDT->unloadADT(Block.tile);
	ASSERT(success);

	FileWDT->drop();
}

IFileADT* CMapTileSceneNode::getFileADT() const
{ 
	return Block.tile->fileAdt;
}

void CMapTileSceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	update();

	TileScene->update();

	if (!Visible ||
		(frustumcheck && !isNodeEligible()))
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();

	TileScene->setCameraChunk(NULL_PTR);
	registerVisibleChunks(cam);

	TileScene->registerInstances(cam);
}

aabbox3df CMapTileSceneNode::getBoundingBox() const
{
	CFileADT* adt = static_cast<CFileADT*>(Block.tile->fileAdt);
	return adt->getBoundingBox();
}

vector3df CMapTileSceneNode::getCenter() const
{
	CFileADT* adt = static_cast<CFileADT*>(Block.tile->fileAdt);

	vector3df  v = adt->getBoundingBox().getCenter();
	f32 h;
	bool ret = adt->getHeight(v.X, v.Z, h);
	ASSERT(ret);
	v.Y = h;

	AbsoluteTransformation.transformVect(v);
	return v;
}

bool CMapTileSceneNode::getHeightNormal( f32 x, f32 z, f32* height, vector3df* normal ) const
{
	vector3df v(x, 0, z);
	matrix4 mat = AbsoluteTransformation;
	mat.makeInverse();
	mat.transformVect(v);
		
	CFileADT* adt = static_cast<CFileADT*>(Block.tile->fileAdt);

	f32 h;
	if (normal && !adt->getNormal(v.X, v.Z, *normal))			//don't transform normal
		return false;

	if (height)
	{
		if (!adt->getHeight(v.X, v.Z, h))
			return false;

		v.Y = h;
		AbsoluteTransformation.transformVect(v);

		*height = v.Y;
	}

	return true;
}

void CMapTileSceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	if (!Block.tile || !Block.tile->fileAdt)
		return;

	//collect
	Block.HighResChunkRenderList.clear();
	Block.LowResChunkRenderList.clear();

	for (u8 row=0; row<16; ++row)
	{
		for (u8 col=0; col<16; ++col)
		{
			const SDynChunk& dynChunk = Block.DynChunks[row][col];

			if (dynChunk.highres)		//collect high
			{
				addChunkRenderList(true, row, col);
			}

			if (dynChunk.lowres)		//collect low
			{
				addChunkRenderList(false, row, col);
			}
		}
	}
}

void CMapTileSceneNode::render() const
{
	if (!Block.tile || !Block.tile->fileAdt)
		return;

	renderChunkRenderList(true);
	renderChunkRenderList(false);
}

void CMapTileSceneNode::addChunkRenderList( bool high, u8 row, u8 col )
{
	CMapBlock* block = &Block;
	CFileADT* adt = static_cast<CFileADT*>(block->tile->fileAdt);

	const CMapChunk* chunk = adt->getChunk(row, col);
	u8 k = row * 16 + col;

	CMapBlock::T_ChunkRenderList& renderList = high ? block->HighResChunkRenderList : block->LowResChunkRenderList;

	bool find = false;
	for (CMapBlock::T_ChunkRenderList::iterator itr = renderList.begin(); itr != renderList.end(); ++itr)
	{
		u8 num = itr->row * 16 + itr->col + itr->chunkCount;
		if (num == k)		//连续并且纹理相同，可以合并
		{
			const CMapChunk* srcChunk = adt->getChunk(itr->row, itr->col);
			if (srcChunk && srcChunk->hasSameTexture(*chunk))
			{
				++itr->chunkCount;
				find = true;
			}
			break;
		}
	}
	if (!find)		//增加
	{
		SChunkRenderUnit unit;
		unit.row = row;
		unit.col = col;
		unit.chunkCount = 1;
		renderList.push_back(unit);
	}
}

void CMapTileSceneNode::renderChunkRenderList( bool high ) const
{
	const CMapBlock* block = &Block;
	CFileADT* adt = static_cast<CFileADT*>(block->tile->fileAdt);
	if (adt->getNumTextures() == 0)
		return;

	const CMapBlock::T_ChunkRenderList& renderList = high ? block->HighResChunkRenderList : block->LowResChunkRenderList;

	CTerrainServices* terrainServices = static_cast<CTerrainServices*>(g_Engine->getTerrainServices());
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	for (CMapBlock::T_ChunkRenderList::const_iterator itr = renderList.begin(); itr != renderList.end(); ++itr)
	{
		SRenderUnit unit = {0};

		const CMapChunk* chunk = adt->getChunk(itr->row, itr->col);		//start chunk

		unit.bufferParam.vbuffer0 = adt->getVBuffer();
		unit.bufferParam.vType = adt->getVertexType();

#ifdef USE_WITH_GLES2
		unit.drawParam.voffset0 = adt->getChunkVerticesOffset(itr->row, itr->col);
#else
		unit.drawParam.baseVertIndex = adt->getChunkVerticesOffset(itr->row, itr->col);
#endif

		unit.drawParam.numVertices = terrainServices->getVertexCount() * itr->chunkCount;
		unit.primType = EPT_TRIANGLES;	
		unit.sceneNode = this;
		unit.material = Material;
		unit.u.chunk = (CMapChunk*)chunk;
		unit.u.adt = adt;
		unit.matWorld = &AbsoluteTransformation;

		if (high)		
		{
			unit.u.lowres = false;
			unit.bufferParam.ibuffer = terrainServices->getHighResIndexBuffer();	
			unit.primCount = terrainServices->getHighResPrimCount() * itr->chunkCount;
		}
		else
		{
			unit.u.lowres = true;
			unit.bufferParam.ibuffer = terrainServices->getLowResIndexBuffer();	
			unit.primCount = terrainServices->getLowResPrimCount() * itr->chunkCount;
		}

		sceneRenderServices->addRenderUnit(&unit, ERT_TERRAIN);
	}
}

bool CMapTileSceneNode::isNodeEligible() const
{
	aabbox3df box = getWorldBoundingBox();

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if (!cam)
		return false;
	return cam->getViewFrustum().isInFrustum(box);
}

void CMapTileSceneNode::addM2SceneNodes()
{
	TileScene->addM2SceneNodes();
}

void CMapTileSceneNode::addWMOSceneNodes()
{
	TileScene->addWMOSceneNodes();
}

void CMapTileSceneNode::registerVisibleChunks(ICamera* cam)
{
	CFileADT* adt = static_cast<CFileADT*>(Block.tile->fileAdt);
	frustum f = cam->getViewFrustum();
	plane3df clipPlane = cam->getTerrainClipPlane();

	//chunk
	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{
			const CMapChunk* chunk = adt->getChunk(i, j);
			SDynChunk* dynChunk = &Block.DynChunks[i][j];

			//找到camera所在的chunk
			aabbox3df box = dynChunk->terrianWorldBox;
			box.MinEdge.Y = -FLT_MAX;
			box.MaxEdge.Y = FLT_MAX;
			if (!TileScene->getCameraChunk() && box.isPointInside(cam->getPosition()))
			{
				TileScene->setCameraChunk((CMapChunk*)chunk);
			}

			if (!f.isInFrustum(dynChunk->terrianWorldBox))
			{
				dynChunk->lowres = false;
				dynChunk->highres = false;
				continue;
			}

			EIntersectionRelation3D intersect = dynChunk->terrianWorldBox.classifyPlaneRelation(clipPlane);
			switch(intersect)
			{
			case ISREL3D_FRONT:
				dynChunk->highres = true;
				dynChunk->lowres = false;
				break;
			case ISREL3D_CLIPPED:
				dynChunk->highres = true;
				dynChunk->lowres = true;
				break;
			case ISREL3D_BACK:
				dynChunk->highres = false;
				dynChunk->lowres = true;
				break;
			default:
				ASSERT(false);
			}
		}
	}
}

void CMapTileSceneNode::onUpdated()
{
	IMapTileSceneNode::onUpdated();
	
	CFileADT* adt = static_cast<CFileADT*>(Block.tile->fileAdt);

	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{
			const CMapChunk* chunk = adt->getChunk(i, j);
			SDynChunk* dynChunk = &Block.DynChunks[i][j];
			dynChunk->terrianWorldBox = chunk->box;
			AbsoluteTransformation.transformBox(dynChunk->terrianWorldBox);
		}
	}
}

void CMapTileSceneNode::startLoadingM2SceneNodes()
{
	TileScene->startLoadingM2SceneNodes();
}

void CMapTileSceneNode::startLoadingWMOSceneNodes()
{
	TileScene->startLoadingWMOSceneNodes();
}

