#include "stdafx.h"
#include "CFileM2.h"
#include "mywow.h"

CFileM2::CFileM2()
{
	FileData = nullptr;

	NumTextures = 0;

	Type = MT_NONE;
	IsCharacter = false;
	IsHD = false;
}

CFileM2::~CFileM2()
{
	clear();
}

void CFileM2::clear()
{
	clearAllActions();

	delete Skin;

	delete[] RibbonEmitters;
	delete[] ParticleSystems;
	
	delete[] ModelCameras;

	delete[] AttachLookup;
	delete[] Attachments;
	delete[] RenderFlags;
	delete[] BoneLookup;
	delete[]	AnimationLookup; 
	delete[] Bones;
	delete[] Animations;

	delete[]	TextureAnim;
	delete[] TransparencyLookup;
	delete[]	Transparencies;
	delete[] Colors;

	for (uint32_t i=0; i<NumTextures; ++i)
	{
		if (Textures[i])
			Textures[i]->drop();
	}

	delete[] GlobalSequences;
	delete[] TexLookup;
	delete[] Textures;
	delete[] TextureTypes;
	delete[] TextureFlags;
	delete[] BoundTris;
	delete[] Bounds;
	delete[] AVertices;
	delete[] GVertices;
}

bool CFileM2::loadFile( IMemFile* file )
{
	const char* magic = (const char*)file->getBuffer();
	if (strncmp(magic, "MD20", 4) == 0)
		M2Version = MD20;
	else if (strncmp(magic, "MD21", 4) == 0)
		M2Version = MD21;
	else
		ASSERT(false);

	if (M2Version == MD20)
		return loadFileMD20(file);
	else if (M2Version == MD21)
		return loadFileMD21(file);
	else
		return false;
}

bool CFileM2::loadFileMD20(IMemFile* file)
{
	ASSERT(file->getSize() >= sizeof(M2::Header20));

	FileData = file->getBuffer();
	m2Header = (M2::Header20*)FileData;

	char meshName[DEFAULT_SIZE];
	Q_strncpy(meshName, DEFAULT_SIZE, (const char*)&FileData[m2Header->_modelNameOffset], m2Header->_modelNameLength);
	meshName[m2Header->_modelNameLength] = '\0';

	const char* sharp = strstr(meshName, "#");
	if (!sharp)
		Q_strcpy(Name, DEFAULT_SIZE,  meshName);
	else
	{
		Q_strncpy(Name, DEFAULT_SIZE, meshName, sharp - meshName);
		Name[sharp - meshName] = '\0';
	}

	getFileNameNoExtensionA(file->getFileName(), FileName, DEFAULT_SIZE);

	getFileDirA(file->getFileName(), Dir, QMAX_PATH);
	Q_strcat(Dir, QMAX_PATH, "\\");

	Type = getM2Type(Dir);				//角色npc

	uint32_t race, gender;
	IsCharacter = g_Engine->getWowDatabase()->getRaceGender(Name, race, gender, IsHD);

	NumVertices = m2Header->_nVertices;
	NumBoundingVerts = m2Header->_nBoundingVertices;	
	NumBoundingTriangles = m2Header->_nBoundingTriangles;
	NumTextures = m2Header->_nTextures;
	NumTexLookup = m2Header->_nTexLookup;
	NumGlobalSequences = m2Header->_nGlobalSequences;
	NumColors = m2Header->_nColors;
	NumTransparencies = m2Header->_nTransparency;
	NumTranparencyLookukp = m2Header->_nTransLookup;
	NumTexAnim= m2Header->_nTextureanimations;
	NumAnimations = m2Header->_nAnimations;
	NumAnimationLookup = m2Header->_nAnimationLookup;
	NumBones = m2Header->_nBones;
	NumBoneLookup = m2Header->_nKeyBoneLookup;
	NumRenderFlags = m2Header->_nRenderFlags;
	NumAttachments = m2Header->_nAttachments;
	NumAttachLookup = m2Header->_nAttachLookup;
	NumParticleSystems = m2Header->_nParticleEmitters;
	NumRibbonEmitters = m2Header->_nRibbonEmitters;
	NumModelCameras = m2Header->_nCameras;

	if (Type == MT_CHARACTER && IsCharacter)
	{
		ASSERT(NumAnimationLookup > ANIMATION_HANDSCLOSED);
		ASSERT(NumBoneLookup >= BONE_MAX);
	}

	loadVertices();

	loadBounds();

	loadTextures();

	loadSequences();

	loadColor();

	loadTransparency();

	loadTextureAnimation();

	loadBones();

	loadRenderFlags();

	loadAttachments();

	loadModelCameras();

	loadParticleSystems();

	loadRibbonEmitters();

	if(!loadSkin(0))
	{
		return false;
	}

	return true;
}

