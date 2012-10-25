#include "stdafx.h"
#include "CFileWDT.h"
#include "mpq_libmpq.h"
#include "mywow.h"
#include "CMapEnvironment.h"

IFileWDT* CWDTLoader::loadWDT( MPQFile* file, s32 mapid )
{
	CFileWDT* wdtFile = new CFileWDT;
	if (!wdtFile->loadFile(file, mapid))
	{
		wdtFile->drop();
		return NULL;
	}
	return wdtFile;
}

CFileWDT::CFileWDT()
{
	FileData = NULL;
	TileOffsets = new s32[TILENUM * TILENUM];
	MapEnvironment = NULL;
	MapId = 0;

	AdtRow = AdtCol = -1;

	::memset(MapName, 0, sizeof(c16)*DEFAULT_SIZE);
}

CFileWDT::~CFileWDT()
{
	delete MapEnvironment;

	delete[] TextureFileNameBlock;
	delete[] TextureInfos;
	delete[] TileOffsets;
	delete[] WmoInstances;
	delete[] WmoFileNameIndices;
	delete[] WmoFileNameBlock;

	for (u32 i=0; i<Tiles.size(); ++i)
	{
		STile* t = &Tiles[i];
		if (t->heightmapOuter)
			delete[] t->heightmapOuter;

		if (t->heightmapInner)
			delete[] t->heightmapInner;
	}
}

bool CFileWDT::loadFile( MPQFile* file, s32 mapid )
{
	MapId = mapid;

	SMapRecord* mapRecord = g_Engine->getWowDatabase()->getMapById(MapId);
	if (mapRecord)
	{
		wcscpy_s(MapName, DEFAULT_SIZE, mapRecord->name);
	}

	const c8* name = file->getFileName();
	getFileNameNoExtensionA(name, Name, MAX_PATH);

	c8 fourcc[5];
	u32 size;

	// WDT files specify exactly which map tiles are present in a world, 
	// if any, and can also reference a "global" WMO. They have a chunked file structure.
	while (!file->isEof())
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		u32 nextpos = file->getPos() + size;
		if (strcmp(fourcc, "MVER") == 0)
		{
			u32 version;
			file->read(&version, size);
			_ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MPHD") == 0)
		{
		}
		else if (strcmp(fourcc, "MAIN") == 0)
		{
			_ASSERT(size == TILENUM * TILENUM * 8);

			for (u8 i=0; i<TILENUM; ++i)			//col
			{
				for (u8 j=0; j<TILENUM; ++j)				//row
				{
					c8 adtname[MAX_PATH];
					sprintf_s(adtname, MAX_PATH, "%s_%d_%d", Name, j, i);
					strcat_s(adtname, MAX_PATH, ".adt");
					if(g_Engine->getWowEnvironment()->exists(adtname))
					{
						STile tile; 
						tile.row = i;
						tile.col = j;
											
						Tiles.push_back(tile);
						TileLookup[MAKEWORD(j, i)] = Tiles.size() - 1;
					}
				}
			}
		}
		else if (strcmp(fourcc, "MWMO") == 0)
		{
			_ASSERT(size <= 256);
			file->read(GlobalWmoFileName, size);
		}
		else if (strcmp(fourcc, "MODF") == 0)
		{
			_ASSERT(size == sizeof(ADT::SWMOPlacement));
			ADT::SWMOPlacement placement;
			file->read(&placement, sizeof(ADT::SWMOPlacement));
			GlobalWmoInstance.wmoIndex = placement.wmoIndex;
			GlobalWmoInstance.id = placement.id;
			vector3df position = placement.pos;
			vector3df dir = vector3df(-placement.dir.X, placement.dir.Y - 90, 0);
			_ASSERT(placement.flags == 0);
			GlobalWmoInstance.box.MinEdge = placement.pos2;
			GlobalWmoInstance.box.MaxEdge = placement.pos3;
			GlobalWmoInstance.pos = placement.pos;
			GlobalWmoInstance.dir = placement.dir;
			GlobalWmoInstance.scale = 1.0f;
			GlobalWmoInstance.mat.setRotationDegrees(dir);
			GlobalWmoInstance.mat.setTranslation(position);
			_ASSERT(placement.unknown == 0);
		}
		else
		{
			_ASSERT(false);
		}

		file->seek((int)nextpos);
	}

	loadWDL();

	loadTEX();

	_ASSERT(!MapEnvironment);
	MapEnvironment = new CMapEnvironment(mapid);

	//1st adt
	STile* tile = &Tiles[0];
	if(loadADT(tile))
	{
		AdtPosition = tile->fileAdt->getBoundingBox().MinEdge;
		AdtRow = (s32)tile->row;
		AdtCol = (s32)tile->col;
		unloadADT(tile);
	}

	return true;
}

