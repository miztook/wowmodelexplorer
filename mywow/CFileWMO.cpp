#include "stdafx.h"
#include "CFileWMO.h"
#include "mpq_libmpq.h"
#include "mywow.h"
#include "CLock.h"

extern CRITICAL_SECTION g_wmoCS;

IFileWMO* CWMOLoader::loadWMO( MPQFile* file )
{
	CFileWMO* wmoFile = new CFileWMO;
	if (!wmoFile->loadFile(file))
	{
		wmoFile->drop();
		return NULL;
	}
	return wmoFile;
}

CFileWMO::CFileWMO()
{
	FileData = NULL;
}

CFileWMO::~CFileWMO()
{
	clear();
}

void CFileWMO::clear()
{
	delete[] Fogs;

	delete[] Doodads;
	delete[] ModelNamesBlock;
	delete[] DoodadSets;

	delete[] Lights;

	delete[] Portals;
	delete[] PortalVertices;

	for (u32 i=0; i<Header.nGroups; ++i)
	{
		delete[] Groups[i].Doodads;
		delete[] Groups[i].Lights;
		delete[]	Groups[i].Batches;

		delete Groups[i].IndexBuffer;
		delete Groups[i].TVertexBuffer;
		delete Groups[i].GVertexBuffer;
	}
	delete[] Groups;

	for (u32 i=0; i<Header.nMaterials; ++i)
	{
		if (Materials[i].texture0)
			Materials[i].texture0->drop();
		if (Materials[i].texture1)
			Materials[i].texture1->drop();
	}
	delete[] Materials;

	delete[] GroupNamesBlock;
	delete[] TextureFileNameBlock;
}