bool CFileM2::loadFileMD21(IMemFile* file)
{
	ASSERT(file->getSize() >= sizeof(M2::Header21));

	FileData = file->getBuffer();
	m2HeaderEx = (M2::Header21*)FileData;
	m2Header = (M2::Header20*)&m2HeaderEx->_header20;

	FileData += (uint32_t)((uint8_t*)m2Header - (uint8_t*)m2HeaderEx);

	char meshName[DEFAULT_SIZE];
	Q_strncpy(meshName, DEFAULT_SIZE, (const char*)&FileData[m2Header->_modelNameOffset], m2Header->_modelNameLength);
	meshName[m2Header->_modelNameLength] = '\0';

	const char* sharp = strstr(meshName, "#");
	if (!sharp)
		Q_strcpy(Name, DEFAULT_SIZE,  meshName);
	else
	{
		Q_strncpy(Name, DEFAULT_SIZE, meshName, sharp - meshName);
		Name[sharp - meshName] = '\0';
	}

	getFileNameNoExtensionA(file->getFileName(), FileName, DEFAULT_SIZE);

	getFileDirA(file->getFileName(), Dir, QMAX_PATH);
	Q_strcat(Dir, QMAX_PATH, "\\");

	Type = getM2Type(Dir);				//角色npc

	uint32_t race, gender;
	IsCharacter = g_Engine->getWowDatabase()->getRaceGender(Name, race, gender, IsHD);

	NumVertices = m2Header->_nVertices;
	NumBoundingVerts = m2Header->_nBoundingVertices;	
	NumBoundingTriangles = m2Header->_nBoundingTriangles;
	NumTextures = m2Header->_nTextures;
	NumTexLookup = m2Header->_nTexLookup;
	NumGlobalSequences = m2Header->_nGlobalSequences;
	NumColors = m2Header->_nColors;
	NumTransparencies = m2Header->_nTransparency;
	NumTranparencyLookukp = m2Header->_nTransLookup;
	NumTexAnim= m2Header->_nTextureanimations;
	NumAnimations = m2Header->_nAnimations;
	NumAnimationLookup = m2Header->_nAnimationLookup;
	NumBones = m2Header->_nBones;
	NumBoneLookup = m2Header->_nKeyBoneLookup;
	NumRenderFlags = m2Header->_nRenderFlags;
	NumAttachments = m2Header->_nAttachments;
	NumAttachLookup = m2Header->_nAttachLookup;
	NumParticleSystems = m2Header->_nParticleEmitters;
	NumRibbonEmitters = m2Header->_nRibbonEmitters;
	NumModelCameras = m2Header->_nCameras;

	if (Type == MT_CHARACTER && IsCharacter)
	{
		ASSERT(NumAnimationLookup > ANIMATION_HANDSCLOSED);
		ASSERT(NumBoneLookup >= BONE_MAX);
	}

	loadVertices();

	loadBounds();

	loadTextures();

	loadSequences();

	loadColor();

	loadTransparency();

	loadTextureAnimation();

	loadBones();

	loadRenderFlags();

	loadAttachments();

	loadModelCameras();

	loadParticleSystems();

	loadRibbonEmitters();

	if(!loadSkin(0))
	{
		return false;
	}

	return true;
}

void CFileM2::loadVertices()
{
	if (NumVertices == 0)
		return;

	GVertices = new SVertex_PNT2W[NumVertices];
	AVertices = new SVertex_A[NumVertices];

	M2::vertex* v = (M2::vertex*)(&FileData[m2Header->_ofsVertices]);

	for (uint32_t i=0; i<NumVertices; ++i)
	{
		GVertices[i].Pos = fixCoordinate(v[i]._Position);
		GVertices[i].Normal = fixCoordinate(v[i]._Normal);
		GVertices[i].TCoords0 = v[i]._TextureCoords0;
		GVertices[i].TCoords1 = v[i]._TextureCoords1;
		for (uint32_t j=0; j<4; ++j)
		{
			GVertices[i].Weights[j] = v[i]._BoneWeight[j];
			AVertices[i].BoneIndices[j] = v[i]._BoneIndices[j];
		}

		if (i == 0)
			BoundingBox.reset(GVertices[i].Pos);
		else
			BoundingBox.addInternalPoint(GVertices[i].Pos);
	}
}

void CFileM2::loadBounds()
{
	if (NumBoundingVerts > 0)
	{
		Bounds = new vector3df[NumBoundingVerts];
		vector3df* b = (vector3df*)(&FileData[m2Header->_ofsBoundingVertices]);
		for (uint32_t i=0; i<NumBoundingVerts; ++i)
		{
			Bounds[i] = fixCoordinate(b[i]);
		}
	}

	if (NumBoundingTriangles > 0)
	{
		BoundTris = new uint16_t[NumBoundingTriangles];
		uint16_t* idx = (uint16_t*)(&FileData[m2Header->_ofsBoundingTriangles]);
		Q_memcpy( BoundTris, sizeof(uint16_t)*NumBoundingTriangles, idx, sizeof(uint16_t) * NumBoundingTriangles);
	}
	
	BoundingAABBox = aabbox3df(fixCoordinate(m2Header->_boundingbox.MinEdge),fixCoordinate(m2Header->_boundingbox.MaxEdge));
	BoundingRadius = m2Header->_boundingRadius;
	CollisionAABBox = aabbox3df(fixCoordinate(m2Header->_vertexBox.MinEdge),fixCoordinate(m2Header->_vertexBox.MaxEdge));
	CollisionRadius = m2Header->_vertexRadius;
}

