#include "stdafx.h"
#include "CFileADT.h"
#include "mywow.h"

#define  BLENDMAP_SIZE	(64 * 16)

/*
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
		1		2		3		4		5		6		7		8
	1		2		3		4		5		6		7		8		9
*/

CFileADT::CFileADT()
{
	Box.set(vector3df(99999.9f), vector3df(-99999.9f));

	Vertices = new SVertex_PNCT2[16 * 16 * 145];	

	VertexBuffer = new IVertexBuffer;
	VertexBuffer->set(Vertices, EST_PNCT2, 16 * 16 * 145, EMM_STATIC);

	Data_BlendMap =  NULL_PTR;
	BlendMap = NULL_PTR;
	FileData = NULL_PTR;
}

CFileADT::~CFileADT()
{
	if (BlendMap)
		BlendMap->drop();

	delete[] Data_BlendMap;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture)
			Textures[i].texture->drop();
	}

	delete[] WmoInstances;
	delete[] M2Instances;
	delete[] WmoFileNameIndices;
	delete[] WmoFileNameBlock;
	delete[] M2FileNameIndices;
	delete[] M2FileNameBlock;

	delete VertexBuffer;
}

bool CFileADT::loadFile( IMemFile* file )
{
	const c8* name = file->getFileName();
	getFullFileNameNoExtensionA(name, Name, QMAX_PATH);

	c8 fourcc[5];
	u32 size;

	u32 nChunks = 0;

	while( !file->isEof() )
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		u32 nextpos = file->getPos() + size;

		if (strcmp(fourcc, "MVER") == 0)				//version
		{
			u32 version;
			file->read(&version, size);
			ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MHDR") == 0)		//Contains offsets relative to &MHDR.data in the file for specific chunks
		{
		}
		else if (strcmp(fourcc, "MH2O") == 0)
		{
		}
		else if (strcmp(fourcc, "MCNK") == 0)		//After the above mentioned chunks come 256 individual MCNK chunks, row by row
		{
			u32 row = nChunks / 16;
			u32 col = nChunks % 16;

			//read
			readChunk(file, row, col, file->getPos() + size);

			Box.addInternalBox(Chunks[row][col].box);

			++nChunks;
		}
		else if (strcmp(fourcc, "MFBO") == 0)		//A bounding box for flying. 
		{
			ASSERT(size == 36);
		}
		else if (strcmp(fourcc, "MBMH") == 0)
		{

		}
		else if (strcmp(fourcc, "MBBB") == 0)
		{

		}
		else if (strcmp(fourcc, "MBMI") == 0)
		{

		}
		else if (strcmp(fourcc, "MBNV") == 0)
		{

		}
		else
		{
			//MessageBoxA(NULL_PTR, fourcc, "", 0);
			ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	ASSERT(nChunks == 256);

	buildTexcoords();

	//objects
	loadObj0();

	//textures
	loadTex(0);

	buildMaps();

	return true;
}

bool CFileADT::loadFileSimple( IMemFile* file )
{
	const c8* name = file->getFileName();
	getFullFileNameNoExtensionA(name, Name, QMAX_PATH);

	return loadObj0Simple();
}

void CFileADT::readChunk( IMemFile* file, u8 row, u8 col, u32 lastpos)
{
	CMapChunk& currentChunk = Chunks[row][col];

	u32 offset = getChunkVerticesOffset(row, col);
	SVertex_PNCT2* vertices = &Vertices[offset];

	ADT::chunkHeader header;
	file->read(&header, sizeof(ADT::chunkHeader));

	currentChunk.areaID = header.areaid;
	currentChunk.zbase = -header.zpos + ZEROPOINT;
	currentChunk.ybase = header.ypos;
	currentChunk.xbase = header.xpos - ZEROPOINT;
	currentChunk.box.set(vector3df(99999.9f), vector3df(-99999.9f));

	c8 fourcc[5];
	u32 size;

	while( file->getPos() < lastpos )
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		u32 nextpos = file->getPos() + size;

		if (strcmp(fourcc, "MCVT") == 0)			
		{
			ASSERT(size == 145 * sizeof(f32));
			u32 count = 0;
			for (u32 j=0; j<17; ++j)
			{
				for (s32 i=0; i<((j%2)?8:9); ++i)
				{
					f32 h, xpos, zpos;
					file->read(&h, 4);
					xpos = -i * UNITSIZE;
					zpos = j * 0.5f * UNITSIZE;
					if (j%2)
					{
						xpos -= UNITSIZE * 0.5f;
					}

					vector3df v(currentChunk.xbase + xpos, currentChunk.ybase + h, currentChunk.zbase + zpos);
					vertices[count].Pos = v;
					++count;
					if (v.Y < currentChunk.box.MinEdge.Y)
						currentChunk.box.MinEdge.Y = v.Y;
					if (v.Y > currentChunk.box.MaxEdge.Y)
						currentChunk.box.MaxEdge.Y = v.Y;
				}
			}
			currentChunk.box.MinEdge.X = currentChunk.xbase - 8 * UNITSIZE;
			currentChunk.box.MinEdge.Z = currentChunk.zbase;
			currentChunk.box.MaxEdge.X = currentChunk.xbase;
			currentChunk.box.MaxEdge.Z = currentChunk.zbase + 8 * UNITSIZE;
		}
		else if (strcmp(fourcc, "MCNR") == 0)	
		{
			nextpos = file->getPos() + 0x1C0; // size fix

			u32 count = 0;
			u8 nor[3];
			for (u32 j=0; j<17; ++j)
			{
				for (s32 i=0; i<((j%2)?8:9); ++i)
				{
					file->read(nor, 3);
					vector3df n(-(f32)nor[1]/127.0f, (f32)nor[2]/127.0f, -(f32)nor[0]/127.0f);
					vertices[count].Normal = n;
					++count;
				}
			}
		}
		else if (strcmp(fourcc, "MCCV") == 0)
		{
			ASSERT( size == 145 * 4);

			u32 count = 0;
			u8 clr[4];
			for (u32 j=0; j<17; ++j)
			{
				for (s32 i=0; i<((j%2)?8:9); ++i)
				{
					file->read(clr, 4);
					SColor color(clr[3], clr[0], clr[1], clr[2]);
					vertices[count].Color = color;
					++count;
				}
			}
		}
		else if (strcmp(fourcc, "MCLV") == 0)
		{
			ASSERT( size == 145 * 4);

			/*
			It looks like an array of Colors, one per heightmap vertex (i.e. a sequence of 145 x unsigned int). 
			Most of them are 0xFF000000, but some differ - e.g. 0xFF060102. 
			In almost all cases RGB components are very low (0x00 - 0x10).
			*/
		}
		else if (strcmp(fourcc, "MCSE") == 0)
		{
			ASSERT(size % 28 == 0);

			u32 nSounds = size / sizeof(ADT::SSoundEmitter);
			if (nSounds)
			{
				ASSERT(currentChunk.sounds == NULL_PTR);
				currentChunk.sounds = new SChunkSound[nSounds];
				ADT::SSoundEmitter sound;
				for (u32 i=0; i<nSounds; ++i)
				{
					file->read(&sound, sizeof(ADT::SSoundEmitter));
					currentChunk.sounds[i].soundID = sound.SoundEntriesAdvancedId;
					currentChunk.sounds[i].pos = sound.position;
					currentChunk.sounds[i].range = sound.size;
				}
			}
		}
		else if (strcmp(fourcc, "MCLQ") == 0)
		{

		}
		else if (strcmp(fourcc, "MCBB") == 0)
		{

		}
		else
		{
			ASSERT(false);
		}

		file->seek((s32)nextpos);
	}
}

const CMapChunk* CFileADT::getChunk( u8 row, u8 col ) const
{
	ASSERT(row < 16 && col <16);
	return &Chunks[row][col];
}

bool CFileADT::getHeight( f32 x, f32 z, f32& height ) const
{
	//不在这个adt中，返回
	if(x < Chunks[0][0].xbase - TILESIZE || x > Chunks[0][0].xbase ||
		z < Chunks[0][0].zbase || z > Chunks[0][0].zbase + TILESIZE)
	return false;

	//找到所在chunk
	u8 row = (u8)((z - Chunks[0][0].zbase) / CHUNKSIZE);
	u8 col = (u8)((Chunks[0][0].xbase - x) / CHUNKSIZE);
	ASSERT(row < 16 && col < 16);

	//找到所在chunk中的grid
	const f32 zbase = Chunks[row][col].zbase;
	const f32 xbase = Chunks[row][col].xbase;
	const f32 gridsize = UNITSIZE * 0.5f;
	u8 gridRow = (u8)((z - zbase) / gridsize);
	u8 gridCol = (u8)((xbase - x) / gridsize);

	if (gridRow == 16)
	{
		ASSERT(row < 15);
		++row;
		gridRow = 0;
	}

	ASSERT(gridRow < 16 && gridCol < 16);

	//对grid的4个点插值
	vector3df positions[4];
	getGridPosition(row, col, gridRow, gridCol, positions);

	const vector3df& a = positions[0];
	const vector3df& b = positions[1];
	const vector3df& c = positions[2];
	const vector3df& d = positions[3];

	f32 dx = (a.X - x) / gridsize;
	f32 dz = (z - a.Z) / gridsize;

	if (dx > dz)
		height = a.Y + (d.Y - b.Y)*dz + (b.Y - a.Y)*dx;
	else
		height = a.Y + (d.Y - c.Y)*dx + (c.Y - a.Y)*dz;

	return true;
}

bool CFileADT::getNormal( f32 x, f32 z, vector3df& normal ) const
{
	//不在这个adt中，返回
	if(x < Chunks[0][0].xbase - TILESIZE || x > Chunks[0][0].xbase ||
		z < Chunks[0][0].zbase || z > Chunks[0][0].zbase + TILESIZE)
		return false;

	//找到所在chunk
	u8 row = (u8)((z - Chunks[0][0].zbase) / CHUNKSIZE);
	u8 col = (u8)((Chunks[0][0].xbase - x) / CHUNKSIZE);
	ASSERT(row < 16 && col < 16);

	//找到所在chunk中的grid
	const f32 zbase = Chunks[row][col].zbase;
	const f32 xbase = Chunks[row][col].xbase;
	const f32 gridsize = UNITSIZE * 0.5f;
	u8 gridRow = (u8)((z - zbase) / gridsize);
	u8 gridCol = (u8)((xbase - x) / gridsize);
	ASSERT(gridRow < 16 && gridCol < 16);

	//对grid的4个点插值
	vector3df normals[4];
	getGridNormal(row, col, gridRow, gridCol, normals);

	const vector3df& a = normals[0];
	const vector3df& b = normals[1];
	const vector3df& c = normals[2];
	const vector3df& d = normals[3];

	f32 dx = (a.X - x) / gridsize;
	f32 dz = (z - a.Z) / gridsize;

	if (dx > dz)
		normal = a + (d - b)*dz + (b - a)*dx;
	else
		normal = a + (d - c)*dx + (c - a)*dz;

	return true;
}

void CFileADT::getGridPosition( u8 row, u8 col, u8 gridRow, u8 gridCol, vector3df* positions ) const
{
	SVertex_PNCT2* baseVertices = &Vertices[getChunkVerticesOffset(row, col)];

	u8 r = gridRow;
	u8 c = gridCol / 2;

	if (gridRow % 2 == 0)
	{
		if (gridCol % 2 ==0)
		{
			positions[0] = baseVertices[getVertexIndex(r, c)].Pos;
			positions[1] = (baseVertices[getVertexIndex(r, c)].Pos + baseVertices[getVertexIndex(r, c+1)].Pos) * 0.5f;
			positions[2] = (baseVertices[getVertexIndex(r+2, c)].Pos + baseVertices[getVertexIndex(r, c)].Pos) * 0.5f;
			positions[3] = baseVertices[getVertexIndex(r+1, c)].Pos;
		}
		else
		{
			positions[0] = (baseVertices[getVertexIndex(r, c)].Pos + baseVertices[getVertexIndex(r, c+1)].Pos) * 0.5f;
			positions[1] = baseVertices[getVertexIndex(r, c+1)].Pos;
			positions[2] = baseVertices[getVertexIndex(r+1, c)].Pos;
			positions[3] = (baseVertices[getVertexIndex(r, c+1)].Pos + baseVertices[getVertexIndex(r+2, c+1)].Pos) * 0.5f;
		}
	}
	else
	{
		if (gridCol % 2 ==0)
		{
			positions[0] = (baseVertices[getVertexIndex(r-1, c)].Pos + baseVertices[getVertexIndex(r+1, c)].Pos) * 0.5f;
			positions[1] = baseVertices[getVertexIndex(r, c)].Pos;
			positions[2] = baseVertices[getVertexIndex(r+1, c)].Pos;
			positions[3] = (baseVertices[getVertexIndex(r+1, c)].Pos + baseVertices[getVertexIndex(r+1, c+1)].Pos) * 0.5f;
		}
		else
		{
			positions[0] = baseVertices[getVertexIndex(r, c)].Pos;
			positions[1] = (baseVertices[getVertexIndex(r-1, c+1)].Pos + baseVertices[getVertexIndex(r+1, c+1)].Pos) * 0.5f;
			positions[2] = (baseVertices[getVertexIndex(r+1, c)].Pos + baseVertices[getVertexIndex(r+1, c+1)].Pos) * 0.5f;
			positions[3] = baseVertices[getVertexIndex(r+1, c+1)].Pos;
		}
	}
}

void CFileADT::getGridNormal( u8 row, u8 col, u8 gridRow, u8 gridCol, vector3df* normals ) const
{
	SVertex_PNCT2* baseVertices = &Vertices[getChunkVerticesOffset(row, col)];

	u8 r = gridRow;
	u8 c = gridCol / 2;

	if (gridRow % 2 == 0)
	{
		if (gridCol % 2 ==0)
		{
			normals[0] = baseVertices[getVertexIndex(r, c)].Normal;
			normals[1] = (baseVertices[getVertexIndex(r, c)].Normal + baseVertices[getVertexIndex(r, c+1)].Normal) * 0.5f;
			normals[2] = (baseVertices[getVertexIndex(r+2, c)].Normal + baseVertices[getVertexIndex(r, c)].Normal) * 0.5f;
			normals[3] = baseVertices[getVertexIndex(r+1, c)].Normal;
		}
		else
		{
			normals[0] = (baseVertices[getVertexIndex(r, c)].Normal + baseVertices[getVertexIndex(r, c+1)].Normal) * 0.5f;
			normals[1] = baseVertices[getVertexIndex(r, c+1)].Normal;
			normals[2] = baseVertices[getVertexIndex(r+1, c)].Normal;
			normals[3] = (baseVertices[getVertexIndex(r, c+1)].Normal + baseVertices[getVertexIndex(r+2, c+1)].Normal) * 0.5f;
		}
	}
	else
	{
		if (gridCol % 2 ==0)
		{
			normals[0] = (baseVertices[getVertexIndex(r-1, c)].Normal + baseVertices[getVertexIndex(r+1, c)].Normal) * 0.5f;
			normals[1] = baseVertices[getVertexIndex(r, c)].Normal;
			normals[2] = baseVertices[getVertexIndex(r+1, c)].Normal;
			normals[3] = (baseVertices[getVertexIndex(r+1, c)].Normal + baseVertices[getVertexIndex(r+1, c+1)].Normal) * 0.5f;
		}
		else
		{
			normals[0] = baseVertices[getVertexIndex(r, c)].Normal;
			normals[1] = (baseVertices[getVertexIndex(r-1, c+1)].Normal + baseVertices[getVertexIndex(r+1, c+1)].Normal) * 0.5f;
			normals[2] = (baseVertices[getVertexIndex(r+1, c)].Normal + baseVertices[getVertexIndex(r+1, c+1)].Normal) * 0.5f;
			normals[3] = baseVertices[getVertexIndex(r+1, c+1)].Normal;
		}
	}
}

void CFileADT::loadObj0()
{
	c8 name[QMAX_PATH];
	Q_strcpy(name, QMAX_PATH, Name);
	Q_strcat(name, QMAX_PATH, "_obj0.adt");

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return;

	u32 nChunks = 0;

	c8 fourcc[5];
	u32 size;

	while( !file->isEof() )
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
			ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MMDX") == 0)		//List of filenames for M2 models that appear in this map tile. 
		{
			ASSERT(M2FileNameBlock == 0);
			M2FileNameBlock = new c8[size];
			file->read(M2FileNameBlock, size);
		}
		else if (strcmp(fourcc, "MMID") == 0)		//List of offsets of model filenames in the MMDX chunk. 
		{
			NumM2FileNames = size / sizeof(u32);
			ASSERT(M2FileNameIndices == 0);
			M2FileNameIndices = new u32[NumM2FileNames];
			file->read(M2FileNameIndices, NumM2FileNames * sizeof(u32));
		}
		else if (strcmp(fourcc, "MWMO") == 0)	//List of filenames for WMOs (world map objects) that appear in this map tile. 
		{
			ASSERT(WmoFileNameBlock == 0);
			WmoFileNameBlock = new c8[size];
			file->read(WmoFileNameBlock, size);
		}
		else if (strcmp(fourcc, "MWID") == 0)		//List of offsets of WMO filenames in the MWMO chunk.
		{
			NumWmoFileNames = size / sizeof(u32);
			ASSERT(WmoFileNameIndices == 0);
			WmoFileNameIndices = new u32[NumWmoFileNames];
			file->read(WmoFileNameIndices, NumWmoFileNames * sizeof(u32));
		}
		else if (strcmp(fourcc, "MDDF") == 0)		//Placement information for doodads (M2 models). 
		{
			NumM2Instance = size / sizeof(ADT::SM2Placement);
			if (NumM2Instance)
			{
				M2Instances = new SM2Instance[NumM2Instance];
				ADT::SM2Placement placement;
				for (u32 i=0; i<NumM2Instance; ++i)
				{	
					file->read(&placement, sizeof(ADT::SM2Placement));
					M2Instances[i].m2Index = placement.m2Index;
					M2Instances[i].id = placement.id;
					M2Instances[i].dir = vector3df(placement.dir.Z -90, placement.dir.Y + 90, -placement.dir.X);
					M2Instances[i].scale = (f32)placement.scale / 1024.0f;
					M2Instances[i].position = vector3df(-placement.pos.X, placement.pos.Y, placement.pos.Z);
					//ASSERT(placement.unknown == 0);
				}
			}
		}
		else if (strcmp(fourcc, "MODF") == 0)		//Placement information for WMOs
		{
			NumWmoInstance = size / sizeof(ADT::SWMOPlacement);
			ADT::SWMOPlacement placement;
			if (NumWmoInstance)
			{
				WmoInstances = new SWmoInstance[NumWmoInstance];		
				for (u32 i=0; i<NumWmoInstance; ++i)
				{	
					file->read(&placement, sizeof(ADT::SWMOPlacement));
					WmoInstances[i].wmoIndex = placement.wmoIndex;
					WmoInstances[i].id = placement.id;
					WmoInstances[i].box.MinEdge = placement.pos2;
					WmoInstances[i].box.MaxEdge = placement.pos3;
					WmoInstances[i].pos = vector3df(-placement.pos.X, placement.pos.Y, placement.pos.Z);
					vector3df dir = vector3df(-placement.dir.X, placement.dir.Y - 90, 0);
					WmoInstances[i].dir = dir;
					WmoInstances[i].scale = 1.0f;
				}
			}
		}
		else if (strcmp(fourcc, "MCNK") == 0)
		{
			u32 lastpos = nextpos = file->getPos() + size;

			while( file->getPos() < lastpos )
			{
				c8 lfourcc[5];
				u32 lsize;

				file->read(lfourcc, 4);
				file->read(&lsize, 4);

				flipcc(lfourcc);
				lfourcc[4] = 0;

				if (lsize == 0)
					continue;

				u32 lnextpos = file->getPos() + lsize;

				if (strcmp(lfourcc, "MCRD") == 0)
				{
				}
				else if (strcmp(lfourcc, "MCRW") == 0)
				{
				}
				else
				{
					ASSERT(false);
				}

				file->seek((s32)lnextpos);
			}

			++nChunks;
		}
		else if (strcmp(fourcc, "MCLQ") == 0)			
		{
			//not used
		}
		else 
		{
			ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	delete file;
}

bool CFileADT::loadObj0Simple()
{
	c8 name[QMAX_PATH];
	Q_strcpy(name, QMAX_PATH, Name);
	Q_strcat(name, QMAX_PATH, "_obj0.adt");

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return false;

	u32 nChunks = 0;

	c8 fourcc[5];
	u32 size;

	while( !file->isEof() )
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
			ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MMDX") == 0)		//List of filenames for M2 models that appear in this map tile. 
		{
			ASSERT(M2FileNameBlock == 0);
			M2FileNameBlock = new c8[size];
			file->read(M2FileNameBlock, size);
		}
		else if (strcmp(fourcc, "MMID") == 0)		//List of offsets of model filenames in the MMDX chunk. 
		{
			NumM2FileNames = size / sizeof(u32);
			ASSERT(M2FileNameIndices == 0);
			M2FileNameIndices = new u32[NumM2FileNames];
			file->read(M2FileNameIndices, NumM2FileNames * sizeof(u32));
		}
		else if (strcmp(fourcc, "MWMO") == 0)	//List of filenames for WMOs (world map objects) that appear in this map tile. 
		{
			ASSERT(WmoFileNameBlock == 0);
			WmoFileNameBlock = new c8[size];
			file->read(WmoFileNameBlock, size);
		}
		else if (strcmp(fourcc, "MWID") == 0)		//List of offsets of WMO filenames in the MWMO chunk.
		{
			NumWmoFileNames = size / sizeof(u32);
			ASSERT(WmoFileNameIndices == 0);
			WmoFileNameIndices = new u32[NumWmoFileNames];
			file->read(WmoFileNameIndices, NumWmoFileNames * sizeof(u32));
		}
		else if (strcmp(fourcc, "MDDF") == 0)		//Placement information for doodads (M2 models). 
		{
			
		}
		else if (strcmp(fourcc, "MODF") == 0)		//Placement information for WMOs
		{
			
		}
		else if (strcmp(fourcc, "MCNK") == 0)
		{
			++nChunks;
		}
		else if (strcmp(fourcc, "MCLQ") == 0)			
		{
			//not used
		}
		else 
		{
			ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	delete file;

	return true;
}

bool CFileADT::loadFileTextures(IMemFile* file)
{
	const c8* name = file->getFileName();
	getFullFileNameNoExtensionA(name, Name, QMAX_PATH);

	return loadTex(0);
}

bool CFileADT::loadTex( u32 n )
{
	c8 name[QMAX_PATH];
	Q_sprintf(name, QMAX_PATH, "%s_tex%d.adt", Name, (s32)n);

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(name);
	if (!file)
		return false;

	u32 nChunks = 0;

	c8 fourcc[5];
	u32 size;

	while( !file->isEof() )
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
			ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MAMP") == 0)
		{

		}
		else if (strcmp(fourcc, "MTEX") == 0)
		{
			c8* buffer = (c8*)Z_AllocateTempMemory(size+1);
			file->read(buffer, size);
			buffer[size] = '\0';

			c8* p = buffer;
			while(p < buffer + size)
			{
				const c8* str =  p;
				c8 path[256];
				Q_strcpy(path, 256, str);
				s32 idx = (s32)strlen(path) - 4;
				ASSERT(idx > 0);
				path[idx] = '\0';
				Q_strcat(path, 256, "_s.blp");
				bool specular = g_Engine->getWowEnvironment()->exists(path);
				if(!specular)
					Q_strcpy(path, 256, str);

				ITexture* tex = g_Engine->getResourceLoader()->loadTexture(path);
				Textures.push_back(STex(tex, specular));
				p += strlen(p) + 1;
			}

			Z_FreeTempMemory(buffer);
		}
		else if (strcmp(fourcc, "MCNK") == 0)
		{
			CMapChunk& currentChunk = Chunks[nChunks/16][nChunks%16];

			u32 lastpos = nextpos = file->getPos() + size;

			while( file->getPos() < lastpos )
			{
				c8 mk_fourcc[5];
				u32 mk_size;

				file->read(mk_fourcc, 4);
				file->read(&mk_size, 4);

				flipcc(mk_fourcc);
				mk_fourcc[4] = 0;

				if (mk_size == 0)
					continue;

				u32 lnextpos = file->getPos() + mk_size;

				if (strcmp(mk_fourcc, "MCLY") == 0)					//texture layer
				{
					currentChunk.numTextures = mk_size / sizeof(ADT::MCLY);
					ASSERT(currentChunk.numTextures <= 4);
					for (u32 i=0; i<currentChunk.numTextures; ++i)
					{
						file->read(&currentChunk.mclys[i], sizeof(ADT::MCLY));
						u32 idx = currentChunk.mclys[i].textureId;
						ASSERT(idx < Textures.size());
						currentChunk.textures[i] = Textures[idx].texture;
					}
				}
				else if (strcmp(mk_fourcc, "MCSH") == 0)				//shadow map
				{
					//tex1的size为 2 * 64
					ASSERT(mk_size == 8 * 64);
					
					u8* sbuf = (u8*)Z_AllocateTempMemory(sizeof(u8) * 64 * 64);
					u8 tmp[8];
					u8* p = sbuf;
					for (u32 i=0; i<64; ++i)
					{
						file->read(tmp, 8);
						for (u8 c=0; c<8; ++c)
							for (u32 k = 0x01; k != 0x100; k<<=1)
								*p++ = (tmp[c] & k) ? 76 : 0;
					}
					
#ifdef FIXPIPELINE
					Q_memcpy(currentChunk.data_shadowmap, sizeof(u8) * 64 * 64, sbuf, sizeof(u8) * 64 * 64);
#endif
					for (u32 k=0; k<64*64; ++k) {
						currentChunk.data_blendmap[k*4 + 3] = sbuf[k];
					}
					Z_FreeTempMemory(sbuf);
				}
				else if (strcmp(mk_fourcc, "MCAL") == 0)				//alpha map
				{
					u8* data = file->getPointer();

					currentChunk.numAlphaMap = 0;
					ASSERT(currentChunk.numTextures <= 4);

					//从第一个纹理开始，检查alphamap
					for (u32 i=1; i<currentChunk.numTextures; ++i)
					{
						if ((currentChunk.mclys[i].flags & MCLY_USE_ALPHAMAP) == 0)
							continue;

						++currentChunk.numAlphaMap;

						u8* amap = (u8*)Z_AllocateTempMemory(sizeof(u8) * 64 * 64);

						u8* abuf = data + currentChunk.mclys[i].offsetInMCAL;
						if (currentChunk.mclys[i].flags & MCLY_ALPHAMAP_COMPRESS)
						{
							u32 offI = 0;
							u32 offO = 0;
							while( offO < 64 * 64)
							{
								bool fill = (abuf[offI] & 0x80) > 0;
								u32 num = abuf[offI] & 0x7f;
								++offI;
								for (u32 k=0; k<num; ++k)
								{
									if (offO >= 64 *64)
										break;

									amap[offO] = abuf[offI];

									++offO;
									if (!fill)
										++offI;
								}
								if (fill)
									++offI;
							}
						}
						else			//no compress
						{
							unsigned char *p;
							p = amap;
							for (int k=0; k<64; ++k) {
								for (int j=0; j<64; ++j) {
									p[k * 64 + j] = abuf[k * 64 +j] ;
								}
							}
						}

#ifdef FIXPIPELINE
						Q_memcpy(currentChunk.data_alphamap[i-1], sizeof(u8) * 64 * 64, amap, sizeof(u8) * 64 * 64);
#endif
						for (u32 k=0; k<64*64; ++k) {
							currentChunk.data_blendmap[k*4+i-1] = amap[k];
						}
						Z_FreeTempMemory(amap);

					}	//for
				}
				else if (strcmp(mk_fourcc, "MCMT") == 0)
				{

				}
				else
				{
					ASSERT(false);
				}

				file->seek((s32)lnextpos);
			}

			++nChunks;
		}
		else if (strcmp(fourcc, "MCAL") == 0)
		{

		}
		else if (strcmp(fourcc, "MXTF") == 0)
		{
		}
		else if (strcmp(fourcc, "MTXP") == 0)
		{
		}
		else 
		{
			//MessageBoxA(NULL_PTR, fourcc, "", 0);
			ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	delete file;

	ASSERT(nChunks == 256);

	return true;
}

void CFileADT::buildTexcoords()
{
	//消除接缝问题
	const float fixstep = 1.0f/16.0f;
	const float step = 0.96f/16.0f;
	const float delta = 0.02f/16.0f;

	for (u8 row=0; row<16; ++row)
	{
		for (u8 col=0; col<16; ++col)
		{
			const float minX = col * fixstep + delta;
			const float minY = row * fixstep + delta; 

			u32 offset = getChunkVerticesOffset(row, col);
			SVertex_PNCT2* v = &Vertices[offset];

			//detail, alpha
			const float detail_half = 0.5f;
			const float alpha_half = step / 16.0f;
			f32 tx,ty, tx2, ty2;
			for (int j=0; j<17; j++) {
				for (int i=0; i<((j%2)?8:9); i++) {
					tx = 1.0f * i;
					ty = 0.5f * j;
					tx2 = minX + step / 8.0f * i;
					ty2 =  minY + step / 16.0f * j;
					if (j%2) {
						// offset by half
						tx += detail_half;
						tx2 += alpha_half;
					}
					v->TCoords0.set(tx, ty);
					v->TCoords1.set(tx2, ty2);

					++v;
				}
			}
		}
	}
}

const c8* CFileADT::getM2FileName( u32 index ) const
{
	if(index >= NumM2FileNames)
		return "";
	return (const c8*)&M2FileNameBlock[M2FileNameIndices[index]];
}

const c8* CFileADT::getWMOFileName( u32 index ) const
{
	if (index >= NumWmoFileNames)
		return "";
	return (const c8*)&WmoFileNameBlock[WmoFileNameIndices[index]];
}

const c8* CFileADT::getTextureName(u32 index) const
{
	if (index >= (u32)Textures.size())
		return "";
	ITexture* tex = Textures[index].texture;

	return tex ? tex->getFileName() : "";
}

#ifdef FIXPIPELINE
void CFileADT::buildMaps()
{
	//process shadow + alpha map, delete blend map
	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{	
			for (u32 n=0; n<64*64; ++n)
			{
				u8 v = Chunks[i][j].data_shadowmap[n];
				Chunks[i][j].data_shadowmap[n] = (0x00 | v<<8);
			}

			for (u32 k=0; k<3; ++k)
			{
				if (Chunks[i][j].data_alphamap[k])
				{
					for (u32 n=0; n<64*64; ++n)
					{
						u8 v = Chunks[i][j].data_alphamap[k][n];
						Chunks[i][j].data_alphamap[k][n] = (0xff | v << 8);
					}
				}
			}
		}
	}
}

bool CFileADT::buildVideoResources()
{
	CMutex lock(g_Globals.adtCS);

	if (VideoBuilt)
		return true;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture)
			Textures[i].texture->createVideoTexture();
	}

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(VertexBuffer);

	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{
			//shadow map
			if (Chunks[i][j].data_shadowmap)
			{
				ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(64,64), ECF_A8L8, Chunks[i][j].data_shadowmap, true);

				Chunks[i][j].shadowmap = tex;
			}

			//alpha map
			for (u32 k=0; k<3; ++k)
			{
				if (Chunks[i][j].data_alphamap[k])
				{
					ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(64,64), ECF_A8L8, Chunks[i][j].data_alphamap[k], true);

					Chunks[i][j].alphamap[k] = tex;
				}
			}
		}
	}
	
	VideoBuilt = true;

	return true;
}

