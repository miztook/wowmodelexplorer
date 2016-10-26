#include "stdafx.h"
#include "CFileWMO.h"
#include "mywow.h"

CWMOGroup::CWMOGroup()
{
	flags = 0;

	Indices = NULL_PTR;
	Vertices = NULL_PTR;
	Batches = NULL_PTR;
	Lights = NULL_PTR;
	Doodads = NULL_PTR;
	BspTriangles = NULL_PTR;
	BspNodes = NULL_PTR;

	BspVertices = NULL_PTR;
	BspVertexBuffer = NULL_PTR;
	BspIndices = NULL_PTR;
	BspIndexbuffer = NULL_PTR;

	NumBatches = NumLights = NumDoodads =
		NumBspNodes = NumBspTriangles = 0;	
	VCount = ICount = 0;
	VStart = IStart = 0;

	outdoor = false;
	indoor = false;
	hasVertexColor = false;

	memset(name, 0, DEFAULT_SIZE);
}

CWMOGroup::~CWMOGroup()
{
	delete[]	Doodads;
	delete[]	Lights;
	delete[]	Batches;

	delete[] BspTriangles;
	delete[] BspNodes;
	delete BspIndexbuffer;
	delete BspVertexBuffer;
}

CFileWMO::CFileWMO()
{
	FileData = NULL_PTR;

	PortalVertexBuffer = NULL_PTR;

	FrontPortalEntries = BackPortalEntries = NULL_PTR;

	Vertices = NULL_PTR;
	Indices = NULL_PTR;
	VertexBuffer = NULL_PTR;
	IndexBuffer = NULL_PTR;
}

CFileWMO::~CFileWMO()
{
	delete[] Indices;
	delete[] Vertices;

	delete IndexBuffer;
	delete VertexBuffer;

	delete[] BackPortalEntries;
	delete[] FrontPortalEntries;

	delete PortalVertexBuffer;

	clear();
}

