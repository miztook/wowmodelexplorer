#include "stdafx.h"
#include "CFileADT.h"
#include "mpq_libmpq.h"
#include "mywow.h"
#include "CLock.h"

#define	BIG_OBJECT_VOLUME		144		//体积于此的认为是大物体

#define  MIN_ALPHASIZE		0.0f
#define  MAX_ALPHASIZE		 1.0f

extern CRITICAL_SECTION g_adtCS;

IFileADT* CADTLoader::loadADT( MPQFile* file )
{
	CFileADT* adtFile = new CFileADT;
	if (!adtFile->loadFile(file))
	{
		adtFile->drop();
		return NULL;
	}
	return adtFile;
}

CFileADT::CFileADT()
{
	Box.set(vector3df(99999.9f), vector3df(-99999.9f));

	GVertices = new SGVertex_PNC[16 * 16 * 145];
	TVertices = new STVertex_2T[16 * 16 * 145];

	GVertexBuffer = new IVertexBuffer;
	GVertexBuffer->set(GVertices, EST_G_PNC, 16 * 16 * 145, EMM_STATIC);
	TVertexBuffer = new IVertexBuffer;
	TVertexBuffer->set(TVertices, EST_T_2T, 16 * 16 * 145, EMM_STATIC);
}

CFileADT::~CFileADT()
{
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

	for (u32 i=0; i<16; ++i)
	{
		for (u32 j=0; j<16; ++j)
		{
			delete[] Chunks[i][j].sounds;
			delete[] Chunks[i][j].data_shadowmap;
			for (u32 k=0; k<3; ++k)
			{
				delete[] Chunks[i][j].data_alphamap[k];
			}
			delete[] Chunks[i][j].data_blendmap;
		}
	}

	delete TVertexBuffer;
	delete GVertexBuffer;
}

bool CFileADT::loadFile( MPQFile* file )
{
	const c8* name = file->getFileName();
	getFileNameNoExtensionA(name, Name, MAX_PATH);

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
			_ASSERT(version == 18);
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
			_ASSERT(size == 36);
		}
		else
		{
			_ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	_ASSERT(nChunks == 256);

	//objects
	loadObj0();

	//textures
	loadTex(0);

	return true;
}