void CFileADT::releaseVideoResources()
{
	CMutex lock(g_Globals.adtCS);

	if (!VideoBuilt)
		return;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture && Textures[i].texture->getReferenceCount() == 2)
			Textures[i].texture->releaseVideoTexture();
	}

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(VertexBuffer);

	for (u32 i=0; i<16; ++i)
	{
		for (u32 j=0; j<16; ++j)
		{
			for (u32 k=0; k<3; ++k)
			{
				if (Chunks[i][j].alphamap[k])
				{
					Chunks[i][j].alphamap[k]->drop();
					Chunks[i][j].alphamap[k] = NULL_PTR;
				}
			}

			if(Chunks[i][j].shadowmap)
			{
				Chunks[i][j].shadowmap->drop();
				Chunks[i][j].shadowmap = NULL_PTR;
			}
		}
	}

	VideoBuilt = false;
}

#else
void CFileADT::buildMaps()
{
	Data_BlendMap =  new u32[BLENDMAP_SIZE * BLENDMAP_SIZE];
	//
	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{
			u32* dst = Data_BlendMap + i * 64 * BLENDMAP_SIZE + j * 64;
			u32* src = (u32*)Chunks[i][j].data_blendmap;
			for (u8 k=0; k<64; ++k)
			{
				Q_memcpy(dst + k*BLENDMAP_SIZE, sizeof(u32) * 64,  src + k*64, 4*64);
			}
		}
	}
}

bool CFileADT::buildVideoResources()
{
	//CLock lock(&g_Globals.adtCS);

	if (VideoBuilt)
		return true;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture)
			Textures[i].texture->createVideoTexture();
	}

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(VertexBuffer);

	ASSERT(!BlendMap);

	ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(BLENDMAP_SIZE, BLENDMAP_SIZE), ECF_A8R8G8B8, Data_BlendMap, true);
	BlendMap = tex;

	VideoBuilt = true;

	return true;
}

void CFileADT::releaseVideoResources()
{
	//CLock lock(&g_Globals.adtCS);

	if (!VideoBuilt)
		return;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture && Textures[i].texture->getReferenceCount() == 2)
			Textures[i].texture->releaseVideoTexture();
	}

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(VertexBuffer);

	if (BlendMap)
	{
		BlendMap->drop();
		BlendMap = NULL_PTR;
	}

	VideoBuilt = false;
}

#endif