void CFileWMO::clear()
{
	delete[] Fogs;

	delete[] Doodads;
	delete[] ModelNamesBlock;
	delete[] DoodadSets;

	delete[] Lights;

	delete[] PortalRelations;
	delete[] Portals;
	delete[] PortalVertices;

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

bool CFileWMO::loadFile( IMemFile* file )
{
	const c8* name = file->getFileName();
	getFullFileNameNoExtensionA(name, Name, QMAX_PATH);

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
			ASSERT(version == 17);
		}
		else if (strcmp(fourcc, "MOHD") == 0)				//header
		{
			ASSERT(size == sizeof(Header));
			file->read(&Header, sizeof(Header));

			Materials = new SWMOMaterial[Header.nMaterials];
			Groups = new CWMOGroup[Header.nGroups];
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
			ASSERT(size == Header.nMaterials * sizeof(WMO::wmoMaterial));
			WMO::wmoMaterial* materials = (WMO::wmoMaterial*)Z_AllocateTempMemory(Header.nMaterials * sizeof(WMO::wmoMaterial));
			file->read(materials, Header.nMaterials * sizeof(WMO::wmoMaterial));
			for (u32 i=0; i<Header.nMaterials; ++i)
			{
				const WMO::wmoMaterial& m = materials[i];

				Materials[i].flags = m.flags;
				Materials[i].shaderType = m.shadertype;
				Materials[i].alphatest = m.alphatest != 0;

				Materials[i].color0.set(m.col1A, m.col1R, m.col1G, m.col1B);
				const c8* tex1 = &TextureFileNameBlock[m.tex1];
				Materials[i].texture0= g_Engine->getResourceLoader()->loadTexture(tex1);

				Materials[i].color1.set(m.col2A, m.col2R, m.col2G, m.col2B);
				const c8* tex2 = &TextureFileNameBlock[m.tex2];
				Materials[i].texture1 = g_Engine->getResourceLoader()->loadTexture(tex2);

				Materials[i].color2.set(m.col3A, m.col3R, m.col3G, m.col3B);
			}
			Z_FreeTempMemory(materials);
		}
		else if (strcmp(fourcc, "MOGN") == 0)					//group name
		{
			GroupNamesBlock = new c8[size];
			file->read(GroupNamesBlock, size);
			GroupNameBlockSize = size;
		}
		else if (strcmp(fourcc, "MOGI") == 0)					//group
		{
			ASSERT(size == Header.nGroups * sizeof(WMO::wmoGroupInfo));
			WMO::wmoGroupInfo* groupInfos = (WMO::wmoGroupInfo*)Z_AllocateTempMemory(Header.nGroups * sizeof(WMO::wmoGroupInfo));
			file->read(groupInfos, Header.nGroups * sizeof(WMO::wmoGroupInfo));
			for (u32 i=0; i<Header.nGroups; ++i)
			{
				const WMO::wmoGroupInfo& gi = groupInfos[i];
				Groups[i].flags = gi.flags;
				Groups[i].box.set(fixCoordinate(gi.min), fixCoordinate(gi.max));

				if(gi.nameIndex >= 0 && gi.nameIndex < (s32)GroupNameBlockSize)
					Q_strcpy(Groups[i].name, DEFAULT_SIZE, &GroupNamesBlock[gi.nameIndex]);
				else
					Q_strcpy(Groups[i].name, DEFAULT_SIZE, "");

				Groups[i].outdoor = (Groups[i].flags & 0x8) != 0;
				Groups[i].indoor = (Groups[i].flags & 0x2000) != 0;
			}

			Z_FreeTempMemory(groupInfos);
		}
		else if (strcmp(fourcc, "MOSB") == 0)			//skybox
		{

		}
		else if (strcmp(fourcc, "MOPV") == 0)			//portal vertices
		{
			ASSERT(PortalVertices == NULL_PTR);
			NumPortalVertices = size / sizeof(vector3df);
			vector3df v;
			if (NumPortalVertices)
			{
				PortalVertices = new vector3df[NumPortalVertices];
				vector3df* pVertices = (vector3df*)Z_AllocateTempMemory(sizeof(vector3df) * NumPortalVertices);
				file->read(pVertices, NumPortalVertices * sizeof(vector3df));
				for (u32 i=0; i<NumPortalVertices; ++i)
				{
					if(i % 4 == 2)
						PortalVertices[i] = fixCoordinate(pVertices[i/4 + 3]);
					else if(i % 4 == 3)
						PortalVertices[i] = fixCoordinate(pVertices[i/4 + 2]);
					else
						PortalVertices[i] = fixCoordinate(pVertices[i]);
				}

				Z_FreeTempMemory(pVertices);
			}
		}
		else if (strcmp(fourcc, "MOPT") == 0)					//portal info
		{
			ASSERT(size == Header.nPortals * sizeof(WMO::wmoPortalInfo));
			
			WMO::wmoPortalInfo* portalInfos = (WMO::wmoPortalInfo*)Z_AllocateTempMemory(Header.nPortals * sizeof(WMO::wmoPortalInfo));
			file->read(portalInfos, Header.nPortals * sizeof(WMO::wmoPortalInfo));

			for (u32 i=0; i<Header.nPortals; ++i)
			{
				const WMO::wmoPortalInfo& pi = portalInfos[i];

				Portals[i].vStart = pi.baseIndex;
				Portals[i].vCount = pi.numVerts;
				Portals[i].plane.Normal = fixCoordinate(pi.normal);
				Portals[i].plane.D = pi.d;
			}

			Z_FreeTempMemory(portalInfos);
		}
		else if (strcmp(fourcc, "MOPR") == 0)
		{

			NumPortalRelations = size / sizeof(WMO::wmoPortalRelation);
			PortalRelations = new SWMOPortalRelation[NumPortalRelations];

			WMO::wmoPortalRelation* relations = (WMO::wmoPortalRelation*)Z_AllocateTempMemory(NumPortalRelations * sizeof(WMO::wmoPortalRelation));
			file->read(relations, NumPortalRelations * sizeof(WMO::wmoPortalRelation));
			
			for (u32 i=0; i<NumPortalRelations; ++i)
			{
				const WMO::wmoPortalRelation& pr = relations[i];
				PortalRelations[i].portalIndex = pr.portal;
				PortalRelations[i].groupIndex = pr.group;
				PortalRelations[i].face = pr.dir == 1;

				ASSERT(pr.portal < Header.nPortals);
				ASSERT(pr.group < Header.nGroups);
			}

			Z_FreeTempMemory(relations);
		}
		else if (strcmp(fourcc, "MOLT") == 0)
		{
			ASSERT(size == Header.nLights * sizeof(WMO::wmoLight));
			WMO::wmoLight* lights = (WMO::wmoLight*)Z_AllocateTempMemory(Header.nLights * sizeof(WMO::wmoLight));
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

			Z_FreeTempMemory(lights);
		}
		else if (strcmp(fourcc, "MODS") == 0)			//doodad set
		{
			ASSERT(size == Header.nDoodadSets * sizeof(WMO::wmoDoodadSet));

			WMO::wmoDoodadSet* sets = (WMO::wmoDoodadSet*)Z_AllocateTempMemory(Header.nDoodadSets * sizeof(WMO::wmoDoodadSet));
			file->read(sets, Header.nDoodadSets * sizeof(WMO::wmoDoodadSet));

			for (u32 i=0; i<Header.nDoodadSets; ++i)
			{
				const WMO::wmoDoodadSet& s = sets[i];

				Q_strcpy(DoodadSets[i].name, 20, s.name);
				DoodadSets[i].start = s.start;
				DoodadSets[i].count = s.count;
			}
			Z_FreeTempMemory(sets);
		}
		else if (strcmp(fourcc, "MODN") == 0)				//doodad names
		{
			ModelNamesBlock = new c8[size];
			file->read(ModelNamesBlock, size);
			DoodadNameBlockSize = size;
		}
		else if (strcmp(fourcc, "MODD") == 0)				//doodad def
		{
			NumDoodads = size / sizeof(WMO::wmoDoodadDef);
			Doodads = new SWMODoodad[NumDoodads];

			WMO::wmoDoodadDef* dds = (WMO::wmoDoodadDef*)Z_AllocateTempMemory(NumDoodads * sizeof(WMO::wmoDoodadDef));
			file->read(dds, NumDoodads * sizeof(WMO::wmoDoodadDef));

			for (u32 i=0; i<NumDoodads; ++i)
			{
				const WMO::wmoDoodadDef& dd = dds[i];
				
				if(dd.nameIndex >= 0 && dd.nameIndex < (s32)DoodadNameBlockSize)
					Q_strcpy(Doodads[i].name, 256, &ModelNamesBlock[dd.nameIndex]);
				else
					Q_strcpy(Doodads[i].name, 256, "");

				Doodads[i].position = fixCoordinate(dd.pos);
				Doodads[i].quat.set(dd.rot[0], dd.rot[1], dd.rot[2], dd.rot[3]);
				Doodads[i].scale = dd.scale;
				Doodads[i].color.set(dd.colA, dd.colR, dd.colG, dd.colB);
			}

			Z_FreeTempMemory(dds);
		}
		else if (strcmp(fourcc, "MFOG") == 0)
		{
			NumFogs = size / sizeof(WMO::wmoFog);
			Fogs = new SWMOFog[NumFogs];

			WMO::wmoFog* fogs = (WMO::wmoFog*)Z_AllocateTempMemory(NumFogs * sizeof(WMO::wmoFog));
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

			Z_FreeTempMemory(fogs);
		}
		else if (strcmp(fourcc, "MOGP") == 0)
		{
		
		}
		else if (strcmp(fourcc, "MCVP") == 0)
		{
			
		}
		else if (strcmp(fourcc, "GFID") == 0)
		{

		}
		else
		{
		//	MessageBoxA(NULL_PTR, fourcc, "", 0);
			ASSERT(false);
		}

		file->seek((s32)nextpos);
	}

	//load grouops and build bouding box
	Box.set(vector3df(999999.9f), vector3df(-999999.9f));
	for (u32 i=0; i<Header.nGroups; ++i)
	{
		Groups[i].loadFile(i, this);
		Box.addInternalBox(Groups[i].box);

		//group box
		for (u32 c=0; c<Groups[i].NumBatches; ++c)
		{
			SWMOBatch* batch = &Groups[i].Batches[c];
			batch->box.set(vector3df(999999.9f), vector3df(-999999.9f));
			for (u32 k=batch->vertexStart; k<batch->vertexEnd; ++k)
			{
				batch->box.addInternalPoint(Groups[i].Vertices[k].Pos);
			}
		}
	}

	buildGroupBuffers();

	//portal
	for (u32 i=0; i<Header.nPortals; ++i)
	{
		SWMOPortal* p = &Portals[i];
		p->box.set(vector3df(999999.9f), vector3df(-999999.9f));
		for (u32 c=0; c<p->vCount; ++c)
		{
			p->box.addInternalPoint(PortalVertices[p->vStart + c]);
		}
	}

	//portal relation
	for (u32 i=0; i<NumPortalRelations; ++i)
	{
		SWMOPortalRelation* relation = &PortalRelations[i];
		SWMOPortal* p = &Portals[relation->portalIndex];
		ASSERT(relation->groupIndex < (s16)Header.nGroups);

		if (relation->face)
			p->frontGroupIndex = relation->groupIndex;
		else
			p->backGroupIndex = relation->groupIndex;
	}

	buildPortalEntries();

	PortalVertexBuffer = new IVertexBuffer(false);
	PortalVertexBuffer->set(PortalVertices, EST_P, NumPortalVertices, EMM_STATIC);

	return true;
}

