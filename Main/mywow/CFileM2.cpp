#include "stdafx.h"
#include "CFileM2.h"
#include "mywow.h"

CFileM2::CFileM2()
{
	FileData = NULL_PTR;

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

	for (u32 i=0; i<NumTextures; ++i)
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
	const c8* magic = (const c8*)file->getBuffer();
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

	c8 meshName[DEFAULT_SIZE];
	Q_strncpy(meshName, DEFAULT_SIZE, (const c8*)&FileData[m2Header->_modelNameOffset], m2Header->_modelNameLength);
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

	u32 race, gender;
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

	FileData += (u32)((u8*)m2Header - (u8*)m2HeaderEx);

	c8 meshName[DEFAULT_SIZE];
	Q_strncpy(meshName, DEFAULT_SIZE, (const c8*)&FileData[m2Header->_modelNameOffset], m2Header->_modelNameLength);
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

	u32 race, gender;
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

	for (u32 i=0; i<NumVertices; ++i)
	{
		GVertices[i].Pos = fixCoordinate(v[i]._Position);
		GVertices[i].Normal = fixCoordinate(v[i]._Normal);
		GVertices[i].TCoords0 = v[i]._TextureCoords0;
		GVertices[i].TCoords1 = v[i]._TextureCoords1;
		for (u32 j=0; j<4; ++j)
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
		for (u32 i=0; i<NumBoundingVerts; ++i)
		{
			Bounds[i] = fixCoordinate(b[i]);
		}
	}

	if (NumBoundingTriangles > 0)
	{
		BoundTris = new u16[NumBoundingTriangles];
		u16* idx = (u16*)(&FileData[m2Header->_ofsBoundingTriangles]);
		Q_memcpy( BoundTris, sizeof(u16)*NumBoundingTriangles, idx, sizeof(u16) * NumBoundingTriangles);
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

	TextureFlags = new u32[NumTextures];
	TextureTypes = new ETextureTypes[NumTextures];
	Textures = new ITexture*[NumTextures];

	IResourceLoader* resourceLoader = g_Engine->getResourceLoader();

	M2::texture* texDef = (M2::texture*)(&FileData[m2Header->_ofsTextures]);

	for (u32 i=0; i<NumTextures; ++i)
	{
		TextureFlags[i] = texDef[i]._flags;
		TextureTypes[i] = (ETextureTypes)texDef[i]._type;

		if (TextureTypes[i] == 0)
		{
			const c8* name = (const c8*)(&FileData[texDef[i]._ofsFilename]);
			u32 len =  texDef[i]._lenFilename;
			ASSERT(len < QMAX_PATH);
			string256 texname(name);
			Textures[i] = resourceLoader->loadTexture(texname.c_str());
			ASSERT(Textures[i]);	
		}
		else
		{
			Textures[i] = NULL_PTR;
		}
	}

	if (NumTexLookup > 0)
	{
		TexLookup = new s16[NumTexLookup];
		s16* t = (s16*)(&FileData[m2Header->_ofsTexLookup]);
		Q_memcpy(TexLookup, sizeof(s16)*NumTexLookup, t, sizeof(s16)*NumTexLookup);
	}
}

void CFileM2::loadAttachments()
{
	//attachment
	if (NumAttachments > 0)
	{
		Attachments = new SModelAttachment[NumAttachments];
		M2::attach* att = (M2::attach*)(&FileData[m2Header->_ofsAttachments]);
		for (u32 i=0; i<NumAttachments; ++i)
		{
			Attachments[i].id = att[i]._Id;
			Attachments[i].position = fixCoordinate(att[i]._Position);
 			Attachments[i].boneIndex = (att[i]._Bone >= 0 && att[i]._Bone < (s32)NumBones) ? 
 				att[i]._Bone : -1;
		}
	}

	if (NumAttachLookup > 0)
	{
		AttachLookup = new s16[NumAttachLookup];
		s16* p = (s16*)(&FileData[m2Header->_ofsAttachLookup]);
		Q_memcpy(AttachLookup, sizeof(s16)*NumAttachLookup, p, sizeof(s16)*NumAttachLookup);
	}
}

void CFileM2::loadSequences()
{
	//global sequences
	if (NumGlobalSequences > 0)
	{
		GlobalSequences = new s32[NumGlobalSequences];
		s32* p = (s32*)(&FileData[m2Header->_ofsGlobalSequences]);
		Q_memcpy(GlobalSequences, sizeof(s32)*NumGlobalSequences, p, sizeof(s32)*NumGlobalSequences);
	}
}

void CFileM2::loadColor()
{
	if (NumColors > 0)
	{
		Colors = new SModelColor[NumColors];
		M2::colorDef* c = (M2::colorDef*)(&FileData[m2Header->_ofsColors]);
		for (u32 i=0; i<NumColors; ++i)
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
		for (u32 i=0; i<NumTransparencies; ++i)
			Transparencies[i].init(FileData, GlobalSequences, NumGlobalSequences, t[i]);
	}

	//transparency lookup
	if (NumTranparencyLookukp > 0)
	{
		TransparencyLookup = new s16[NumTranparencyLookukp];
		s16* t = (s16*)(&FileData[m2Header->_ofsTransLookup]);
		Q_memcpy(TransparencyLookup, sizeof(s16)*NumTranparencyLookukp, t, sizeof(s16)*NumTranparencyLookukp);
	}
}

void CFileM2::loadTextureAnimation()
{
	if (NumTexAnim > 0)
	{
		TextureAnim = new SModelTextureAnim[NumTexAnim];
		M2::texanimDef* t = (M2::texanimDef*)(&FileData[m2Header->_ofsTextureanimations]);
		for (u32 i=0; i<NumTexAnim; ++i)
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

	for (u32 i=0; i<NumAnimations; ++i)
	{
		M2::animseq* anim = (M2::animseq*)(&FileData[m2Header->_ofsAnimations]);

		Animations[i].animID = anim[i]._AnimationID;
		Animations[i].animSubID	=	anim[i]._SubAnimationID;
		Animations[i].timeLength = anim[i]._Length;
		Animations[i].NextAnimation = anim[i]._NextAnimation;
		Animations[i].Index = anim[i]._Index;

// 		const c8* animName = g_Engine->getWowDatabase()->getAnimationName(Animations[i].animID);
// 		if (strcmp(animName, "Stand") != 0)
// 		{
// 			animMpqs[i] = NULL_PTR;
// 			continue;
// 		}

		c8   filename[QMAX_PATH];
		Q_sprintf(filename, QMAX_PATH, "%s%s%04d-%02d.anim", Dir, Name, Animations[i].animID, Animations[i].animSubID);

		animMpqs[i] = g_Engine->getWowEnvironment()->openFile(filename);
		if (animMpqs[i])
		{
			animFiles[i].data = animMpqs[i]->getBuffer();
			animFiles[i].size = animMpqs[i]->getSize();
		}
	}

	//动画
	for (u32 i=0; i<NumAnimations; ++i)
	{
		if (Animations[i].animSubID != 0)
			continue;

		const c8* animName = g_Engine->getWowDatabase()->getAnimationName(Animations[i].animID);
		AnimationNameLookup[animName] = Animations[i].Index;
	}

	if (NumAnimationLookup > 0)
	{
		AnimationLookup = new s16[NumAnimationLookup];
		s16* a = (s16*)(&FileData[m2Header->_ofsAnimationLookup]);
		Q_memcpy(AnimationLookup, sizeof(s16)*NumAnimationLookup, a, sizeof(s16)*NumAnimationLookup);
	}

	if (NumBoneLookup > 0)
	{
		BoneLookup = new s16[NumBoneLookup];
		s16* b = (s16*)(&FileData[m2Header->_ofsKeyBoneLookup]);
		Q_memcpy(BoneLookup, sizeof(s16)*NumBoneLookup, b, sizeof(s16)*NumBoneLookup);
	}

	if (NumBones > 0)
	{
		Bones = new SModelBone[NumBones];
		M2::bone* b = (M2::bone*)(&FileData[m2Header->_ofsBones]);
		for (u32 i=0; i<NumBones; ++i)
		{
			Bones[i].init(FileData, GlobalSequences, NumGlobalSequences, b[i], animFiles);

			if (Type == MT_CHARACTER)
				setBoneType((s16)i);
		}
	}

	for (s32 i=NumAnimations-1; i>=0; --i)
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

		for (u32 i=0; i<NumRenderFlags; ++i)
		{		
			//raw
			RenderFlags[i].flags = rfs[i].flags;
			RenderFlags[i].blend = rfs[i].blend;

			RenderFlags[i].lighting = (rfs[i].flags & RENDERFLAGS_UNLIT) == 0;
			RenderFlags[i].zwrite = (rfs[i].flags & RENDERFLAGS_UNZWRITE) == 0;
			RenderFlags[i].frontCulling = (rfs[i].flags & RENDERFLAGS_TWOSIDED) == 0;
			RenderFlags[i].invisible = (rfs[i].flags & 256) != 0;
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
		for (u32 i=0; i<NumParticleSystems; ++i)
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
		for (u32 i=0; i<NumParticleSystems; ++i)
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
	for(u32 i=0; i<NumRibbonEmitters; ++i)
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
		for (u32 i=0; i<NumModelCameras; ++i)
		{
			ModelCameras[i].init(cdefs[i], FileData, GlobalSequences);
		}
	}
}

bool CFileM2::loadSkin(u32 idx)
{
	c8   filename[QMAX_PATH];
	
	Q_sprintf(filename, QMAX_PATH, "%s%s%02d.skin", Dir, Name, (s32)idx);
	if(!g_Engine->getWowEnvironment()->exists(filename))
		Q_sprintf(filename, QMAX_PATH, "%s%s%02d.skin", Dir, FileName, (s32)idx);

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

s16 CFileM2::getAnimationIndex( const c8* name, u32 subIdx /*= 0*/ ) const
{
	T_AnimationLookup::const_iterator itr = AnimationNameLookup.find(name);
	if (itr == AnimationNameLookup.end())
		return -1;

	s16 next = itr->second;
	while (next != -1)
	{
		if (Animations[next].animSubID == subIdx)
			break;
		next = Animations[next].NextAnimation;
	}
	return next;
}

u32 CFileM2::getAnimationCount( const c8* name ) const
{
	T_AnimationLookup::const_iterator itr = AnimationNameLookup.find(name);
	if (itr == AnimationNameLookup.end())
		return 0;

	s16 next = itr->second;
	u32 count = 0;
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
	for (u32 i=0; i<NumTextures; ++i)
	{
		if (Textures[i])
			Textures[i]->createVideoTexture();
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

	for (u32 i=0; i<NumTextures; ++i)
	{
		if (Textures[i] && Textures[i]->getReferenceCount()==2)		//refcount==2说明此时纹理只被当前m2文件使用，可以release
			Textures[i]->releaseVideoTexture();
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

void CFileM2::setBoneType( s16 boneIdx )
{
	for (u32 i=0; i<5; ++i)
	{
		s16 fistIndex = BoneLookup[BONE_LFINGER1 + i];
		if (fistIndex == boneIdx)
		{
	 		Bones[boneIdx].bonetype = EBT_LEFTHAND;
	 		return;
		}
	}

	for (u32 i=0; i<5; ++i)
	{
		s16 fistIndex = BoneLookup[BONE_RFINGER1 + i];
		if (fistIndex == boneIdx)
		{
			Bones[boneIdx].bonetype = EBT_RIGHTHAND;
			return;
		}
	}

	s32 parent = Bones[boneIdx].parent;
	while(parent != -1)
	{
		for (u32 i=0; i<5; ++i)
		{
			s16 fistIndex = BoneLookup[BONE_LFINGER1 + i];
			if (fistIndex == parent)
			{
				Bones[boneIdx].bonetype = EBT_LEFTHAND;
				return;
			}
		}

		for (u32 i=0; i<5; ++i)
		{
			s16 fistIndex = BoneLookup[BONE_RFINGER1 + i];
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

wow_m2Action* CFileM2::getAction( const c8* name ) const
{
	T_ActionMap::const_iterator itr = ActionMap.find(name);
	if (itr != ActionMap.end())
	{
		return itr->second;
	}
	return NULL_PTR;
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

u32 CFileM2::getSkinIndex(u32 race, u32 gender, bool isHD)
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
	: Geosets(NULL_PTR), Indices(NULL_PTR), AVertices(NULL_PTR), NumIndices(0), NumBoneVertices(0),
	NumGeosets(0), NumTexUnit(0), GVertexBuffer(NULL_PTR), AVertexBuffer(NULL_PTR), IndexBuffer(NULL_PTR)
{ 
	GVertexBuffer = new IVertexBuffer(false);
	AVertexBuffer = new IVertexBuffer(false);
	IndexBuffer = new IIndexBuffer(false);
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
	u8* skinBuffer = file->getBuffer();

	M2::skin_header* skinHeader = (M2::skin_header*)skinBuffer;

	NumGeosets = skinHeader->_nSubmeshes;
	NumTexUnit = skinHeader->_nTextureUnits;
	NumIndices = skinHeader->_nTriangles;
	if (NumGeosets == 0 || NumTexUnit == 0 || NumIndices == 0)
		return true;

	//indices
	u16* indexLookup = (u16*)(&skinBuffer[skinHeader->_ofsIndices]);
	u16* triangles = (u16*)(&skinBuffer[skinHeader->_ofsTriangles]);

	Indices= new u16[NumIndices];
	for (u32 i=0; i<NumIndices; ++i)
	{
		Indices[i] = indexLookup[triangles[i]];
	}

	//geosets
	Geosets = new CGeoset[NumGeosets];

	//texture unit
	u16* texanimlookup = (u16*)(m2->getFileData() + m2->m2Header->_ofsTexAnimLookup);
	u32 numtexanimlookup = m2->m2Header->_nTexAnimLookup;

	M2::textureUnit* t = (M2::textureUnit*)(&skinBuffer[skinHeader->_ofsTextureUnits]);
	for (u32 i=0; i<NumTexUnit; ++i)
	{
		CGeoset* geo = &Geosets[t[i]._submeshIdx];
		STexUnit texUnit;

		texUnit.Mode = t[i]._mode;
		texUnit.Shading = t[i]._shading;
		texUnit.TexID = (t[i]._textureIdx >= 0 && t[i]._textureIdx < (s16)m2->NumTexLookup) ? 
			m2->TexLookup[t[i]._textureIdx] : -1;

		texUnit.rfIndex = (t[i]._renderFlagsIdx >= 0 && t[i]._renderFlagsIdx < (s16)m2->NumRenderFlags) ? 
			t[i]._renderFlagsIdx : -1;
		texUnit.ColorIndex = (t[i]._colorIdx >= 0 && t[i]._colorIdx < (s16)m2->NumColors) ?
			t[i]._colorIdx : -1;

		texUnit.TransIndex = (t[i]._transparencyIdx >= 0 && t[i]._transparencyIdx < (s16)m2->NumTranparencyLookukp) ? 
			m2->TransparencyLookup[t[i]._transparencyIdx] : -1;
		texUnit.TexAnimIndex = (t[i]._animationIdx >= 0 && t[i]._animationIdx < (s16)numtexanimlookup) ?
			texanimlookup[t[i]._animationIdx] : -1;
		texUnit.TexFlags = texUnit.TexID == -1 ? 0 : m2->TextureFlags[texUnit.TexID];
		texUnit.WrapX = texUnit.TexAnimIndex == -1 && (texUnit.TexFlags & TEXTURE_WRAPX) == 0;
		texUnit.WrapY = texUnit.TexAnimIndex == -1 && (texUnit.TexFlags & TEXTURE_WRAPY) == 0;

		geo->TexUnits.push_back(texUnit);
	}

	u32	boneVerticesOffset = 0;
	u32  boneVerticesCount = 0;
	T_BoneVerticesList	boneVertList;
	CBoneUnit::T_BoneIndices triangleBoneIndices;

	//bool isHD = m2->isHD();
	M2::submesh* sm = (M2::submesh*)(&skinBuffer[skinHeader->_ofsSubmeshes]);
	for (u32 i=0; i<NumGeosets; ++i)
	{
		u16 nInds = sm[i]._nTriangles;

		CGeoset* set = &Geosets[i];
		set->VStart = sm[i]._startVertex;
		set->VCount = sm[i]._nVertices;
		set->IStart = sm[i]._startTriangle;
		set->ICount = nInds;
		set->GeoID = sm[i]._ID;	
		set->MaxWeights = 0;
		set->BillBoard = true;

		set->BoneUnits.clear();
		set->BoneUnits.emplace_back(CBoneUnit());
		CBoneUnit* currentBoneUnit = &set->BoneUnits.back();
		currentBoneUnit->Index = (s8)set->BoneUnits.size() -1;
		
		u16 lastIndex = 0;
		for (u16 k=0; k<set->ICount/3; ++k)
		{
			u16 index0 = Indices[sm[i]._startTriangle + 3*k];
			u16 index1 = Indices[sm[i]._startTriangle + 3*k + 1];
			u16 index2 = Indices[sm[i]._startTriangle + 3*k + 2];

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
			for (u32 n=0; n<3; ++n)
			{
				u32 weights = 0;
				for (u16 m=0; m<MAX_BONE_BLEND; ++m)
				{
					if (g[n]->Weights[m] == 0)
						continue;

					weights += g[n]->Weights[m];

					u8 idx = b[n]->BoneIndices[m];
					if (idx >= m2->NumBones)
						continue;

					if (!m2->Bones[idx].billboard)
						set->BillBoard = false;

					triangleBoneIndices.insert(idx);

					if (weights >= 255)
					{
						set->MaxWeights = max_(set->MaxWeights, (u16)(m+1));
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
				u16 tcount = k - lastIndex;
				currentBoneUnit->StartIndex = set->IStart + lastIndex * 3;
				lastIndex = k;
				currentBoneUnit->TCount = tcount;

				//bonemap
				currentBoneUnit->local2globalMap.clear();
				for (CBoneUnit::T_BoneIndices::const_iterator itr = currentBoneUnit->BoneIndices.begin(); 
					itr!=currentBoneUnit->BoneIndices.end(); ++itr)
				{
					u8 boneIndex = *itr;

					if(boneIndex >= m2->NumBones)
						continue;

					currentBoneUnit->local2globalMap.push_back(boneIndex);
					global2localMap[boneIndex] = (u8)currentBoneUnit->local2globalMap.size() - 1;
				}
				currentBoneUnit->BoneCount = (u8)currentBoneUnit->local2globalMap.size();

				//分配临时内存
				SBoneVertEntry entry;
				entry.num = set->VCount;
				entry.aVertices = (SVertex_A*)Z_AllocateTempMemory(sizeof(SVertex_A) * set->VCount);
				boneVertList.push_back(entry);

				currentBoneUnit->BoneVStart = boneVerticesOffset;
				boneVerticesOffset += set->VCount;

				for (u32 m=0; m<set->VCount; ++m)
				{
					SVertex_A* v = &m2->AVertices[set->VStart + m];		
					for (u32 n=0; n<MAX_BONE_BLEND; ++n)
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
				currentBoneUnit->Index = (s8)set->BoneUnits.size() -1;
				currentBoneUnit->BoneVStart = boneVerticesOffset;
			}		
		}

		u16 k = set->ICount / 3;
		if (!currentBoneUnit->BoneIndices.empty())
		{
			CBoneUnit::T_bone2boneMap global2localMap;

			//index buffer
			u32 tcount = k - lastIndex;
			currentBoneUnit->StartIndex = set->IStart + lastIndex * 3;
			currentBoneUnit->TCount = tcount;

			//bonemap
			currentBoneUnit->local2globalMap.clear();
			for (CBoneUnit::T_BoneIndices::const_iterator itr = currentBoneUnit->BoneIndices.begin(); 
				itr!=currentBoneUnit->BoneIndices.end(); ++itr)
			{
				u8 boneIndex = *itr;

				if(boneIndex >= m2->NumBones)
					continue;

				currentBoneUnit->local2globalMap.push_back(boneIndex);
				global2localMap[boneIndex] = (u8)currentBoneUnit->local2globalMap.size() - 1;
			}
			currentBoneUnit->BoneCount = (u8)currentBoneUnit->local2globalMap.size();

			//分配临时内存
			SBoneVertEntry entry;
			entry.num = set->VCount;
			entry.aVertices = (SVertex_A*)Z_AllocateTempMemory(sizeof(SVertex_A) * set->VCount);
			boneVertList.emplace_back(entry);

			currentBoneUnit->BoneVStart = boneVerticesOffset;
			boneVerticesOffset += set->VCount;
			for (u32 m=0; m<set->VCount; ++m)
			{
				SVertex_A* v = &m2->AVertices[set->VStart + m];
				for (u32 n=0; n<MAX_BONE_BLEND; ++n)
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
			u32 tcount = k - lastIndex;
			currentBoneUnit->StartIndex = set->IStart + lastIndex * 3;
			currentBoneUnit->TCount = tcount;

			currentBoneUnit->BoneCount = 0;
			currentBoneUnit->BoneVStart = boneVerticesOffset;
		}

		boneVerticesCount += (u32)set->BoneUnits.size() * set->VCount;

		if (set->MaxWeights >1 || set->VCount % 4 != 0 || (u16)(set->VCount * 1.5f) != set->ICount)
		{
			set->BillBoard = false;
		}

		if (set->BillBoard)
		{
			u32 numRects = set->VCount / 4;
			set->BillboardRects = new SBRect[numRects];
			for (u32 r=0; r<numRects; ++r)
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
	u32 nCount = 0;
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