void CFileM2::loadTextures()
{
	if (NumTextures == 0)
		return;

	TextureFlags = new uint32_t[NumTextures];
	TextureTypes = new ETextureTypes[NumTextures];
	Textures = new ITexture*[NumTextures];

	IResourceLoader* resourceLoader = g_Engine->getResourceLoader();

	M2::texture* texDef = (M2::texture*)(&FileData[m2Header->_ofsTextures]);

	for (uint32_t i=0; i<NumTextures; ++i)
	{
		TextureFlags[i] = texDef[i]._flags;
		TextureTypes[i] = (ETextureTypes)texDef[i]._type;

		if (TextureTypes[i] == 0)
		{
			const char* name = (const char*)(&FileData[texDef[i]._ofsFilename]);
			uint32_t len =  texDef[i]._lenFilename;
			ASSERT(len < QMAX_PATH);
			string256 texname(name);
			Textures[i] = resourceLoader->loadTexture(texname.c_str());
			ASSERT(Textures[i]);	
		}
		else
		{
			Textures[i] = nullptr;
		}
	}

	if (NumTexLookup > 0)
	{
		TexLookup = new int16_t[NumTexLookup];
		int16_t* t = (int16_t*)(&FileData[m2Header->_ofsTexLookup]);
		Q_memcpy(TexLookup, sizeof(int16_t)*NumTexLookup, t, sizeof(int16_t)*NumTexLookup);
	}
}

void CFileM2::loadAttachments()
{
	//attachment
	if (NumAttachments > 0)
	{
		Attachments = new SModelAttachment[NumAttachments];
		M2::attach* att = (M2::attach*)(&FileData[m2Header->_ofsAttachments]);
		for (uint32_t i=0; i<NumAttachments; ++i)
		{
			Attachments[i].id = att[i]._Id;
			Attachments[i].position = fixCoordinate(att[i]._Position);
 			Attachments[i].boneIndex = (att[i]._Bone >= 0 && att[i]._Bone < (int32_t)NumBones) ? 
 				att[i]._Bone : -1;
		}
	}

	if (NumAttachLookup > 0)
	{
		AttachLookup = new int16_t[NumAttachLookup];
		int16_t* p = (int16_t*)(&FileData[m2Header->_ofsAttachLookup]);
		Q_memcpy(AttachLookup, sizeof(int16_t)*NumAttachLookup, p, sizeof(int16_t)*NumAttachLookup);
	}
}

void CFileM2::loadSequences()
{
	//global sequences
	if (NumGlobalSequences > 0)
	{
		GlobalSequences = new int32_t[NumGlobalSequences];
		int32_t* p = (int32_t*)(&FileData[m2Header->_ofsGlobalSequences]);
		Q_memcpy(GlobalSequences, sizeof(int32_t)*NumGlobalSequences, p, sizeof(int32_t)*NumGlobalSequences);
	}
}

void CFileM2::loadColor()
{
	if (NumColors > 0)
	{
		Colors = new SModelColor[NumColors];
		M2::colorDef* c = (M2::colorDef*)(&FileData[m2Header->_ofsColors]);
		for (uint32_t i=0; i<NumColors; ++i)
			Colors[i].init(FileData, GlobalSequences, NumGlobalSequences, c[i]);
	}
}

void CFileM2::loadTransparency()
{
	//transparency
	if (NumTransparencies > 0)
	{
		Transparencies = new SModelTransparency[NumTransparencies];
		M2::transDef* t = (M2::transDef*)(&FileData[m2Header->_ofsTransparency]);
		for (uint32_t i=0; i<NumTransparencies; ++i)
			Transparencies[i].init(FileData, GlobalSequences, NumGlobalSequences, t[i]);
	}

	//transparency lookup
	if (NumTranparencyLookukp > 0)
	{
		TransparencyLookup = new int16_t[NumTranparencyLookukp];
		int16_t* t = (int16_t*)(&FileData[m2Header->_ofsTransLookup]);
		Q_memcpy(TransparencyLookup, sizeof(int16_t)*NumTranparencyLookukp, t, sizeof(int16_t)*NumTranparencyLookukp);
	}
}

void CFileM2::loadTextureAnimation()
{
	if (NumTexAnim > 0)
	{
		TextureAnim = new SModelTextureAnim[NumTexAnim];
		M2::texanimDef* t = (M2::texanimDef*)(&FileData[m2Header->_ofsTextureanimations]);
		for (uint32_t i=0; i<NumTexAnim; ++i)
			TextureAnim[i].init(FileData, GlobalSequences, NumGlobalSequences, t[i]);
	}
}