void CFileWMO::buildGroupBuffers()
{
	u32 vCount = 0;
	u32 iCount = 0;
	for (u32 i=0; i<Header.nGroups; ++i)
	{
		CWMOGroup* group = &Groups[i];
		
		group->VStart = vCount;
		vCount += group->VCount;
		group->IStart = iCount;
		iCount += group->ICount;
	}

	Vertices = new SVertex_PNCT2[vCount];
	Indices = new u32[iCount];

	for (u32 i=0; i<Header.nGroups; ++i)
	{
		CWMOGroup* group = &Groups[i];

		Q_memcpy(&Vertices[group->VStart], sizeof(SVertex_PNCT2) * group->VCount, group->Vertices, sizeof(SVertex_PNCT2) * group->VCount);
		for (u32 k=0; k<group->ICount; ++k)
			Indices[group->IStart + k] = group->Indices[k] + group->VStart;

		//合并之后释放 group顶点
		delete[] group->Vertices;
		delete[] group->Indices;
	}

	VertexBuffer = new IVertexBuffer(false);
	VertexBuffer->set(Vertices, EST_PNCT2, vCount, EMM_STATIC);

	IndexBuffer = new IIndexBuffer(false);
	IndexBuffer->set(Indices, EIT_32BIT, iCount, EMM_STATIC);
}

