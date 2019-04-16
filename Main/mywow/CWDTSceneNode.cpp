#include "stdafx.h"
#include "CWDTSceneNode.h"
#include "mywow.h"
#include "CFileWDT.h"
#include "CMapEnvironment.h"
#include "CSkySceneNode.h"
#include "wow_wdtScene.h"
#include "CSceneRenderServices.h"
#include "CTerrainServices.h"
#include "CFileADT.h"
#include "CFileWDT.h"
	
CWDTSceneNode::CWDTSceneNode( IFileWDT* wdt, ISceneNode* parent )
	: IWDTSceneNode(parent)
{
	FileWDT = static_cast<CFileWDT*>(wdt);

	FileWDT->grab();

	Material.VertexShader = g_Engine->getDriver()->getShaderServices()->getVertexShader(EVST_TERRAIN);
	Material.MaterialType = EMT_TERRAIN_MULTIPASS;
	Material.Lighting = true;
	Material.Cull = ECM_BACK;
	Material.FogEnable = true;

//scene
	WdtScene= new wow_wdtScene(this);

	Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s", FileWDT->getMapName());
}

CWDTSceneNode::~CWDTSceneNode()
{
	delete WdtScene;

	FileWDT->drop();
}

void CWDTSceneNode::setCurrentTile( int32_t row, int32_t col )
{
	WdtScene->setCurrentTile(row, col);
}

vector3df CWDTSceneNode::getCenter() const
{
	vector3df position(0,0,0);
	if (MapBlocks[0].tile && 
		MapBlocks[0].tile->row == WdtScene->getRow() && 
		MapBlocks[0].tile->col == WdtScene->getCol())
	{
		ASSERT(MapBlocks[0].tile->fileAdt);
		position = static_cast<CFileADT*>(MapBlocks[0].tile->fileAdt)->getBoundingBox().getCenter();
	}
	else
	{
		FileWDT->getPositionByTile(WdtScene->getRow(), WdtScene->getCol(), position);
	}

	AbsoluteTransformation.transformVect(position);
	return position;
}

bool CWDTSceneNode::getHeightNormal( float x, float z, float* height, vector3df* normal ) const
{
	vector3df v(x, 0, z);
	int32_t row, col;
	if (!getTileByPosition(v, row, col))
		return false;

	matrix4 mat = AbsoluteTransformation;			//到原点空间
	mat.makeInverse();
	mat.transformVect(v);

	uint32_t num = WdtScene->getNumBlocks();				//在已加载的block中找高度
	for (uint32_t i=0; i<num; ++i)
	{
		const CMapBlock* block = &MapBlocks[i];
		CFileADT* adt = static_cast<CFileADT*>(block->tile->fileAdt);
		if (block->tile->row == (uint8_t)row && block->tile->col == (uint8_t)col)
		{
			float h;
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
	}

	return false;
}

vector3df CWDTSceneNode::getPositionByTile( int32_t row, int32_t col ) const
{
	vector3df position(0,0,0);

	FileWDT->getPositionByTile(row, col, position);

	AbsoluteTransformation.transformVect(position);
	return position;
}

bool CWDTSceneNode::getTileByPosition( vector3df pos, int32_t& row, int32_t& col ) const
{
	matrix4 mat;
	if(!AbsoluteTransformation.getInverse(mat))
		return false;

	vector3df position = pos;
	mat.transformVect(position);
	return FileWDT->getTileByPosition(position, row, col);
}

void CWDTSceneNode::registerSceneNode( bool frustumcheck, int sequence )
{
	update();

	WdtScene->update();

	if (!Visible)
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);

	IWDTSceneNode::registerSceneNode(frustumcheck, sequence);

	const ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();

	WdtScene->setCameraChunk(nullptr);
	uint32_t num = WdtScene->getNumBlocks();
	for (uint32_t i=0; i<num; ++i)
	{
		registerVisibleChunks(i, cam);
	}
}

void CWDTSceneNode::tick( uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible)
{
	uint32_t num = WdtScene->getNumBlocks();
	for (uint32_t i=0; i<num; ++i)
	{
		collectBlockRenderList(i);	
	}
}

void CWDTSceneNode::render() const
{
	//debug
// 	ITexture* tex = MapBlock[1][1].tile->fileAdt->getBlendMap();
// 	g_Engine->getDrawServices()->draw2DImage(tex, vector2di(0,0), true);
// 	return; 

	uint32_t num = WdtScene->getNumBlocks();
	for (uint32_t i=0; i<num; ++i)
	{
		renderMapBlock(i);	
	}

// 	if (CamChunk)
// 	{
// 		g_Engine->getDrawServices()->draw2DImage(g_Engine->getFont()->getTexture(0), vector2di(300,100), true);
// 	}
}