void CFileM2::loadBones()
{
	if (NumAnimations == 0)
		return;

	IMemFile** animMpqs = (IMemFile**)Z_AllocateTempMemory(NumAnimations * sizeof(IMemFile*));
	SAnimFile* animFiles = (SAnimFile*)Z_AllocateTempMemory(NumAnimations * sizeof(SAnimFile));
	memset(animFiles, 0, NumAnimations * sizeof(SAnimFile));

	Animations = new SModelAnimation[NumAnimations];
	memset(Animations, 0, sizeof(SModelAnimation) * NumAnimations);

	for (uint32_t i=0; i<NumAnimations; ++i)
	{
		M2::animseq* anim = (M2::animseq*)(&FileData[m2Header->_ofsAnimations]);

		Animations[i].animID = anim[i]._AnimationID;
		Animations[i].animSubID	=	anim[i]._SubAnimationID;
		Animations[i].timeLength = anim[i]._Length;
		Animations[i].NextAnimation = anim[i]._NextAnimation;
		Animations[i].Index = anim[i]._Index;

// 		const char* animName = g_Engine->getWowDatabase()->getAnimationName(Animations[i].animID);
// 		if (strcmp(animName, "Stand") != 0)
// 		{
// 			animMpqs[i] = nullptr;
// 			continue;
// 		}

		char   filename[QMAX_PATH];
		Q_sprintf(filename, QMAX_PATH, "%s%s%04d-%02d.anim", Dir, Name, Animations[i].animID, Animations[i].animSubID);

		animMpqs[i] = g_Engine->getWowEnvironment()->openFile(filename);
		if (animMpqs[i])
		{
			animFiles[i].data = animMpqs[i]->getBuffer();
			animFiles[i].size = animMpqs[i]->getSize();
		}
	}

	//动画
	for (uint32_t i=0; i<NumAnimations; ++i)
	{
		if (Animations[i].animSubID != 0)
			continue;

		const char* animName = g_Engine->getWowDatabase()->getAnimationName(Animations[i].animID);
		AnimationNameLookup[animName] = Animations[i].Index;
	}

	if (NumAnimationLookup > 0)
	{
		AnimationLookup = new int16_t[NumAnimationLookup];
		int16_t* a = (int16_t*)(&FileData[m2Header->_ofsAnimationLookup]);
		Q_memcpy(AnimationLookup, sizeof(int16_t)*NumAnimationLookup, a, sizeof(int16_t)*NumAnimationLookup);
	}

	if (NumBoneLookup > 0)
	{
		BoneLookup = new int16_t[NumBoneLookup];
		int16_t* b = (int16_t*)(&FileData[m2Header->_ofsKeyBoneLookup]);
		Q_memcpy(BoneLookup, sizeof(int16_t)*NumBoneLookup, b, sizeof(int16_t)*NumBoneLookup);
	}

	if (NumBones > 0)
	{
		Bones = new SModelBone[NumBones];
		M2::bone* b = (M2::bone*)(&FileData[m2Header->_ofsBones]);
		for (uint32_t i=0; i<NumBones; ++i)
		{
			Bones[i].init(FileData, GlobalSequences, NumGlobalSequences, b[i], animFiles);

			if (Type == MT_CHARACTER)
				setBoneType((int16_t)i);
		}
	}

	for (int32_t i=NumAnimations-1; i>=0; --i)
	{	
		if (animMpqs[i])
			delete animMpqs[i];
	}

	Z_FreeTempMemory(animFiles);
	Z_FreeTempMemory(animMpqs);

}

void CFileM2::loadRenderFlags()
{
	if (NumRenderFlags > 0)
	{
		RenderFlags = new SRenderFlag[NumRenderFlags];

		M2::renderflag* rfs = (M2::renderflag*)&FileData[m2Header->_ofsRenderFlags];

		for (uint32_t i=0; i<NumRenderFlags; ++i)
		{		
			//raw
			RenderFlags[i].flags = rfs[i].flags;
			RenderFlags[i].blend = rfs[i].blend;
		}
	}
}

void CFileM2::loadParticleSystems()
{
	if (NumParticleSystems == 0)
		return;

	if (m2Header->_version[0] >= 0x10)
	{
		M2::ModelParticleEmitterDefV10* pdefsV10 = (M2::ModelParticleEmitterDefV10*)&FileData[m2Header->_ofsParticleEmitters];
		M2::ModelParticleEmitterDef*	pdefs;

		ParticleSystems = new ParticleSystem[NumParticleSystems];
		for (uint32_t i=0; i<NumParticleSystems; ++i)
		{
			pdefs = (M2::ModelParticleEmitterDef*)&pdefsV10[i];
			ParticleSystems[i].Mesh = this;
			ParticleSystems[i].init(*pdefs, FileData, GlobalSequences, NumGlobalSequences);
		}
	}
	else
	{
		M2::ModelParticleEmitterDef*	pdefs =  (M2::ModelParticleEmitterDef*)&FileData[m2Header->_ofsParticleEmitters];

		ParticleSystems = new ParticleSystem[NumParticleSystems];
		for (uint32_t i=0; i<NumParticleSystems; ++i)
		{
			ParticleSystems[i].Mesh = this;
			ParticleSystems[i].init(pdefs[i], FileData, GlobalSequences, NumGlobalSequences);
		}
	}
}

void CFileM2::loadRibbonEmitters()
{
	if (NumRibbonEmitters == 0)
		return;

	M2::ModelRibbonEmitterDef* rdefs = (M2::ModelRibbonEmitterDef*)&FileData[m2Header->_ofsRibbonEmitters];
	RibbonEmitters = new RibbonEmitter[NumRibbonEmitters];
	for(uint32_t i=0; i<NumRibbonEmitters; ++i)
	{
		RibbonEmitters[i].Mesh = this;
		RibbonEmitters[i].init(rdefs[i], FileData, GlobalSequences, NumGlobalSequences);
	}
}