bool CFileWMO::buildVideoResources()
{
	//CLock lock(&g_Globals.wmoCS);

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

	if(VertexBuffer)
		g_Engine->getHardwareBufferServices()->createHardwareBuffer(VertexBuffer);

	if (IndexBuffer)
		g_Engine->getHardwareBufferServices()->createHardwareBuffer(IndexBuffer);

	if (NumPortalVertices && PortalVertexBuffer)
		g_Engine->getHardwareBufferServices()->createHardwareBuffer(PortalVertexBuffer);

	VideoBuilt = true;

	return true;
}

void CFileWMO::releaseVideoResources()
{
	//CLock lock(&g_Globals.wmoCS);

	//texture
	for (u32 i=0; i<Header.nMaterials; ++i)
	{
		if (Materials[i].texture0 && Materials[i].texture0->getReferenceCount() == 2)
			Materials[i].texture0->releaseVideoTexture();

		if (Materials[i].texture1 && Materials[i].texture1->getReferenceCount() == 2)
			Materials[i].texture1->releaseVideoTexture();
	}

	if(VertexBuffer)
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(VertexBuffer);

	if (IndexBuffer)
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(IndexBuffer);

	if (NumPortalVertices && PortalVertexBuffer)
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(PortalVertexBuffer);

	VideoBuilt = false;
}

void CFileWMO::buildPortalEntries()
{
	FrontGroupRefMap.clear();
	FrontPortalEntries = new SPortalEntry[Header.nPortals];
	for (u32 i=0; i<Header.nPortals; ++i)
	{
		FrontPortalEntries[i].group0 = Portals[i].frontGroupIndex;
		FrontPortalEntries[i].portalIndex = i;
	}
	heapsort<SPortalEntry>(FrontPortalEntries, Header.nPortals);

	for (u32 i=0; i<Header.nPortals; ++i)
	{
		s16 id = FrontPortalEntries[i].group0;
		T_GroupRefMap::iterator itr = FrontGroupRefMap.find(id);
		if ( itr == FrontGroupRefMap.end() )
		{
			SGroupRef gr;
			gr.start = i;
			gr.count = 1;
			FrontGroupRefMap[id] = gr;
		}
		else
		{
			++itr->second.count;
		}
	}

	BackGroupRefMap.clear();
	BackPortalEntries = new SPortalEntry[Header.nPortals];
	for (u32 i=0; i<Header.nPortals; ++i)
	{
		BackPortalEntries[i].group0 = Portals[i].backGroupIndex;
		BackPortalEntries[i].portalIndex = i;
	}
	heapsort<SPortalEntry>(BackPortalEntries, Header.nPortals);

	for (u32 i=0; i<Header.nPortals; ++i)
	{
		s16 id = BackPortalEntries[i].group0;
		T_GroupRefMap::iterator itr = BackGroupRefMap.find(id);
		if ( itr == BackGroupRefMap.end() )
		{
			SGroupRef gr;
			gr.start = i;
			gr.count = 1;
			BackGroupRefMap[id] = gr;
		}
		else
		{
			++itr->second.count;
		}
	}
}