bool CFileWMO::loadFile( MPQFile* file )
{
	const c8* name = file->getFileName();
	getFileNameNoExtensionA(name, Name, MAX_PATH);

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

		if (strcmp(fourcc, "MVER") == 0)				//version
		{
			u32 version;
			file->read(&version, size);
			_ASSERT(version == 17);
		}
		else if (strcmp(fourcc, "MOHD") == 0)				//header
		{
			_ASSERT(size == sizeof(Header));
			file->read(&Header, sizeof(Header));

			Materials = new SWMOMaterial[Header.nMaterials];
			Groups = new SWMOGroup[Header.nGroups];
			Portals = new SWMOPortal[Header.nPortals];
			Lights = new SWMOLight[Header.nLights];
			DoodadSets = new SWMODoodadSet[Header.nDoodadSets];
		}
		else if (strcmp(fourcc, "MOTX") == 0)					//texture
		{
			TextureFileNameBlock = new c8[size];
			file->read(TextureFileNameBlock, size);
		}
		else if (strcmp(fourcc, "MOMT") == 0)				//material
		{
			_ASSERT(size == Header.nMaterials * sizeof(WMO::wmoMaterial));
			WMO::wmoMaterial* materials = (WMO::wmoMaterial*)Hunk_AllocateTempMemory(Header.nMaterials * sizeof(WMO::wmoMaterial));
			file->read(materials, Header.nMaterials * sizeof(WMO::wmoMaterial));
			for (u32 i=0; i<Header.nMaterials; ++i)
			{
				const WMO::wmoMaterial& m = materials[i];

				Materials[i].flags = m.flags;
				Materials[i].alphatest = m.alphatest != 0;

				Materials[i].color0.set(m.col1A, m.col1R, m.col1G, m.col1B);
				const c8* tex1 = &TextureFileNameBlock[m.tex1];
				Materials[i].texture0= g_Engine->getResourceLoader()->loadTexture(tex1);

				Materials[i].color1.set(m.col2A, m.col2R, m.col2G, m.col2B);
				const c8* tex2 = &TextureFileNameBlock[m.tex2];
				Materials[i].texture1 = g_Engine->getResourceLoader()->loadTexture(tex2);
			}
			Hunk_FreeTempMemory(materials);
		}
		else if (strcmp(fourcc, "MOGN") == 0)					//group name
		{
			GroupNamesBlock = new c8[size];
			file->read(GroupNamesBlock, size);
		}
		else if (strcmp(fourcc, "MOGI") == 0)					//group
		{
			_ASSERT(size == Header.nGroups * sizeof(WMO::wmoGroupInfo));
			WMO::wmoGroupInfo* groupInfos = (WMO::wmoGroupInfo*)Hunk_AllocateTempMemory(Header.nGroups * sizeof(WMO::wmoGroupInfo));
			file->read(groupInfos, Header.nGroups * sizeof(WMO::wmoGroupInfo));
			for (u32 i=0; i<Header.nGroups; ++i)
			{
				const WMO::wmoGroupInfo& gi = groupInfos[i];
				Groups[i].flags = gi.flags;
				Groups[i].box.set(fixCoordinate(gi.min), fixCoordinate(gi.max));
				Groups[i].name = &GroupNamesBlock[gi.nameIndex];
			}

			Hunk_FreeTempMemory(groupInfos);
		}
		else if (strcmp(fourcc, "MOSB") == 0)			//skybox
		{

		}
		else if (strcmp(fourcc, "MOPV") == 0)			//portal vertices
		{
			_ASSERT(PortalVertices == NULL);
			NumPortalVertices = size / sizeof(vector3df);
			vector3df v;
			if (NumPortalVertices)
			{
				PortalVertices = new vector3df[NumPortalVertices];
				for (u32 i=0; i<NumPortalVertices; ++i)
				{
					file->read(&v, sizeof(vector3df));
					PortalVertices[i] = fixCoordinate(v);
				}
			}
		}
		else if (strcmp(fourcc, "MOPT") == 0)					//portal info
		{
			_ASSERT(size == Header.nPortals * sizeof(WMO::wmoPortalInfo));
			
			WMO::wmoPortalInfo* portalInfos = (WMO::wmoPortalInfo*)Hunk_AllocateTempMemory(Header.nPortals * sizeof(WMO::wmoPortalInfo));
			file->read(portalInfos, Header.nPortals * sizeof(WMO::wmoPortalInfo));

			for (u32 i=0; i<Header.nPortals; ++i)
			{
				const WMO::wmoPortalInfo& pi = portalInfos[i];

				Portals[i].vStart = pi.baseIndex;
				Portals[i].vCount = pi.numVerts;
			}

			Hunk_FreeTempMemory(portalInfos);
		}
		else if (strcmp(fourcc, "MOPR") == 0)
		{
			
		}
		else if (strcmp(fourcc, "MOLT") == 0)
		{
			_ASSERT(size == Header.nLights * sizeof(WMO::wmoLight));
			WMO::wmoLight* lights = (WMO::wmoLight*)Hunk_AllocateTempMemory(Header.nLights * sizeof(WMO::wmoLight));
			file->read(lights, Header.nLights * sizeof(WMO::wmoLight));

			for (u32 i=0; i<Header.nLights; ++i)
			{
				const WMO::wmoLight& li = lights[i];

				Lights[i].lighttype = (E_LIGHT_TYPE)li.lighttype;
				Lights[i].color.set(li.colA, li.colR, li.colG, li.colB);
				Lights[i].position = fixCoordinate(li.pos);
				Lights[i].intensity = li.intensity;
				Lights[i].attenStart = li.attenStart;
				Lights[i].attenEnd = li.attenEnd;
			}

			Hunk_FreeTempMemory(lights);
		}
		else if (strcmp(fourcc, "MODS") == 0)			//doodad set
		{
			_ASSERT(size == Header.nDoodadSets * sizeof(WMO::wmoDoodadSet));

			WMO::wmoDoodadSet* sets = (WMO::wmoDoodadSet*)Hunk_AllocateTempMemory(Header.nDoodadSets * sizeof(WMO::wmoDoodadSet));
			file->read(sets, Header.nDoodadSets * sizeof(WMO::wmoDoodadSet));

			for (u32 i=0; i<Header.nDoodadSets; ++i)
			{
				const WMO::wmoDoodadSet& s = sets[i];

				strcpy_s(DoodadSets[i].name, 20, s.name);
				DoodadSets[i].start = s.start;
				DoodadSets[i].count = s.count;
			}
			Hunk_FreeTempMemory(sets);
		}
		else if (strcmp(fourcc, "MODN") == 0)				//doodad names
		{
			ModelNamesBlock = new c8[size];
			file->read(ModelNamesBlock, size);
		}
		else if (strcmp(fourcc, "MODD") == 0)				//doodad def
		{
			NumDoodads = size / sizeof(WMO::wmoDoodadDef);
			Doodads = new SWMODoodad[NumDoodads];

			WMO::wmoDoodadDef* dds = (WMO::wmoDoodadDef*)Hunk_AllocateTempMemory(NumDoodads * sizeof(WMO::wmoDoodadDef));
			file->read(dds, NumDoodads * sizeof(WMO::wmoDoodadDef));

			for (u32 i=0; i<NumDoodads; ++i)
			{
				const WMO::wmoDoodadDef& dd = dds[i];
				
				Doodads[i].name = &ModelNamesBlock[dd.nameIndex];
				Doodads[i].position = fixCoordinate(dd.pos);
				Doodads[i].quat.set(dd.rot[0], dd.rot[1], dd.rot[2], dd.rot[3]);
				Doodads[i].scale = dd.scale;
				Doodads[i].color.set(dd.colA, dd.colR, dd.colG, dd.colB);
			}

			Hunk_FreeTempMemory(dds);
		}
		else if (strcmp(fourcc, "MFOG") == 0)
		{
			NumFogs = size / sizeof(WMO::wmoFog);
			Fogs = new SWMOFog[NumFogs];

			WMO::wmoFog* fogs = (WMO::wmoFog*)Hunk_AllocateTempMemory(NumFogs * sizeof(WMO::wmoFog));
			file->read(fogs, NumFogs * sizeof(WMO::wmoFog));

			for (u32 i=0; i<NumFogs; ++i)
			{
				const WMO::wmoFog& f = fogs[i];

				Fogs[i].position = fixCoordinate(f.pos);
				Fogs[i].radius1 = f.r1;
				Fogs[i].radius2 = f.r2;
				Fogs[i].fogstart = f.fogstart;
				Fogs[i].fogend = f.fogend;
				Fogs[i].color.set(f.colA, f.colR, f.colG, f.colB);
			}

			Hunk_FreeTempMemory(fogs);
		}
		else if (strcmp(fourcc, "MOGP") == 0)
		{
		}
		else
		{
			_ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	Box.reset(vector3df(0));
	for (u32 i=0; i<Header.nGroups; ++i)
	{
		Groups[i].loadFile(i, this);
		Box.addInternalBox(Groups[i].box);
	}

	return true;
}

bool CFileWMO::buildVideoResources()
{
	CLock lock(&g_wmoCS);

	if (VideoBuilt)
		return true;

	//texture
	for (u32 i=0; i<Header.nMaterials; ++i)
	{
		if (Materials[i].texture0)
			Materials[i].texture0->createVideoTexture();

		if (Materials[i].texture1)
			Materials[i].texture1->createVideoTexture();
	}

	for (u32 i=0; i<Header.nGroups; ++i)
	{
		if(Groups[i].GVertexBuffer)
		{
			g_Engine->getHardwareBufferServices()->createHardwareBuffer(Groups[i].GVertexBuffer);
			g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Groups[i].GVertexBuffer, 0, Groups[i].VCount);
		}
		
		if(Groups[i].TVertexBuffer)
		{
			g_Engine->getHardwareBufferServices()->createHardwareBuffer(Groups[i].TVertexBuffer);
			g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Groups[i].TVertexBuffer, 0, Groups[i].VCount);
		}

		if (Groups[i].IndexBuffer)
		{
			g_Engine->getHardwareBufferServices()->createHardwareBuffer(Groups[i].IndexBuffer);
			g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Groups[i].IndexBuffer, 0, Groups[i].ICount);
		}
	}

	VideoBuilt = true;
	return true;
}