void CWDTSceneNode::onUpdated()
{
	IWDTSceneNode::onUpdated();

	uint32_t num = WdtScene->getNumBlocks();
	for (uint32_t i=0; i<num; ++i)
	{
		updateMapBlock(i);
	}
}

void CWDTSceneNode::registerVisibleChunks( uint32_t blockIndex, const ICamera* cam )
{
	CMapBlock* block = &MapBlocks[blockIndex];

	CFileADT* adt = nullptr;
	if (!block->tile || !(adt = static_cast<CFileADT*>(block->tile->fileAdt)))
		return;

	frustum f = cam->getViewFrustum();
	plane3df clipPlane = cam->getTerrainClipPlane();

	//chunk
	for (uint8_t i=0; i<16; ++i)
	{
		for (uint8_t j=0; j<16; ++j)
		{
			const CMapChunk* chunk = adt->getChunk(i, j);
			SDynChunk* dynChunk = &block->DynChunks[i][j];

			//找到camera所在的chunk
			aabbox3df box = dynChunk->terrianWorldBox;
			box.MinEdge.Y = -FLT_MAX;
			box.MaxEdge.Y = FLT_MAX;
			if (!WdtScene->getCameraChunk() && box.isPointInside(cam->getPosition()))
			{
				WdtScene->setCameraChunk((CMapChunk*)chunk);
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

void CWDTSceneNode::updateMapBlock( uint32_t blockIndex )
{
	CMapBlock* block = &MapBlocks[blockIndex];

	CFileADT* adt = nullptr;
	if (!block->tile || !(adt = static_cast<CFileADT*>(block->tile->fileAdt)))
		return;

	for (uint8_t i=0; i<16; ++i)
	{
		for (uint8_t j=0; j<16; ++j)
		{
			const CMapChunk* chunk = adt->getChunk(i, j);
			SDynChunk* dynChunk = &block->DynChunks[i][j];
			dynChunk->terrianWorldBox = chunk->box;
			AbsoluteTransformation.transformBox(dynChunk->terrianWorldBox);
		}
	}
}

void CWDTSceneNode::collectBlockRenderList(uint32_t blockIndex)
{
	CMapBlock* block = &MapBlocks[blockIndex];

	//collect
	block->HighResChunkRenderList.clear();
	block->LowResChunkRenderList.clear();

	CFileADT* adt = nullptr;
	if (!block->tile || !(adt = static_cast<CFileADT*>(block->tile->fileAdt)))
		return;

	if (adt->getNumTextures() == 0)
		return;

	for (uint8_t row=0; row<16; ++row)
	{
		for (uint8_t col=0; col<16; ++col)
		{
			const SDynChunk& dynChunk = block->DynChunks[row][col];

			if (dynChunk.highres)		//collect high
			{
				addChunkRenderList(blockIndex, true, row, col);
			}

			if (dynChunk.lowres)		//collect low
			{
				addChunkRenderList(blockIndex, false, row, col);
			}
		}
	}
}

void CWDTSceneNode::renderMapBlock( uint32_t blockIndex ) const
{
	renderChunkRenderList(blockIndex, true);
	renderChunkRenderList(blockIndex, false);
}

void CWDTSceneNode::addChunkRenderList( uint32_t blockIndex, bool high, uint8_t row, uint8_t col )
{
	CMapBlock* block = &MapBlocks[blockIndex];
	CFileADT* adt = static_cast<CFileADT*>(block->tile->fileAdt);

	const CMapChunk* chunk = adt->getChunk(row, col);
	uint8_t k = row * 16 + col;
		
	CMapBlock::T_ChunkRenderList& renderList = high ? block->HighResChunkRenderList : block->LowResChunkRenderList;

	bool find = false;
	for (CMapBlock::T_ChunkRenderList::iterator itr = renderList.begin(); itr != renderList.end(); ++itr)
	{
		uint8_t num = itr->row * 16 + itr->col + itr->chunkCount;
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

void CWDTSceneNode::renderChunkRenderList( uint32_t blockIndex, bool high ) const
{
	const CMapBlock* block = &MapBlocks[blockIndex];
	CFileADT* adt = static_cast<CFileADT*>(block->tile->fileAdt);

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

void CWDTSceneNode::setAdtLoadSize( E_ADT_LOAD size )
{
	WdtScene->setAdtLoadSize(size);
}