u32 CFileWMO::getPortalCountAsFront( u32 frontGroupIndex ) const
{
	T_GroupRefMap::const_iterator itr = FrontGroupRefMap.find((s16)frontGroupIndex);
	if (itr == FrontGroupRefMap.end())
		return 0;
	
	return itr->second.count;
}

s32 CFileWMO::getPortalIndexAsFront( u32 frontGroupIndex, u32 index ) const
{
	T_GroupRefMap::const_iterator itr = FrontGroupRefMap.find((s16)frontGroupIndex);
	if (itr == FrontGroupRefMap.end() || index >= itr->second.count)
		return -1;

	return FrontPortalEntries[itr->second.start + index].portalIndex;
}

u32 CFileWMO::getPortalCountAsBack( u32 backGroupIndex ) const
{
	T_GroupRefMap::const_iterator itr = BackGroupRefMap.find((s16)backGroupIndex);
	if (itr == BackGroupRefMap.end())
		return 0;

	return itr->second.count;
}

s32 CFileWMO::getPortalIndexAsBack( u32 backGroupIndex, u32 index ) const
{
	T_GroupRefMap::const_iterator itr = BackGroupRefMap.find((s16)backGroupIndex);
	if (itr == BackGroupRefMap.end() || index >= itr->second.count)
		return -1;

	return BackPortalEntries[itr->second.start + index].portalIndex;
}

bool CWMOGroup::loadFile( u32 index, IFileWMO* wmo )
{
	c8 path[QMAX_PATH];
	getFullFileNameNoExtensionA(wmo->Name, path, QMAX_PATH);

	c8 filename[QMAX_PATH];
	Q_sprintf(filename, QMAX_PATH, "%s_%03d.wmo", path, (s32)index);

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	ASSERT(file);
	if (!file)
		return false;

	WMO::wmoGroupHeader header;

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

		if (strcmp(fourcc, "MVER") == 0)				//version
		{
			u32 version;
			file->read(&version, size);
			ASSERT(version == 17);
		}
		else if (strcmp(fourcc, "MOGP") == 0)
		{
			size = sizeof(WMO::wmoGroupHeader);
			nextpos = file->getPos() + size;
			file->read(&header, sizeof(WMO::wmoGroupHeader));
		}
		else if (strcmp(fourcc, "MOPY") == 0)				//
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
			Vertices = new SVertex_PNCT2[VCount];

			vector3df* tmp = (vector3df*)Z_AllocateTempMemory(size);
			file->read(tmp, size);
			for (u32 i=0; i<VCount; ++i)
			{
				Vertices[i].Pos = fixCoordinate(tmp[i]);
			}
			Z_FreeTempMemory(tmp);
		}
		else if (strcmp(fourcc, "MONR") == 0)
		{
			vector3df* tmp = (vector3df*)Z_AllocateTempMemory(size);
			file->read(tmp, size);
			for (u32 i=0; i<VCount; ++i)
			{
				Vertices[i].Normal = fixCoordinate(tmp[i]);
				Vertices[i].Normal.normalize();
			}
			Z_FreeTempMemory(tmp);
		}
		else if (strcmp(fourcc, "MOTV") == 0)
		{
			vector2df* tmp = (vector2df*)Z_AllocateTempMemory(size);
			file->read(tmp, size);
			if (nTcoords == 0)
			{
				for (u32 i=0; i<VCount; ++i)
					Vertices[i].TCoords0 = tmp[i];
			}
			else
			{
				for (u32 i=0; i<VCount; ++i)
					Vertices[i].TCoords1 = tmp[i];
			}
			Z_FreeTempMemory(tmp);
			++nTcoords;
			ASSERT(nTcoords <= 2);
		}
		else if(strcmp(fourcc, "MOBA") == 0)
		{
			NumBatches = size / sizeof(WMO::wmoGroupBatch);
			Batches = new SWMOBatch[NumBatches];

			WMO::wmoGroupBatch* bats = (WMO::wmoGroupBatch*)Z_AllocateTempMemory(NumBatches * sizeof(WMO::wmoGroupBatch));
			file->read(bats, NumBatches * sizeof(WMO::wmoGroupBatch));
			for (u32 i=0; i<NumBatches; ++i)
			{
				const WMO::wmoGroupBatch& b = bats[i];

				Batches[i].indexStart = b.indexStart;
				Batches[i].indexCount = b.indexCount;
				Batches[i].vertexStart = b.vertexStart;
				Batches[i].vertexEnd = b.vertexEnd;
				Batches[i].matId = b.matId;

				ASSERT(b.matId < wmo->Header.nMaterials);
			}
			Z_FreeTempMemory(bats);
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
			NumBspNodes = size / sizeof(WMO::wmoBspNode);
			BspNodes = new SWMOBspNode[NumBspNodes];
			WMO::wmoBspNode* nodes = (WMO::wmoBspNode*)Z_AllocateTempMemory(NumBspNodes * sizeof(WMO::wmoBspNode));
			file->read(nodes, NumBspNodes * sizeof(WMO::wmoBspNode));
			for (u32 i=0; i<NumBspNodes; ++i)
			{
				const WMO::wmoBspNode& n = nodes[i];

				BspNodes[i].planetype = n.planetype;
				BspNodes[i].left = n.children[0];
				BspNodes[i].right = n.children[1];
				BspNodes[i].firstface = n.firstface;
				BspNodes[i].numfaces = n.numfaces;
				BspNodes[i].distance = n.distance;

			}
			Z_FreeTempMemory(nodes);
		}
		else if (strcmp(fourcc, "MOBR") == 0)				//bsp triangle
		{
			NumBspTriangles = size / sizeof(u16);
			BspTriangles = new u16[NumBspTriangles];
			file->read(BspTriangles, size);
		}
		else if (strcmp(fourcc, "MOCV") == 0)
		{
			hasVertexColor = true;
			SColor* colors = (SColor*)Z_AllocateTempMemory(size);
			file->read(colors, size);
			for (u32 i=0; i<VCount; ++i)
			{
				Vertices[i].Color = colors[i];
			}
			Z_FreeTempMemory(colors);
		}
		else if (strcmp(fourcc, "MLIQ") == 0)
		{
		}
		else if (strcmp(fourcc, "MOBS") == 0)
		{
		}
		else if (strcmp(fourcc, "MDAL") == 0)
		{
		}
		else if (strcmp(fourcc, "MOTA") == 0)
		{
		}
		else if (strcmp(fourcc, "MOPL") == 0)
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