bool CFileWDT::getPositionByTile( s32 row, s32 col, vector3df& pos )
{
	if(AdtRow == -1 || AdtCol == -1)		//1st adt not read
		return false;

	pos.Z = (row - AdtRow + 0.5f) * TILESIZE;
	pos.X = (col - AdtCol + 0.5f) * TILESIZE;
	pos.Y = 0;
	return true;
}

bool CFileWDT::getTileByPosition(vector3df pos, s32& row, s32& col)
{
	if(AdtRow == -1 || AdtCol == -1)		//1st adt not read
		return false;

	f32 z = pos.Z - AdtPosition.Z;
	f32 x = pos.X - AdtPosition.X;
	row = AdtRow + (s32)floorf(z / TILESIZE);
	col = AdtCol + (s32)floorf(x / TILESIZE);
	return true;
}

STile* CFileWDT::getTile( u8 row, u8 col )
{
	u16 n = MAKEWORD(col, row);
	T_TileLookup::iterator itr = TileLookup.find(n);
	if (itr == TileLookup.end())
		return NULL;
	return &Tiles[itr->second];
}

STile* CFileWDT::getTile( u32 index )
{
	if (index >= Tiles.size())
		return NULL;
	return &Tiles[index];
}

bool CFileWDT::loadADT( STile* tile )
{
	if (!tile || tile->fileAdt)
		return false;

	c8 adtname[MAX_PATH];
	getADTFileName(tile->row, tile->col, adtname, MAX_PATH);

	IFileADT* adt = g_Engine->getResourceLoader()->loadADT(adtname);
	_ASSERT(adt);
	if (adt)
	{
		adt->SetPosition(tile->row, tile->col);
		tile->fileAdt = adt;
		adt->buildVideoResources();
	}

	return adt != NULL;
}

bool CFileWDT::unloadADT( STile* tile )
{
	if (!tile || !tile->fileAdt)
 		return false;

	tile->fileAdt->drop();
	tile->fileAdt = NULL;

	return true;
}

