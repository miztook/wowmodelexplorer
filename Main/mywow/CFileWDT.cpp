#include "stdafx.h"
#include "CFileWDT.h"
#include "mywow.h"
#include "CMapEnvironment.h"
#include "CFileADT.h"

CFileWDT::CFileWDT()
{
	FileData = nullptr;
	TileOffsets = new int32_t[TILENUM * TILENUM];
	MapEnvironment = nullptr;
	MapId = 0;

	AdtRow = AdtCol = -1;

	::memset(Name, 0, sizeof(Name));
	::memset(MapName, 0, sizeof(MapName));
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
}

bool CFileWDT::loadFile( IMemFile* file, int32_t mapid )
{
	MapId = mapid;

	const SMapRecord* mapRecord = g_Engine->getWowDatabase()->getMapById(MapId);
	if (mapRecord)
	{
		Q_strcpy(MapName, DEFAULT_SIZE, mapRecord->name);
	}

	const char* name = file->getFileName();
	getFullFileNameNoExtensionA(name, Name, QMAX_PATH);

	char fourcc[5];
	uint32_t size;

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

		uint32_t nextpos = file->getPos() + size;
		if (strcmp(fourcc, "MVER") == 0)
		{
			uint32_t version;
			file->read(&version, size);
			ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MPHD") == 0)
		{
		}
		else if (strcmp(fourcc, "MAIN") == 0)
		{
			ASSERT(size == TILENUM * TILENUM * 8);

			Tiles.clear();
			for (uint8_t i=0; i<TILENUM; ++i)			//col
			{
				for (uint8_t j=0; j<TILENUM; ++j)				//row
				{
					char adtname[QMAX_PATH];
					Q_sprintf(adtname, QMAX_PATH, "%s_%d_%d", Name, j, i);
					Q_strcat(adtname, QMAX_PATH, ".adt");
					if(g_Engine->getWowEnvironment()->exists(adtname))
					{
						STile tile; 
						tile.row = i;
						tile.col = j;
											
						Tiles.push_back(tile);
						TileLookup[M_MAKEWORD(j, i)] = (uint32_t)Tiles.size() - 1;
					}
				}
			}
		}
		else if (strcmp(fourcc, "MWMO") == 0)
		{
			ASSERT(size <= 256);
			file->read(GlobalWmoFileName, size);
		}
		else if (strcmp(fourcc, "MODF") == 0)
		{
			ASSERT(size == sizeof(ADT::SWMOPlacement));
			ADT::SWMOPlacement placement;
			file->read(&placement, sizeof(ADT::SWMOPlacement));
			GlobalWmoInstance.wmoIndex = placement.wmoIndex;
			GlobalWmoInstance.id = placement.id;
			vector3df position = placement.pos;
			vector3df dir = vector3df(-placement.dir.X, placement.dir.Y - 90, 0);
			ASSERT(placement.flags == 0);
			GlobalWmoInstance.box.MinEdge = placement.pos2;
			GlobalWmoInstance.box.MaxEdge = placement.pos3;
			GlobalWmoInstance.pos = placement.pos;
			GlobalWmoInstance.dir = placement.dir;
			GlobalWmoInstance.scale = 1.0f;
			GlobalWmoInstance.mat.setRotationDegrees(dir);
			GlobalWmoInstance.mat.setTranslation(position);
			ASSERT(placement.unknown == 0);
		}
		else
		{
			ASSERT(false);
		}

		file->seek((int)nextpos);
	}

	loadWDL();

	loadTEX();

	ASSERT(!MapEnvironment);
	MapEnvironment = new CMapEnvironment(mapid);

	//1st adt
	if (!Tiles.empty())
	{
		STile* tile = &Tiles[0];
		if(loadADTData(tile))
		{
			AdtPosition = static_cast<CFileADT*>(tile->fileAdt)->getBoundingBox().MinEdge;
			AdtRow = (int32_t)tile->row;
			AdtCol = (int32_t)tile->col;
			unloadADT(tile);
		}
	}
	
	return true;
}