void CFileADT::readChunk( MPQFile* file, u8 row, u8 col, u32 lastpos)
{
	SMapChunk& currentChunk = Chunks[row][col];

	u32 offset = getChunkVerticesOffset(row, col);
	SGVertex_PNC* gVertices = &GVertices[offset];
	STVertex_2T* tVertices = &TVertices[offset];

	ADT::chunkHeader header;
	file->read(&header, sizeof(ADT::chunkHeader));

	currentChunk.areaID = header.areaid;
	currentChunk.zbase = -header.zpos + ZEROPOINT;
	currentChunk.ybase = header.ypos - 0.5f;
	currentChunk.xbase = -header.xpos + ZEROPOINT;
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
			_ASSERT(size == 145 * sizeof(f32));
			u32 count = 0;
			for (u32 j=0; j<17; ++j)
			{
				for (s32 i=0; i<((j%2)?8:9); ++i)
				{
					f32 h, xpos, zpos;
					file->read(&h, 4);
					xpos = i * UNITSIZE;
					zpos = j * 0.5f * UNITSIZE;
					if (j%2)
					{
						xpos += UNITSIZE * 0.5f;
					}

					vector3df v(currentChunk.xbase + xpos,
						currentChunk.ybase + h, currentChunk.zbase + zpos);
					gVertices[count].Pos = v;
					++count;
					if (v.Y < currentChunk.box.MinEdge.Y)
						currentChunk.box.MinEdge.Y = v.Y;
					if (v.Y > currentChunk.box.MaxEdge.Y)
						currentChunk.box.MaxEdge.Y = v.Y;

					currentChunk.box.MinEdge.X = currentChunk.xbase;
					currentChunk.box.MinEdge.Z = currentChunk.zbase;
					currentChunk.box.MaxEdge.X = currentChunk.xbase + 8 * UNITSIZE;
					currentChunk.box.MaxEdge.Z = currentChunk.zbase + 8 * UNITSIZE;
				}
			}

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
					gVertices[count].Normal = n;
				}
			}
		}
		else if (strcmp(fourcc, "MCCV") == 0)
		{
			_ASSERT( size == 145 * 4);

			u32 count = 0;
			u8 clr[4];
			for (u32 j=0; j<17; ++j)
			{
				for (s32 i=0; i<((j%2)?8:9); ++i)
				{
					file->read(clr, 4);
					SColor color(clr[3], clr[0], clr[1], clr[2]);
					gVertices[count].Color = color;
					++count;
				}
			}
		}
		else if (strcmp(fourcc, "MCLV") == 0)
		{
			_ASSERT( size == 145 * 4);

			/*
			It looks like an array of Colors, one per heightmap vertex (i.e. a sequence of 145 x unsigned int). 
			Most of them are 0xFF000000, but some differ - e.g. 0xFF060102. 
			In almost all cases RGB components are very low (0x00 - 0x10).
			*/
		}
		else if (strcmp(fourcc, "MCSE") == 0)
		{
			_ASSERT(size % 28 == 0);

			u32 nSounds = size / sizeof(ADT::SSoundEmitter);
			if (nSounds)
			{
				_ASSERT(currentChunk.sounds == NULL);
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
		else
		{
			_ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	//tex coords
	buildTexcoords(tVertices);
}

const SMapChunk* CFileADT::getChunk( u8 row, u8 col ) const
{
	if (row >= 16 || col >= 16)
		return NULL;

	return &Chunks[row][col];
}

void CFileADT::loadObj0()
{
	c8 name[MAX_PATH];
	strcpy_s(name, MAX_PATH, Name);
	strcat_s(name, MAX_PATH, "_obj0.adt");

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(name);
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
			_ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MMDX") == 0)		//List of filenames for M2 models that appear in this map tile. 
		{
			_ASSERT(M2FileNameBlock == 0);
			M2FileNameBlock = new c8[size];
			file->read(M2FileNameBlock, size);
		}
		else if (strcmp(fourcc, "MMID") == 0)		//List of offsets of model filenames in the MMDX chunk. 
		{
			NumM2FileNames = size / sizeof(u32);
			_ASSERT(M2FileNameIndices == 0);
			M2FileNameIndices = new u32[NumM2FileNames];
			file->read(M2FileNameIndices, NumM2FileNames * sizeof(u32));
		}
		else if (strcmp(fourcc, "MWMO") == 0)	//List of filenames for WMOs (world map objects) that appear in this map tile. 
		{
			_ASSERT(WmoFileNameBlock == 0);
			WmoFileNameBlock = new c8[size];
			file->read(WmoFileNameBlock, size);
		}
		else if (strcmp(fourcc, "MWID") == 0)		//List of offsets of WMO filenames in the MWMO chunk.
		{
			NumWmoFileNames = size / sizeof(u32);
			_ASSERT(WmoFileNameIndices == 0);
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
					vector3df position = placement.pos;
					vector3df dir = vector3df(-placement.dir.X, placement.dir.Y - 90, 0);
					f32 scale = (f32)placement.scale / 1024.0f;
					M2Instances[i].dir = dir;
					M2Instances[i].scale = scale;
					M2Instances[i].position = position;
					_ASSERT(placement.unknown == 0);
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
					WmoInstances[i].pos = placement.pos;
					vector3df dir = vector3df(-placement.dir.X, placement.dir.Y - 90, 0);
					WmoInstances[i].dir = dir;
					WmoInstances[i].scale = 1.0f;
				}
			}
		}
		else if (strcmp(fourcc, "MCNK") == 0)
		{
			SMapChunk& currentChunk = Chunks[nChunks/16][nChunks%16];

			u32 lastpos = nextpos = file->getPos() + size;

			while( file->getPos() < lastpos )
			{
				c8 fourcc[5];
				u32 size;

				file->read(fourcc, 4);
				file->read(&size, 4);

				flipcc(fourcc);
				fourcc[4] = 0;

				if (size == 0)
					continue;

				u32 nextpos = file->getPos() + size;

				if (strcmp(fourcc, "MCRD") == 0)
				{
				}
				else if (strcmp(fourcc, "MCRW") == 0)
				{
				}
				else
				{
					_ASSERT(false);
				}

				file->seek((s32)nextpos);
			}

			++nChunks;
		}
		else if (strcmp(fourcc, "MCLQ") == 0)			
		{
			//not used
		}
		else 
		{
			_ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	delete file;
}

bool CFileADT::loadTex( u32 n )
{
	c8 name[MAX_PATH];
	sprintf_s(name, MAX_PATH, "%s_tex%d.adt", Name, n);

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(name);
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
			_ASSERT(version == 18);
		}
		else if (strcmp(fourcc, "MAMP") == 0)
		{

		}
		else if (strcmp(fourcc, "MTEX") == 0)
		{
			c8* buffer = (c8*)Hunk_AllocateTempMemory(size+1);
			file->read(buffer, size);
			buffer[size] = '\0';

			c8* p = buffer;
			while(p < buffer + size)
			{
				const c8* str =  p;
				c8 path[256];
				strcpy_s(path, 256, str);
				s32 idx = strlen(path) - 4;
				_ASSERT(idx > 0);
				path[idx] = '\0';
				strcat_s(path, 256, "_s.blp");
				bool specular = g_Engine->getWowEnvironment()->exists(path);
				if(!specular)
					strcpy_s(path, 256, str);

				ITexture* tex = g_Engine->getResourceLoader()->loadTexture(path);
				Textures.push_back(STex(tex, specular));
				p += strlen(p) + 1;
			}

			Hunk_FreeTempMemory(buffer);
		}
		else if (strcmp(fourcc, "MCNK") == 0)
		{
			SMapChunk& currentChunk = Chunks[nChunks/16][nChunks%16];

			u32 lastpos = nextpos = file->getPos() + size;

			while( file->getPos() < lastpos )
			{
				c8 fourcc[5];
				u32 size;

				file->read(fourcc, 4);
				file->read(&size, 4);

				flipcc(fourcc);
				fourcc[4] = 0;

				if (size == 0)
					continue;

				u32 nextpos = file->getPos() + size;

				if (strcmp(fourcc, "MCLY") == 0)					//texture layer
				{
					currentChunk.numTextures = size / sizeof(ADT::MCLY);
					_ASSERT(currentChunk.numTextures <= 4);
					for (u32 i=0; i<currentChunk.numTextures; ++i)
					{
						file->read(&currentChunk.mclys[i], sizeof(ADT::MCLY));
						u32 idx = currentChunk.mclys[i].textureId;
						_ASSERT(idx < Textures.size());
						currentChunk.textures[i] = Textures[idx].texture;
					}
				}
				else if (strcmp(fourcc, "MCSH") == 0)				//shadow map
				{
					//tex1的size为 2 * 64
					_ASSERT(size == 8 * 64);
					
					currentChunk.data_shadowmap = new u8[64 * 64];
					u8* sbuf = currentChunk.data_shadowmap;
					u8 tmp[8];
					u8* p = sbuf;
					for (u32 i=0; i<64; ++i)
					{
						file->read(tmp, 8);
						for (u8 c=0; c<8; ++c)
							for (u32 k = 0x01; k != 0x100; k<<=1)
								*p++ = (tmp[c] & k) ? 76 : 0;
					}

					for (u32 k=0; k<64*64; ++k) {
						currentChunk.data_blendmap[k*4 + 3] = sbuf[k];
					}
				}
				else if (strcmp(fourcc, "MCAL") == 0)				//alpha map
				{
					u8* data = file->getPointer();

					currentChunk.numAlphaMap = 0;
					_ASSERT(currentChunk.numTextures <= 4);

					//从第一个纹理开始，检查alphamap
					for (u32 i=1; i<currentChunk.numTextures; ++i)
					{
						if ((currentChunk.mclys[i].flags & MCLY_USE_ALPHAMAP) == 0)
							continue;

						++currentChunk.numAlphaMap;

						currentChunk.data_alphamap[i-1] = new u8[64 * 64];
						u8* amap = currentChunk.data_alphamap[i-1];

						u8* abuf = data + currentChunk.mclys[i].offsetInMCAL;
						if (currentChunk.mclys[i].flags & MCLY_ALPHAMAP_COMPRESS)
						{
							u32 offI = 0;
							u32 offO = 0;
							while( offO < 64 * 64)
							{
								bool fill = (abuf[offI] & 0x80) > 0;
								u32 n = abuf[offI] & 0x7f;
								++offI;
								for (u32 k=0; k<n; ++k)
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
							for (int i=0; i<64; ++i) {
								for (int j=0; j<64; ++j) {
									p[i * 64 + j] = abuf[i * 64 +j] ;
								}
							}
						}


						for (u32 k=0; k<64*64; ++k) {
							currentChunk.data_blendmap[k*4+i-1] = amap[k];
						}

					}	//for
				}
				else if (strcmp(fourcc, "MCMT") == 0)
				{

				}
				else
				{
					_ASSERT(false);
				}

				file->seek((s32)nextpos);
			}

			++nChunks;
		}
		else if (strcmp(fourcc, "MCAL") == 0)
		{

		}
		else if (strcmp(fourcc, "MXTF") == 0)
		{
		}
		else 
		{
			_ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	delete file;

	_ASSERT(nChunks == 256);

	return true;
}

void CFileADT::buildTexcoords(STVertex_2T* tVertices)
{
	//145 float
	STVertex_2T* v = tVertices;

	//detail, alpha
	const float detail_half = 0.5f;
	const float alpha_half = (MAX_ALPHASIZE - MIN_ALPHASIZE) / 16.0f;
	f32 tx,ty, tx2, ty2;
	for (int j=0; j<17; j++) {
		for (int i=0; i<((j%2)?8:9); i++) {
			tx = 1.0f * i;
			ty = 0.5f * j;
			tx2 = MIN_ALPHASIZE + (MAX_ALPHASIZE - MIN_ALPHASIZE) / 8.0f * i;
			ty2 =  MIN_ALPHASIZE + (MAX_ALPHASIZE - MIN_ALPHASIZE) / 16.0f * j;
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
	_ASSERT(v - tVertices == 145);

}

const c8* CFileADT::getM2FileName( u32 index ) const
{
	if(index >= NumM2FileNames)
		return NULL;
	return (const c8*)&M2FileNameBlock[M2FileNameIndices[index]];
}

const c8* CFileADT::getWMOFileName( u32 index ) const
{
	if (index >= NumWmoFileNames)
		return NULL;
	return (const c8*)&WmoFileNameBlock[WmoFileNameIndices[index]];
}

bool CFileADT::buildVideoResources()
{
	CLock lock(&g_adtCS);

	if (VideoBuilt)
		return true;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture)
			Textures[i].texture->createVideoTexture();
	}

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(GVertexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(GVertexBuffer, 0, GVertexBuffer->Size);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(TVertexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(TVertexBuffer, 0, TVertexBuffer->Size);

	bool ffp = g_Engine->getDriver()->getMaterialRenderServices()->isFFPipeline();

	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{
			if (ffp)			
			{
				//shadow map
				if (Chunks[i][j].data_shadowmap)
				{
					u16* data = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * 64 * 64);
					for (u32 n=0; n<64*64; ++n)
					{
						u8 v = Chunks[i][j].data_shadowmap[n];
						data[n] = (0x00 | v<<8);
					}

					ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(64,64), ECF_A8L8, data, true);
					Hunk_FreeTempMemory(data);

					Chunks[i][j].shadowmap = tex;
				}

				//alpha map
				for (u32 k=0; k<3; ++k)
				{
					if (Chunks[i][j].data_alphamap[k])
					{
						u16* data = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * 64 * 64);
						for (u32 n=0; n<64*64; ++n)
						{
							u8 v = Chunks[i][j].data_alphamap[k][n];
							data[n] = (0xff | v << 8);
						}
						ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(64,64), ECF_A8L8, data, true);
						Hunk_FreeTempMemory(data);

						Chunks[i][j].alphamap[k] = tex;
					}
				}
			}
			else
			{
				//blendmap
				u32* data = (u32*)Hunk_AllocateTempMemory(sizeof(u32) * 64 * 64);
				memcpy_s(data, sizeof(u32)*64*64, Chunks[i][j].data_blendmap, 4*64*64);
				ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromData(dimension2du(64,64), ECF_A8R8G8B8, data, true);
				Hunk_FreeTempMemory(data);

				Chunks[i][j].blendmap = tex;
			}
			
		}
	}

	VideoBuilt = true;
	return true;
}

void CFileADT::releaseVideoResources()
{
	CLock lock(&g_adtCS);

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if(Textures[i].texture && Textures[i].texture->getReferenceCount() == 2)
			Textures[i].texture->releaseVideoTexture();
	}

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(TVertexBuffer);
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(GVertexBuffer);

	bool ffp = g_Engine->getDriver()->getMaterialRenderServices()->isFFPipeline();

	for (u32 i=0; i<16; ++i)
	{
		for (u32 j=0; j<16; ++j)
		{
			if (ffp)
			{
				for (u32 k=0; k<3; ++k)
				{
					if (Chunks[i][j].alphamap[k])
					{
						delete Chunks[i][j].alphamap[k];
						Chunks[i][j].alphamap[k] = NULL;
					}
				}

				if(Chunks[i][j].shadowmap)
				{
					delete Chunks[i][j].shadowmap;
					Chunks[i][j].shadowmap = NULL;
				}
			}
			else
			{
				if (Chunks[i][j].blendmap)
				{
					delete Chunks[i][j].blendmap;
					Chunks[i][j].blendmap = NULL;
				}
			}
		}
	}

	VideoBuilt = false;
}