void CFileWDT::loadWDL()
{
	c8 name[MAX_PATH];
	strcpy_s(name, MAX_PATH, Name);
	strcat_s(name, MAX_PATH, ".wdl");

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return;

	c8 fourcc[5];
	u32 size;

	int countMare = 0;
	int countMaho = 0;

	while (!file->isEof())
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		u32 nextpos = file->getPos() + size;
		if (strcmp(fourcc, "MVER") == 0)
		{
			u32 version;
			file->read(&version, 4);
			_ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MWMO") == 0)			//Filenames for WMO that appear in the low resolution map
		{
			_ASSERT(WmoFileNameBlock == 0);
			WmoFileNameBlock = new c8[size];
			file->read(WmoFileNameBlock, size);
		} 
		else if (strcmp(fourcc, "MWID") == 0)				//List of indexes into the MWMO chunk		
		{
			_ASSERT(WmoFileNameIndices == 0);
			NumWmoFileNames = size / sizeof(u32);
			WmoFileNameIndices = new u32[NumWmoFileNames];
			file->read(WmoFileNameIndices, NumWmoFileNames * sizeof(u32));
		} 
		else if (strcmp(fourcc, "MODF") == 0)				//Placement information for the WMO		
		{
			NumWmoInstance = size / sizeof(ADT::SWMOPlacement);
			if (NumWmoInstance)
			{
				WmoInstances = new SWmoInstance[NumWmoInstance];
				ADT::SWMOPlacement placement;
				for (u32 i=0; i<NumWmoInstance; ++i)
				{	
					file->read(&placement, sizeof(ADT::SWMOPlacement));
					WmoInstances[i].wmoIndex = placement.wmoIndex;
					WmoInstances[i].id = placement.id;
					WmoInstances[i].pos = placement.pos;
					WmoInstances[i].dir = placement.dir;
				}
			}
			
		} 
		else if (strcmp(fourcc, "MAOF") == 0)					//Map Area Offset
		{
			file->read(TileOffsets, size);
		}
		else if (strcmp(fourcc, "MARE") == 0)				
		{
			STile& tile = Tiles[countMare];
			s32 offset = TileOffsets[tile.row * TILENUM + tile.col];
			if (offset)
			{
				file->seek(offset + 8);
			}

			_ASSERT(tile.heightmapOuter == NULL);
			_ASSERT(tile.heightmapInner == NULL);

			tile.heightmapOuter = new s16[17 * 17];
			tile.heightmapInner = new s16[16 * 16];

			file->read(tile.heightmapOuter, 17 * 17 * sizeof(s16));
			file->read(tile.heightmapInner, 16 * 16 * sizeof(s16));

			++countMare;
		}
		else if (strcmp(fourcc, "MAHO") == 0)				
		{	

			++countMaho;
		}
		else
		{
			_ASSERT(false);
		}

		file->seek((int)nextpos);
	}
	delete file;
}

void CFileWDT::loadTEX()
{
	c8 name[MAX_PATH];
	strcpy_s(name, MAX_PATH, Name);
	strcat_s(name, MAX_PATH, ".tex");

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return;

	c8 fourcc[5];
	u32 size;

	while (!file->isEof())
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		u32 nextpos = file->getPos() + size;
		if (strcmp(fourcc, "TXVR") == 0)
		{
			u32 version;
			file->read(&version, 4);
			_ASSERT(version == 0);
		}
		else if (strcmp(fourcc, "TXBT") == 0)
		{
			NumTextures = size / sizeof(ADT::TXBT);
			if (NumTextures > 0)
			{
				TextureInfos = new ADT::TXBT[NumTextures];
				file->read(TextureInfos, NumTextures * sizeof(ADT::TXBT));
			}
		}
		else if (strcmp(fourcc, "TXFN") == 0)
		{
			_ASSERT(TextureFileNameBlock == 0);
			TextureFileNameBlock = new c8[size];
			file->read(TextureFileNameBlock, size);
		}
		else if (strcmp(fourcc, "TXMD") == 0)
		{
			u8* buffer = (u8*)Hunk_AllocateTempMemory(size);
			file->read(buffer, size);
			Hunk_FreeTempMemory(buffer);
		}
		else
		{
			_ASSERT(false);
		}

		file->seek((int)nextpos);
	}

	delete file;
}

const c8* CFileWDT::getWMOFileName( u32 index ) const
{
	if (index >= NumWmoFileNames)
		return NULL;
	return (const c8*)&WmoFileNameBlock[WmoFileNameIndices[index]];
}

void CFileWDT::getADTFileName( u8 row, u8 col, c8* name, u32 size )
{
	_ASSERT(size >= MAX_PATH);
	sprintf_s(name, MAX_PATH, "%s_%d_%d", Name, col, row);
	strcat_s(name, MAX_PATH, ".adt");
}