void CFileM2::loadModelCameras()
{
	if (NumModelCameras == 0)
		return;
	if (m2Header->_version[0] >= 0x10)
	{
		M2::ModelCameraDefV10* cdefs =(M2::ModelCameraDefV10*)&FileData[m2Header->_ofsCameras];
		ModelCameras = new SModelCamera[NumModelCameras];
		for (uint32_t i=0; i<NumModelCameras; ++i)
		{
			ModelCameras[i].init(cdefs[i], FileData, GlobalSequences);
		}
	}
}

bool CFileM2::loadSkin(uint32_t idx)
{
	char   filename[QMAX_PATH];
	
	Q_sprintf(filename, QMAX_PATH, "%s%s%02d.skin", Dir, Name, (int32_t)idx);
	if(!g_Engine->getWowEnvironment()->exists(filename))
		Q_sprintf(filename, QMAX_PATH, "%s%s%02d.skin", Dir, FileName, (int32_t)idx);

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file)
	{
		Skin = new CFileSkin;
		bool success = Skin->loadFile(file, this);
		ASSERT(success);
		delete file;
		return success;
	}

	return true;
}

int16_t CFileM2::getAnimationIndex( const char* name, uint32_t subIdx /*= 0*/ ) const
{
	T_AnimationLookup::const_iterator itr = AnimationNameLookup.find(name);
	if (itr == AnimationNameLookup.end())
		return -1;

	int16_t next = itr->second;
	while (next != -1)
	{
		if (Animations[next].animSubID == subIdx)
			break;
		next = Animations[next].NextAnimation;
	}
	return next;
}

uint32_t CFileM2::getAnimationCount( const char* name ) const
{
	T_AnimationLookup::const_iterator itr = AnimationNameLookup.find(name);
	if (itr == AnimationNameLookup.end())
		return 0;

	int16_t next = itr->second;
	uint32_t count = 0;
	while (next != -1)
	{
		next = Animations[next].NextAnimation;
		++count;
	}
	return count;
}

bool CFileM2::buildVideoResources()
{
	//CLock lock(&g_Globals.m2CS);

	if (VideoBuilt || (!Skin || Skin->NumGeosets == 0))
	{
		return true;
	}

	//texture
	for (uint32_t i=0; i<NumTextures; ++i)
	{
		if (Textures[i])
			IVideoResource::buildVideoResources(Textures[i]);
	}

	//gvertex buffer
	Skin->GVertexBuffer->set(GVertices, EST_PNT2W, NumVertices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->GVertexBuffer);

	//bone buffer
	Skin->AVertexBuffer->set(Skin->AVertices, EST_A, Skin->NumBoneVertices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->AVertexBuffer);

	//index buffer
	Skin->IndexBuffer->set(Skin->Indices, EIT_16BIT, Skin->NumIndices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->IndexBuffer);


	VideoBuilt = true;

	return true;
}

void CFileM2::releaseVideoResources()
{
	//CLock lock(&g_Globals.m2CS);

	if (!Skin || Skin->NumGeosets == 0)
	{
		return;
	}

	for (uint32_t i=0; i<NumTextures; ++i)
	{
		if (Textures[i] && Textures[i]->getReferenceCount()==2)		//refcount==2说明此时纹理只被当前m2文件使用，可以release
			IVideoResource::releaseVideoResources(Textures[i]);
	}

	if (Skin->IndexBuffer)
	{
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Skin->IndexBuffer);
	}

	if (Skin->AVertexBuffer)
	{
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Skin->AVertexBuffer);
	}

	if (Skin->GVertexBuffer)
	{
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Skin->GVertexBuffer);
	}

	VideoBuilt = false;
}

void CFileM2::setBoneType( int16_t boneIdx )
{
	for (uint32_t i=0; i<5; ++i)
	{
		int16_t fistIndex = BoneLookup[BONE_LFINGER1 + i];
		if (fistIndex == boneIdx)
		{
	 		Bones[boneIdx].bonetype = EBT_LEFTHAND;
	 		return;
		}
	}

	for (uint32_t i=0; i<5; ++i)
	{
		int16_t fistIndex = BoneLookup[BONE_RFINGER1 + i];
		if (fistIndex == boneIdx)
		{
			Bones[boneIdx].bonetype = EBT_RIGHTHAND;
			return;
		}
	}

	int32_t parent = Bones[boneIdx].parent;
	while(parent != -1)
	{
		for (uint32_t i=0; i<5; ++i)
		{
			int16_t fistIndex = BoneLookup[BONE_LFINGER1 + i];
			if (fistIndex == parent)
			{
				Bones[boneIdx].bonetype = EBT_LEFTHAND;
				return;
			}
		}

		for (uint32_t i=0; i<5; ++i)
		{
			int16_t fistIndex = BoneLookup[BONE_RFINGER1 + i];
			if (fistIndex == parent)
			{
				Bones[boneIdx].bonetype = EBT_RIGHTHAND;
				return;
			}
		}

		parent = Bones[parent].parent;
	}

	Bones[boneIdx].bonetype = EBT_NONE;
}