void CFileWMO::releaseVideoResources()
{
	CLock lock(&g_wmoCS);

	//texture
	for (u32 i=0; i<Header.nMaterials; ++i)
	{
		if (Materials[i].texture0 && Materials[i].texture0->getReferenceCount() == 2)
			Materials[i].texture0->releaseVideoTexture();

		if (Materials[i].texture1 && Materials[i].texture1->getReferenceCount() == 2)
			Materials[i].texture1->releaseVideoTexture();
	}

	for (u32 i=0; i<Header.nGroups; ++i)
	{
		if(Groups[i].GVertexBuffer)
			g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Groups[i].GVertexBuffer);

		if (Groups[i].TVertexBuffer)
			g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Groups[i].TVertexBuffer);

		if (Groups[i].IndexBuffer)
			g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Groups[i].IndexBuffer);
	}

	VideoBuilt = false;
}

bool SWMOGroup::loadFile( u32 index, IFileWMO* wmo )
{
	c8 tmp[MAX_PATH];
	getFileNameNoExtensionA(wmo->Name, tmp, MAX_PATH);

	c8 filename[MAX_PATH];
	sprintf_s(filename, MAX_PATH, "%s_%03d.wmo", tmp, index);

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	_ASSERT(file);
	if (!file)
		return false;

	WMO::wmoGroupHeader header;

	file->seek(0x14);
	file->read(&header, sizeof(WMO::wmoGroupHeader));

	b1 = fixCoordinate(header.minbox);
	b2 = fixCoordinate(header.maxbox);

	file->seek(0x58);

	c8 fourcc[5];
	u32 size;

	u32 nTcoords = 0;

	while( !file->isEof() )
	{
		file->read(fourcc, 4);
		file->read(&size, 4);

		flipcc(fourcc);
		fourcc[4] = 0;

		if (size == 0)
			continue;

		u32 nextpos = file->getPos() + size;

		if (strcmp(fourcc, "MOPY") == 0)				//
		{
// 			NumTriangleMaterials = size / sizeof(WMO::wmoTriangleMaterial);
// 			TriangleMaterials = new u8[NumTriangleMaterials];
// 
// 			WMO::wmoTriangleMaterial* mats = (WMO::wmoTriangleMaterial*)Hunk_AllocateTempMemory(NumTriangleMaterials * sizeof(WMO::wmoTriangleMaterial));
// 			file->read(mats, NumTriangleMaterials * sizeof(WMO::wmoTriangleMaterial));
// 			for (u32 i=0; i<NumTriangleMaterials; ++i)
// 			{
// 				TriangleMaterials[i] = mats[i].matId;
// 			}
// 			Hunk_FreeTempMemory(mats);
		}
		else if (strcmp(fourcc, "MOVI") == 0)
		{
			ICount = size / sizeof(u16);
			Indices = new u16[ICount];
			file->read(Indices, size);
		}
		else if (strcmp(fourcc, "MOVT") == 0)
		{
			VCount = size / sizeof(vector3df);
			GVertices = new SGVertex_PNC[VCount];
			TVertices = new STVertex_2T[VCount];

			vector3df* tmp = (vector3df*)Hunk_AllocateTempMemory(size);
			file->read(tmp, size);
			for (u32 i=0; i<VCount; ++i)
			{
				GVertices[i].Pos = fixCoordinate(tmp[i]);
			}
			Hunk_FreeTempMemory(tmp);
		}
		else if (strcmp(fourcc, "MONR") == 0)
		{
			vector3df* tmp = (vector3df*)Hunk_AllocateTempMemory(size);
			file->read(tmp, size);
			for (u32 i=0; i<VCount; ++i)
			{
				GVertices[i].Normal = fixCoordinate(tmp[i]);
			}
			Hunk_FreeTempMemory(tmp);
		}
		else if (strcmp(fourcc, "MOTV") == 0)
		{
			vector2df* tmp = (vector2df*)Hunk_AllocateTempMemory(size);
			file->read(tmp, size);
			if (nTcoords == 0)
			{
				for (u32 i=0; i<VCount; ++i)
					TVertices[i].TCoords0 = tmp[i];
			}
			else
			{
				for (u32 i=0; i<VCount; ++i)
					TVertices[i].TCoords1 = tmp[i];
			}
			Hunk_FreeTempMemory(tmp);
			++nTcoords;
			_ASSERT(nTcoords <= 2);
		}
		else if(strcmp(fourcc, "MOBA") == 0)
		{
			NumBatches = size / sizeof(WMO::wmoGroupBatch);
			Batches = new SWMOBatch[NumBatches];

			WMO::wmoGroupBatch* bats = (WMO::wmoGroupBatch*)Hunk_AllocateTempMemory(NumBatches * sizeof(WMO::wmoGroupBatch));
			file->read(bats, NumBatches * sizeof(WMO::wmoGroupBatch));
			for (u32 i=0; i<NumBatches; ++i)
			{
				const WMO::wmoGroupBatch& b = bats[i];

				Batches[i].indexStart = b.indexStart;
				Batches[i].indexCount = b.indexCount;
				Batches[i].vertexStart = b.vertexStart;
				Batches[i].vertexEnd = b.vertexEnd;
				Batches[i].matId = b.matId;

				_ASSERT(b.matId < wmo->Header.nMaterials);
			}
			Hunk_FreeTempMemory(bats);
		}
		else if (strcmp(fourcc, "MOLR") == 0)
		{
			NumLights = size / sizeof(u16);
			Lights = new u16[NumLights];
			file->read(Lights, NumLights * sizeof(u16));		
		}
		else if (strcmp(fourcc, "MODR") == 0)
		{
			NumDoodads = size / sizeof(u16);
			Doodads = new u16[NumDoodads];
			file->read(Doodads, NumDoodads * sizeof(u16));		
		}
		else if (strcmp(fourcc, "MOBN") == 0)			//bsp node
		{
		}
		else if (strcmp(fourcc, "MOBR") == 0)				//bsp triangle
		{
		}
		else if (strcmp(fourcc, "MOCV") == 0)
		{
			SColor* colors = (SColor*)Hunk_AllocateTempMemory(size);
			file->read(colors, size);
			for (u32 i=0; i<VCount; ++i)
			{
				GVertices[i].Color = colors[i];
			}
			Hunk_FreeTempMemory(colors);
		}
		else if (strcmp(fourcc, "MLIQ") == 0)
		{
		}
		else if (strcmp(fourcc, "MOBS") == 0)
		{
		}
		else
		{
			_ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	delete file;

	GVertexBuffer = new IVertexBuffer;
	GVertexBuffer->set(GVertices, EST_G_PNC, VCount, EMM_STATIC);

	TVertexBuffer = new IVertexBuffer;
	TVertexBuffer->set(TVertices, EST_T_2T, VCount, EMM_STATIC);

	IndexBuffer = new IIndexBuffer;
	IndexBuffer->set(Indices, EIT_16BIT, ICount, EMM_STATIC);

	return true;
}