bool CFileWDT::loadFileSimple( IMemFile* file, int32_t mapid )
{
	MapId = mapid;

	const SMapRecord* mapRecord = g_Engine->getWowDatabase()->getMapById(MapId);
	if (mapRecord)
	{
		Q_strcpy(MapName, DEFAULT_SIZE, mapRecord->name);
	}

	const char* name = file->getFileName();
	getFullFileNameNoExtensionA(name, Name, QMAX_PATH);

	Tiles.clear();
	for (uint8_t i=0; i<TILENUM; ++i)			//col
	{
		for (uint8_t j=0; j<TILENUM; ++j)				//row
		{
			char adtname[QMAX_PATH];
			Q_sprintf(adtname, QMAX_PATH, "%s_%d_%d", Name, j, i);
			Q_strcat(adtname, QMAX_PATH, ".adt");
			if(g_Engine->getWowEnvironment()->exists(adtname))
			{
				STile tile; 
				tile.row = i;
				tile.col = j;

				Tiles.push_back(tile);
				TileLookup[M_MAKEWORD(j, i)] = (uint32_t)Tiles.size() - 1;
			}
		}
	}
	return true;
}

bool CFileWDT::getPositionByTile( int32_t row, int32_t col, vector3df& pos )
{
	if(AdtRow == -1 || AdtCol == -1)		//1st adt not read
		return false;

	pos.Z = (row - AdtRow + 0.5f) * TILESIZE;
	pos.X = (AdtCol - col - 0.5f) * TILESIZE;
	pos.Y = 0;
	return true;
}

bool CFileWDT::getTileByPosition(vector3df pos, int32_t& row, int32_t& col)
{
	if(AdtRow == -1 || AdtCol == -1)		//1st adt not read
		return false;

	float z = pos.Z - AdtPosition.Z;
	float x = AdtPosition.X - pos.X;
	row = AdtRow + (int32_t)floorf(z / TILESIZE);
	col = AdtCol + (int32_t)ceilf(x / TILESIZE);
	return true;
}

STile* CFileWDT::getTile( uint8_t row, uint8_t col )
{
	uint16_t n = M_MAKEWORD(col, row);
	T_TileLookup::const_iterator itr = TileLookup.find(n);
	if (itr == TileLookup.end())
		return nullptr;
	return &Tiles[itr->second];
}

bool CFileWDT::loadADT( STile* tile, bool simple )
{
	if (!tile || tile->fileAdt)
		return false;

	char adtname[QMAX_PATH];
	getADTFileName(tile->row, tile->col, adtname, QMAX_PATH);

	IFileADT* adt = g_Engine->getResourceLoader()->loadADT(adtname, simple, !simple);
	ASSERT(adt);
	if (adt)
	{
		adt->SetPosition(tile->row, tile->col);
		tile->fileAdt = adt;
	}

	return adt != nullptr;
}

bool CFileWDT::loadADTTextures(STile* tile)
{
	if (!tile || tile->fileAdt)
		return false;

	char adtname[QMAX_PATH];
	getADTFileName(tile->row, tile->col, adtname, QMAX_PATH);

	IFileADT* adt = g_Engine->getResourceLoader()->loadADTTextures(adtname);
	if (adt)
	{
		adt->SetPosition(tile->row, tile->col);
		tile->fileAdt = adt;
	}

	return adt != nullptr;
}

bool CFileWDT::unloadADT( STile* tile )
{
	if (!tile || !tile->fileAdt)
 		return false;

	tile->fileAdt->drop();
	tile->fileAdt = nullptr;

	return true;
}