wow_m2Action* CFileM2::getAction( const char* name ) const
{
	T_ActionMap::const_iterator itr = ActionMap.find(name);
	if (itr != ActionMap.end())
	{
		return itr->second;
	}
	return nullptr;
}

void CFileM2::clearAllActions()
{
	for(T_ActionMap::const_iterator itr = ActionMap.begin(); itr != ActionMap.end(); ++itr)
	{
		delete itr->second;
	}
	ActionMap.clear();
}

bool CFileM2::addAction( wow_m2Action* action )
{
	T_ActionMap::const_iterator itr = ActionMap.find(action->getActionName());
	if (itr != ActionMap.end())
	{
		ASSERT(false);
		return false;
	}

	ActionMap[action->getActionName()] = action;

	return true;
}

uint32_t CFileM2::getSkinIndex(uint32_t race, uint32_t gender, bool isHD)
{
	if (Type == MT_CHARACTER && IsCharacter)
	{
		if (isHD)
		{
			switch(race)
			{
			case RACE_UNDEAD:
				return gender ? 2 : 0;
			default:
				return 0;
			}
		}
		else
		{
			switch(race)
			{
			case RACE_GOBLIN:
				return gender ? 0 : 1;
			default:
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}
}

CFileSkin::CFileSkin()
	: Geosets(nullptr), Indices(nullptr), AVertices(nullptr), NumIndices(0), NumBoneVertices(0),
	NumGeosets(0), NumTexUnit(0), GVertexBuffer(nullptr), AVertexBuffer(nullptr), IndexBuffer(nullptr)
{ 
	GVertexBuffer = new CVertexBuffer(false);
	AVertexBuffer = new CVertexBuffer(false);
	IndexBuffer = new CIndexBuffer(false);
}

CFileSkin::~CFileSkin()
{
	delete[] Indices;
	delete[] AVertices;
	delete[] Geosets;

	delete IndexBuffer;
	delete AVertexBuffer;
	delete GVertexBuffer;
}

bool CFileSkin::loadFile( IMemFile* file, CFileM2* m2)
{
	uint8_t* skinBuffer = file->getBuffer();

	M2::skin_header* skinHeader = (M2::skin_header*)skinBuffer;

	NumGeosets = skinHeader->_nSubmeshes;
	NumTexUnit = skinHeader->_nTextureUnits;
	NumIndices = skinHeader->_nTriangles;
	if (NumGeosets == 0 || NumTexUnit == 0 || NumIndices == 0)
		return true;

	//indices
	uint16_t* indexLookup = (uint16_t*)(&skinBuffer[skinHeader->_ofsIndices]);
	uint16_t* triangles = (uint16_t*)(&skinBuffer[skinHeader->_ofsTriangles]);

	Indices= new uint16_t[NumIndices];
	for (uint32_t i=0; i<NumIndices; ++i)
	{
		Indices[i] = indexLookup[triangles[i]];
	}

	//geosets
	Geosets = new CGeoset[NumGeosets];

	//texture unit
	uint16_t* texanimlookup = (uint16_t*)(m2->getFileData() + m2->m2Header->_ofsTexAnimLookup);
	uint32_t numtexanimlookup = m2->m2Header->_nTexAnimLookup;

	M2::textureUnit* t = (M2::textureUnit*)(&skinBuffer[skinHeader->_ofsTextureUnits]);
	for (uint32_t i=0; i<NumTexUnit; ++i)
	{
		CGeoset* geo = &Geosets[t[i]._submeshIdx];
		STexUnit texUnit;

		texUnit.Mode = t[i]._mode;
		texUnit.Shading = t[i]._shading;
		texUnit.TexID = (t[i]._textureIdx >= 0 && t[i]._textureIdx < (int16_t)m2->NumTexLookup) ? 
			m2->TexLookup[t[i]._textureIdx] : -1;

		texUnit.rfIndex = (t[i]._renderFlagsIdx >= 0 && t[i]._renderFlagsIdx < (int16_t)m2->NumRenderFlags) ? 
			t[i]._renderFlagsIdx : -1;
		texUnit.ColorIndex = (t[i]._colorIdx >= 0 && t[i]._colorIdx < (int16_t)m2->NumColors) ?
			t[i]._colorIdx : -1;

		texUnit.TransIndex = (t[i]._transparencyIdx >= 0 && t[i]._transparencyIdx < (int16_t)m2->NumTranparencyLookukp) ? 
			m2->TransparencyLookup[t[i]._transparencyIdx] : -1;
		texUnit.TexAnimIndex = (t[i]._animationIdx >= 0 && t[i]._animationIdx < (int16_t)numtexanimlookup) ?
			texanimlookup[t[i]._animationIdx] : -1;
		texUnit.TexFlags = texUnit.TexID == -1 ? 0 : m2->TextureFlags[texUnit.TexID];

		geo->TexUnits.push_back(texUnit);
	}

	uint32_t	boneVerticesOffset = 0;
	uint32_t  boneVerticesCount = 0;
	T_BoneVerticesList	boneVertList;
	CBoneUnit::T_BoneIndices triangleBoneIndices;

	//bool isHD = m2->isHD();
	M2::submesh* sm = (M2::submesh*)(&skinBuffer[skinHeader->_ofsSubmeshes]);
	for (uint32_t i=0; i<NumGeosets; ++i)
	{
		uint16_t nInds = sm[i]._nTriangles;

		CGeoset* set = &Geosets[i];
		set->VStart = sm[i]._startVertex;
		set->VCount = sm[i]._nVertices;
		set->IStart = sm[i]._startTriangle;
		set->ICount = nInds;
		set->GeoID = (sm[i]._ID & 0xffff);	
		set->MaxWeights = 0;
		set->BillBoard = true;

		set->BoneUnits.clear();
		set->BoneUnits.emplace_back(CBoneUnit());
		CBoneUnit* currentBoneUnit = &set->BoneUnits.back();
		currentBoneUnit->Index = (int8_t)set->BoneUnits.size() -1;
		
		uint16_t lastIndex = 0;
		for (uint16_t k=0; k<set->ICount/3; ++k)
		{
			uint16_t index0 = Indices[sm[i]._startTriangle + 3*k];
			uint16_t index1 = Indices[sm[i]._startTriangle + 3*k + 1];
			uint16_t index2 = Indices[sm[i]._startTriangle + 3*k + 2];

			SVertex_PNT2W* g[3];
			SVertex_A* b[3];

			g[0] = &m2->GVertices[index0];
			g[1] = &m2->GVertices[index1];
			g[2] = &m2->GVertices[index2];
			b[0] = &m2->AVertices[index0];
			b[1] = &m2->AVertices[index1];
			b[2] = &m2->AVertices[index2];

			//find bone indices used by triangle
			triangleBoneIndices.clear();
			for (uint32_t n=0; n<3; ++n)
			{
				uint32_t weights = 0;
				for (uint16_t m=0; m<MAX_BONE_BLEND; ++m)
				{
					if (g[n]->Weights[m] == 0)
						continue;

					weights += g[n]->Weights[m];

					uint8_t idx = b[n]->BoneIndices[m];
					if (idx >= m2->NumBones)
						continue;

					if (!m2->Bones[idx].billboard)
						set->BillBoard = false;

					triangleBoneIndices.insert(idx);

					if (weights >= 255)
					{
						set->MaxWeights = max_(set->MaxWeights, (uint16_t)(m+1));
						break;
					}
				}
			}

			if (currentBoneUnit->BoneIndices.size() + triangleBoneIndices.size() <= MAX_BONE_NUM)		//不超过MAX_BONE_NUM
			{
				for (CBoneUnit::T_BoneIndices::const_iterator itr = triangleBoneIndices.begin(); 
					itr!=triangleBoneIndices.end(); ++itr)
				{
					currentBoneUnit->BoneIndices.insert(*itr);
				}
			}
			else			//切分geoset
			{
				CBoneUnit::T_bone2boneMap global2localMap;

				//index buffer
				uint16_t tcount = k - lastIndex;
				currentBoneUnit->StartIndex = set->IStart + lastIndex * 3;
				lastIndex = k;
				currentBoneUnit->TCount = tcount;

				//bonemap
				currentBoneUnit->local2globalMap.clear();
				for (CBoneUnit::T_BoneIndices::const_iterator itr = currentBoneUnit->BoneIndices.begin(); 
					itr!=currentBoneUnit->BoneIndices.end(); ++itr)
				{
					uint8_t boneIndex = *itr;

					if(boneIndex >= m2->NumBones)
						continue;

					currentBoneUnit->local2globalMap.push_back(boneIndex);
					global2localMap[boneIndex] = (uint8_t)currentBoneUnit->local2globalMap.size() - 1;
				}
				currentBoneUnit->BoneCount = (uint8_t)currentBoneUnit->local2globalMap.size();

				//分配临时内存
				SBoneVertEntry entry;
				entry.num = set->VCount;
				entry.aVertices = (SVertex_A*)Z_AllocateTempMemory(sizeof(SVertex_A) * set->VCount);
				boneVertList.push_back(entry);

				currentBoneUnit->BoneVStart = boneVerticesOffset;
				boneVerticesOffset += set->VCount;

				for (uint32_t m=0; m<set->VCount; ++m)
				{
					SVertex_A* v = &m2->AVertices[set->VStart + m];		
					for (uint32_t n=0; n<MAX_BONE_BLEND; ++n)
					{
						CBoneUnit::T_bone2boneMap::const_iterator itr = global2localMap.find(v->BoneIndices[n]);
						if (itr != global2localMap.end())
							entry.aVertices[m].BoneIndices[n] = itr->second;
						else
							entry.aVertices[m].BoneIndices[n] = 0;

						ASSERT(entry.aVertices[m].BoneIndices[n] < currentBoneUnit->BoneCount);
					}
				}

				//next
				set->BoneUnits.emplace_back(CBoneUnit());
				currentBoneUnit = &set->BoneUnits.back();
				currentBoneUnit->Index = (int8_t)set->BoneUnits.size() -1;
				currentBoneUnit->BoneVStart = boneVerticesOffset;
			}		
		}

		uint16_t k = set->ICount / 3;
		if (!currentBoneUnit->BoneIndices.empty())
		{
			CBoneUnit::T_bone2boneMap global2localMap;

			//index buffer
			uint32_t tcount = k - lastIndex;
			currentBoneUnit->StartIndex = set->IStart + lastIndex * 3;
			currentBoneUnit->TCount = tcount;

			//bonemap
			currentBoneUnit->local2globalMap.clear();
			for (CBoneUnit::T_BoneIndices::const_iterator itr = currentBoneUnit->BoneIndices.begin(); 
				itr!=currentBoneUnit->BoneIndices.end(); ++itr)
			{
				uint8_t boneIndex = *itr;

				if(boneIndex >= m2->NumBones)
					continue;

				currentBoneUnit->local2globalMap.push_back(boneIndex);
				global2localMap[boneIndex] = (uint8_t)currentBoneUnit->local2globalMap.size() - 1;
			}
			currentBoneUnit->BoneCount = (uint8_t)currentBoneUnit->local2globalMap.size();

			//分配临时内存
			SBoneVertEntry entry;
			entry.num = set->VCount;
			entry.aVertices = (SVertex_A*)Z_AllocateTempMemory(sizeof(SVertex_A) * set->VCount);
			boneVertList.emplace_back(entry);

			currentBoneUnit->BoneVStart = boneVerticesOffset;
			boneVerticesOffset += set->VCount;
			for (uint32_t m=0; m<set->VCount; ++m)
			{
				SVertex_A* v = &m2->AVertices[set->VStart + m];
				for (uint32_t n=0; n<MAX_BONE_BLEND; ++n)
				{
					CBoneUnit::T_bone2boneMap::const_iterator itr = global2localMap.find(v->BoneIndices[n]);
					if (itr != global2localMap.end())
						entry.aVertices[m].BoneIndices[n] = itr->second;
					else
						entry.aVertices[m].BoneIndices[n] = 0;

					ASSERT(entry.aVertices[m].BoneIndices[n] < currentBoneUnit->BoneCount);
				}
			}
		}
		else
		{
			//index buffer
			uint32_t tcount = k - lastIndex;
			currentBoneUnit->StartIndex = set->IStart + lastIndex * 3;
			currentBoneUnit->TCount = tcount;

			currentBoneUnit->BoneCount = 0;
			currentBoneUnit->BoneVStart = boneVerticesOffset;
		}

		boneVerticesCount += (uint32_t)set->BoneUnits.size() * set->VCount;

		if (set->MaxWeights >1 || set->VCount % 4 != 0 || (uint16_t)(set->VCount * 1.5f) != set->ICount)
		{
			set->BillBoard = false;
		}

		if (set->BillBoard)
		{
			uint32_t numRects = set->VCount / 4;
			set->BillboardRects = new SBRect[numRects];
			for (uint32_t r=0; r<numRects; ++r)
			{
				SBRect& rc = set->BillboardRects[r];

				const vector3df& pos0 = m2->GVertices[set->VStart+r*4].Pos;
				const vector3df& pos1 = m2->GVertices[set->VStart+r*4+1].Pos;
				const vector3df& pos2 = m2->GVertices[set->VStart+r*4+2].Pos;
				const vector3df& pos3 = m2->GVertices[set->VStart+r*4+3].Pos;

				rc.boneIndex = m2->AVertices[set->VStart+r*4].BoneIndices[0];
				rc.center = (pos0 + pos1 + pos2 + pos3) * 0.25f;
				
				STexCoord tc[4];
				tc[0].coords = m2->GVertices[set->VStart+r*4].TCoords0;
				tc[1].coords = m2->GVertices[set->VStart+r*4+1].TCoords0;
				tc[2].coords = m2->GVertices[set->VStart+r*4+2].TCoords0;
				tc[3].coords = m2->GVertices[set->VStart+r*4+3].TCoords0;

				heapsort<STexCoord>(tc, 4);

				rc.texcoords[0] = tc[0].coords;
				rc.texcoords[1] = tc[1].coords;
				rc.texcoords[2] = tc[2].coords;
				rc.texcoords[3] = tc[3].coords;

				aabbox3df box(pos0);
				box.addInternalPoint(pos1);
				box.addInternalPoint(pos2);
				box.addInternalPoint(pos3);

				rc.width = (box.getExtent().X + box.getExtent().Z) * 0.5f;
				rc.height = box.getExtent().Y * 0.5f;
			}
		}
	}

	NumBoneVertices = boneVerticesCount;

	AVertices = new SVertex_A[NumBoneVertices];
	
	//每个boneunit顶点复制到总boneVertices
	uint32_t nCount = 0;
	for (T_BoneVerticesList::const_iterator itr=boneVertList.begin(); itr != boneVertList.end(); ++itr)
	{
		SBoneVertEntry entry = (*itr);

		Q_memcpy(&AVertices[nCount], entry.num * sizeof(SVertex_A), entry.aVertices, entry.num * sizeof(SVertex_A));
		nCount += entry.num;
	}

	//释放临时缓存
	while (!boneVertList.empty())
	{
		SBoneVertEntry entry = boneVertList.back();
		Z_FreeTempMemory(entry.aVertices);
		boneVertList.pop_back();
	}

	return true;
}