//	buildBspVIBuffers();

	return true;
}

void CWMOGroup::buildBspVIBuffers()
{
	if (NumBspNodes > 0)
	{
		//bsp vertex buffer
		BspVertices = new SVertex_P[VCount];
		for (u32 i=0; i<VCount; ++i)
		{
			BspVertices[i].Pos = Vertices[i].Pos;
		}
		BspVertexBuffer = new IVertexBuffer;
		BspVertexBuffer->set(BspVertices, EST_P, VCount, EMM_STATIC);

		//bsp index buffer
		u32 icount = 0;
		for (u32 i=0; i<NumBspNodes; ++i)
		{
			SWMOBspNode* node = &BspNodes[i];
			node->startIndex = icount;
			icount += node->numfaces * 3;
		}

		BspIndices = new u16[icount];
		for (u32 i=0; i<NumBspNodes; ++i)
		{
			SWMOBspNode* node = &BspNodes[i];
			if (node->numfaces == 0)
				continue;

			node->minVertex = VCount;
			node->maxVertex = 0;

			u16* dest = &BspIndices[node->startIndex]; 
			for (u32 k=0; k<node->numfaces; ++k)
			{
				u16 triIndex = BspTriangles[node->firstface + k];

				dest[k*3] = Indices[triIndex*3];
				dest[k*3+1] = Indices[triIndex*3+1];
				dest[k*3+2] = Indices[triIndex*3+2];

				for (u32 c=0; c<3; ++c)
				{
					u16 current = dest[k*3+c];
					if (current > node->maxVertex)
						node->maxVertex = current;
					if (current < node->minVertex)
						node->minVertex = current;
				}
			}

			ASSERT(node->minVertex < node->maxVertex);
		}

		BspIndexbuffer = new IIndexBuffer;
		BspIndexbuffer->set(BspIndices, EIT_16BIT, icount, EMM_STATIC);
	}
}