void CFileWDT::loadWDL()
{
	char name[QMAX_PATH];
	Q_strcpy(name, QMAX_PATH, Name);
	Q_strcat(name, QMAX_PATH, ".wdl");

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return;

	char fourcc[5];
	uint32_t size;

	int countMare = 0;
	int countMaho = 0;
	int countMaoc = 0;

	while (!file->isEof())
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		uint32_t nextpos = file->getPos() + size;
		if (strcmp(fourcc, "MVER") == 0)
		{
			uint32_t version;
			file->read(&version, 4);
			ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MWMO") == 0)			//Filenames for WMO that appear in the low resolution map
		{
			ASSERT(WmoFileNameBlock == 0);
			WmoFileNameBlock = new char[size];
			file->read(WmoFileNameBlock, size);
		} 
		else if (strcmp(fourcc, "MWID") == 0)				//List of indexes into the MWMO chunk		
		{
			ASSERT(WmoFileNameIndices == 0);
			NumWmoFileNames = size / sizeof(uint32_t);
			WmoFileNameIndices = new uint32_t[NumWmoFileNames];
			file->read(WmoFileNameIndices, NumWmoFileNames * sizeof(uint32_t));
		} 
		else if (strcmp(fourcc, "MODF") == 0)				//Placement information for the WMO		
		{
			NumWmoInstance = size / sizeof(ADT::SWMOPlacement);
			if (NumWmoInstance)
			{
				WmoInstances = new SWmoInstance[NumWmoInstance];
				ADT::SWMOPlacement placement;
				for (uint32_t i=0; i<NumWmoInstance; ++i)
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
			++countMare;
		}
		else if (strcmp(fourcc, "MAHO") == 0)				
		{	

			++countMaho;
		}
		else if (strcmp(fourcc, "MAOC") == 0)			//new in 6.0	
		{	

			++countMaoc;
		}
		else if (strcmp(fourcc, "MLMD") == 0)			//new in 7.0	
		{	

		}
		else if (strcmp(fourcc, "MLMX") == 0)			//new in 7.0	
		{	

		}
		else if (strcmp(fourcc, "MAOE") == 0)			//new in 7.0	
		{	

		}
		else
		{
			ASSERT(false);
		}

		file->seek((int)nextpos);
	}
	delete file;
}

void CFileWDT::loadTEX()
{
	char name[QMAX_PATH];
	Q_strcpy(name, QMAX_PATH, Name);
	Q_strcat(name, QMAX_PATH, ".tex");

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return;

	char fourcc[5];
	uint32_t size;

	while (!file->isEof())
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		uint32_t nextpos = file->getPos() + size;
		if (strcmp(fourcc, "TXVR") == 0)
		{
			uint32_t version;
			file->read(&version, 4);
			ASSERT(version == 0);
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
			ASSERT(TextureFileNameBlock == 0);
			TextureFileNameBlock = new char[size];
			file->read(TextureFileNameBlock, size);
		}
		else if (strcmp(fourcc, "TXMD") == 0)
		{
			uint8_t* buffer = (uint8_t*)Z_AllocateTempMemory(size);
			file->read(buffer, size);
			Z_FreeTempMemory(buffer);
		}
		else
		{
			ASSERT(false);
		}

		file->seek((int)nextpos);
	}

	delete file;
}

const char* CFileWDT::getWMOFileName( uint32_t index ) const
{
	if (index >= NumWmoFileNames)
		return nullptr;
	return (const char*)&WmoFileNameBlock[WmoFileNameIndices[index]];
}

void CFileWDT::getADTFileName( uint8_t row, uint8_t col, char* name, uint32_t size ) const
{
	ASSERT(size >= QMAX_PATH);
	Q_sprintf(name, QMAX_PATH, "%s_%d_%d", Name, col, row);
	Q_strcat(name, QMAX_PATH, ".adt");
}

bool CFileWDT::loadADTData( STile* tile )
{
	if (!tile || tile->fileAdt)
		return false;

	char adtname[QMAX_PATH];
	getADTFileName(tile->row, tile->col, adtname, QMAX_PATH);

	IFileADT* adt = g_Engine->getResourceLoader()->loadADT(adtname, false, false);
	ASSERT(adt);
	if (adt)
	{
		adt->SetPosition(tile->row, tile->col);
		tile->fileAdt = adt;
	}

	return adt != nullptr;
}


