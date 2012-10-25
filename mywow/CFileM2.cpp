#include "stdafx.h"
#include "CFileM2.h"
#include "mywow.h"
#include "mpq_libmpq.h"
#include "CLock.h"

extern CRITICAL_SECTION g_m2CS;

IFileM2* CM2Loader::loadM2( MPQFile* file )
{
	CFileM2* m2File = new CFileM2;

	if (!m2File->loadFile(file))
	{
		m2File->drop();	
		return NULL;
	}

	return m2File;
}

CFileM2::CFileM2()
{
	FileData = NULL;

	NumTextures = 0;
}

CFileM2::~CFileM2()
{
	clear();
}

void CFileM2::clear()
{
	delete Skin;

	delete[] ModelCameras;

	delete[] RibbonEmitters;
	delete[] ParticleSystems;
	delete[]	AttachLookup;
	delete[]	Attachments;

	delete[] RenderFlags;
	delete[] BoneLookup;
	delete[] Bones;
	delete[] AnimationLookup;
	delete[] Animations;

	delete[]	TextureAnim;

	delete[]	TransparencyLookup;
	delete[]	Transparencies;
	delete[]	Colors;

	delete[]	GlobalSequences;

	delete[] TexLookup;
	for (u32 i=0; i<NumTextures; ++i)
	{
		if (Textures[i])
			Textures[i]->drop();
	}
	delete[] Textures;
	delete[] TextureTypes;
	delete[] TextureFlags;

	delete[] BoundTris;
	delete[] Bounds;

	delete[] AVertices;
	delete[] TVertices;
	delete[] GVertices;
}


bool CFileM2::loadFile( MPQFile* file )
{
	FileData = file->getBuffer();

	_ASSERT(file->getSize() >= sizeof(M2::header));

	m2Header = (M2::header*)FileData;
	if (m2Header->_magic[0] != 'M' ||				//MD20
		m2Header->_magic[1] != 'D' ||
		m2Header->_magic[2] != '2' ||
		m2Header->_magic[3] != '0')
	{
		_ASSERT(false);
		return false;
	}

	strncpy_s(Name, DEFAULT_SIZE, (const c8*)&FileData[m2Header->_modelNameOffset], m2Header->_modelNameLength);

	getFileDirA(file->getFileName(), Dir, MAX_PATH);
	strcat_s(Dir, MAX_PATH, "\\");

	Type = getM2Type(Dir);				//角色npc

	NumVertices = m2Header->_nVertices;
	NumBoundingVerts = m2Header->_nBoundingVertices;	
	NumBoundingTriangles = m2Header->_nBoundingTriangles;
	NumTextures = m2Header->_nTextures;
	_ASSERT(NumTextures <= MAX_M2_TEXTURES);
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

	if (Type == MT_CHARACTER)
	{
		_ASSERT(NumAnimationLookup > ANIMATION_HANDSCLOSED);
		_ASSERT(NumBoneLookup >= BONE_MAX);
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

	loadParticleSystems();

	loadRibbonEmitters();

	loadModelCameras();

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

	M2::vertex* v = (M2::vertex*)(&FileData[m2Header->_ofsVertices]);
	GVertices = new SGVertex_PN[NumVertices];
	TVertices = new STVertex_1T[NumVertices];
	AVertices = new SAVertex[NumVertices];

	for (u32 i=0; i<NumVertices; ++i)
	{
		GVertices[i].Pos = fixCoordinate(v[i]._Position);
		GVertices[i].Normal = fixCoordinate(v[i]._Normal);
		TVertices[i].TCoords = v[i]._TextureCoords;

		for (u32 j=0; j<4; ++j)
		{
			AVertices[i].Weights[j] = v[i]._BoneWeight[j] / 255.0f;
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
		memcpy_s( BoundTris, sizeof(u16)*NumBoundingTriangles, idx, sizeof(u16) * NumBoundingTriangles);
	}

	CollisionAABBox = aabbox3df(fixCoordinate(m2Header->_vertexBox.MinEdge),fixCoordinate(m2Header->_vertexBox.MaxEdge));
	CollisionRadius = m2Header->_vertexRadius;
	BoundingAABBox = aabbox3df(fixCoordinate(m2Header->_boundingbox.MinEdge),fixCoordinate(m2Header->_boundingbox.MaxEdge));
	BoundingRadius = m2Header->_boundingRadius;
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

		c8 texname[MAX_PATH];
		if (TextureTypes[i] == 0)
		{
			const c8* name = (const c8*)(&FileData[texDef[i]._ofsFilename]);
			u32 len =  texDef[i]._lenFilename;
			_ASSERT(len < MAX_PATH);
			strncpy_s(texname, MAX_PATH, name, len);
		
			Textures[i] = resourceLoader->loadTexture(texname);
			_ASSERT(Textures[i]);	
		}
		else
		{
			Textures[i] = NULL;
			_ASSERT(TextureTypes[i]<= MAX_M2_TEXTURES);
		}
	}

	if (NumTexLookup > 0)
	{
		TexLookup = new s16[NumTexLookup];
		s16* t = (s16*)(&FileData[m2Header->_ofsTexLookup]);
		memcpy_s(TexLookup, sizeof(s16)*NumTexLookup, t, sizeof(s16)*NumTexLookup);
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
		memcpy_s(AttachLookup, sizeof(s16)*NumAttachLookup, p, sizeof(s16)*NumAttachLookup);
	}
}

void CFileM2::loadSequences()
{
	//global sequences
	if (NumGlobalSequences > 0)
	{
		GlobalSequences = new s32[NumGlobalSequences];
		s32* p = (s32*)(&FileData[m2Header->_ofsGlobalSequences]);
		memcpy_s(GlobalSequences, sizeof(s32)*NumGlobalSequences, p, sizeof(s32)*NumGlobalSequences);
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
		memcpy_s(TransparencyLookup, sizeof(s16)*NumTranparencyLookukp, t, sizeof(s16)*NumTranparencyLookukp);
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

	MPQFile** animMpqs = (MPQFile**)Hunk_AllocateTempMemory(NumAnimations * sizeof(MPQFile*));
	SAnimFile* animFiles = (SAnimFile*)Hunk_AllocateTempMemory(NumAnimations * sizeof(SAnimFile));
	memset(animFiles, 0, NumAnimations * sizeof(SAnimFile));

	Animations = new SModelAnimation[NumAnimations];

	for (u32 i=0; i<NumAnimations; ++i)
	{
		M2::animseq* anim = (M2::animseq*)(&FileData[m2Header->_ofsAnimations]);

		Animations[i].animID = anim[i]._AnimationID;
		Animations[i].animSubID	=	anim[i]._SubAnimationID;
		Animations[i].timeLength = anim[i]._Length;
		Animations[i].NextAnimation = anim[i]._NextAnimation;
		Animations[i].Index = anim[i]._Index;

		c8   filename[MAX_PATH];
		sprintf_s(filename, MAX_PATH, "%s%s%04d-%02d.anim", Dir, Name, Animations[i].animID, Animations[i].animSubID);

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
		memcpy_s(AnimationLookup, sizeof(s16)*NumAnimationLookup, a, sizeof(s16)*NumAnimationLookup);
	}

	if (NumBoneLookup > 0)
	{
		BoneLookup = new s16[NumBoneLookup];
		s16* b = (s16*)(&FileData[m2Header->_ofsKeyBoneLookup]);
		memcpy_s(BoneLookup, sizeof(s16)*NumBoneLookup, b, sizeof(s16)*NumBoneLookup);
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

			if(Bones[i].billboard)
				ContainsBillboardBone = true;
		}
	}

	for (s32 i=NumAnimations-1; i>=0; --i)
	{	
		if (animMpqs[i])
			delete (animMpqs[i]);
	}

	Hunk_FreeTempMemory(animFiles);
	Hunk_FreeTempMemory(animMpqs);

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
	c8   filename[MAX_PATH];
	
	sprintf_s(filename, MAX_PATH, "%s%s%02d.skin", Dir, Name, idx);
	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file)
	{
		Skin = new CFileSkin;
		bool success = Skin->loadFile(file, this);
		_ASSERT(success);
		delete file;
		return success;
	}
	
	return true;
}

s16 CFileM2::getAnimationIndex( const c8* name, u32 subIdx /*= 0*/ )
{
	T_AnimationLookup::iterator itr = AnimationNameLookup.find(name);
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

u32 CFileM2::getAnimationCount( const c8* name )
{
	T_AnimationLookup::iterator itr = AnimationNameLookup.find(name);
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
	CLock lock(&g_m2CS);

	if (VideoBuilt)
		return true;

	if (!Skin || Skin->NumGeosets == 0)
		return true;

	//texture
	for (u32 i=0; i<NumTextures; ++i)
	{
		if (Textures[i])
			Textures[i]->createVideoTexture();
	}

	//gvertex buffer
	Skin->GVertexBuffer->set(GVertices, EST_G_PN, NumVertices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->GVertexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Skin->GVertexBuffer,0, NumVertices);

	//tvertex buffer
	Skin->TVertexBuffer->set(TVertices, EST_T_1T, NumVertices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->TVertexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Skin->TVertexBuffer,0, NumVertices);


	//bone buffer
	Skin->AVertexBuffer->set(Skin->AVertices, EST_A, Skin->NumBoneVertices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->AVertexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Skin->AVertexBuffer, 0, Skin->NumBoneVertices);

	//index buffer
	Skin->IndexBuffer->set(Skin->Indices, EIT_16BIT, Skin->NumIndices, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(Skin->IndexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(Skin->IndexBuffer, 0, Skin->NumIndices);


	VideoBuilt = true;
	return true;
}

void CFileM2::releaseVideoResources()
{
	CLock lock(&g_m2CS);

	if (!Skin || Skin->NumGeosets == 0)
		return;

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

	if (Skin->TVertexBuffer)
	{
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(Skin->TVertexBuffer);
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

CFileSkin::CFileSkin()
	: Geosets(NULL), Indices(NULL), AVertices(NULL), 
	NumGeosets(0), NumTexUnit(0), GVertexBuffer(NULL), AVertexBuffer(NULL), IndexBuffer(NULL)
{ 
	GVertexBuffer = new IVertexBuffer(false);
	TVertexBuffer = new IVertexBuffer(false);
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
	delete TVertexBuffer;
	delete GVertexBuffer;
}

bool CFileSkin::loadFile( MPQFile* file, CFileM2* m2)
{
// 	bool nightelfmale = false;			//暗夜精灵男第一个geoset最后12个index为眼睛glow，先去掉，显示默认眼睛
// 	u32 race, gender;
// 	bool isChar = g_Engine->getWowDatabase()->getRaceGender(m2->Name, race, gender);
// 	if (isChar)
// 	{
// 		nightelfmale = (race == RACE_NIGHTELF && gender == 0);
// 	}

	u32 size = file->getSize();
	u8* skinBuffer = file->getBuffer();

	M2::skin_header* skinHeader = (M2::skin_header*)skinBuffer;

	NumGeosets = skinHeader->_nSubmeshes;
	NumTexUnit = skinHeader->_nTextureUnits;
	if (NumGeosets == 0 || NumTexUnit == 0)
		return true;

	//indices
	u16* indexLookup = (u16*)(&skinBuffer[skinHeader->_ofsIndices]);
	u16* triangles = (u16*)(&skinBuffer[skinHeader->_ofsTriangles]);

	NumIndices = skinHeader->_nTriangles;
	_ASSERT(NumIndices > 0);
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

	M2::submesh* m = (M2::submesh*)(&skinBuffer[skinHeader->_ofsSubmeshes]);
	for (u32 i=0; i<NumGeosets; ++i)
	{
		u16 nInds = m[i]._nTriangles;
// 		if (nightelfmale && i==0)				//fix 暗夜精灵眼睛 
// 		{
// 			nInds -= 12;
// 		}

		CGeoset* set = &Geosets[i];
		set->VStart = m[i]._startVertex;
		set->VCount = m[i]._nVertices;
		set->IStart = m[i]._startTriangle;
		set->ICount = nInds;
		set->GeoID = m[i]._ID;	
		set->MaxWeights = m[i]._unknown;

		set->BoneUnits.clear();
		set->BoneUnits.push_back(CBoneUnit());
		CBoneUnit* currentBoneUnit = &set->BoneUnits.back();
		currentBoneUnit->Index = (s8)set->BoneUnits.size() -1;
		u16 lastIndex = 0;
		for (u16 k=0; k<set->ICount; ++k)
		{
			u32 index = Indices[m[i]._startTriangle + k];
			SAVertex* b = &m2->AVertices[index];

			if (k%3 == 0 && currentBoneUnit->BoneIndices.size() > 54)		//54+4 == 58 每个三角形最多受4个顶点影响
			{
				CBoneUnit::T_bone2boneMap global2localMap;

				//index buffer
				u32 icount = k - lastIndex;
				currentBoneUnit->StartIndex = set->IStart + lastIndex;
				lastIndex = k;
				currentBoneUnit->PrimCount = icount / 3;

				//bonemap
				u8 bcount = 0;
				for (CBoneUnit::T_BoneIndices::iterator itr = currentBoneUnit->BoneIndices.begin(); 
					itr!=currentBoneUnit->BoneIndices.end(); ++itr)
				{
					currentBoneUnit->local2globalMap[bcount] =(*itr);
					global2localMap[(*itr)] = bcount;
					++bcount;
				}
				currentBoneUnit->BoneCount = bcount;

				//分配临时内存
				SBoneVertEntry entry;
				entry.num = set->VCount;
				entry.aVertices = (SAVertex*)Hunk_AllocateTempMemory(sizeof(SAVertex) * set->VCount);
				boneVertList.push_back(entry);

				currentBoneUnit->BoneVStart = boneVerticesOffset;
				boneVerticesOffset += set->VCount;

				for (u32 m=0; m<set->VCount; ++m)
				{
					SAVertex* v = &m2->AVertices[set->VStart + m];		
					for (u32 n=0; n<4; ++n)
					{
						entry.aVertices[m].Weights[n] = v->Weights[n];
						entry.aVertices[m].BoneIndices[n] = global2localMap[v->BoneIndices[n]];
						_ASSERT(entry.aVertices[m].BoneIndices[n] < currentBoneUnit->BoneCount);
					}
				}

				//next
				set->BoneUnits.push_back(CBoneUnit());
				currentBoneUnit = &set->BoneUnits.back();
				currentBoneUnit->Index = (s8)set->BoneUnits.size() -1;
				currentBoneUnit->BoneVStart = boneVerticesOffset;
			}

			for (u32 m=0; m<4; ++m)
			{
				u8 idx = b->BoneIndices[m];
				currentBoneUnit->BoneIndices.insert(idx);
			}
		}

		u16 k = set->ICount;
		if (!currentBoneUnit->BoneIndices.empty())
		{
			CBoneUnit::T_bone2boneMap global2localMap;

			//index buffer
			u32 icount = k - lastIndex;
			currentBoneUnit->StartIndex = set->IStart + lastIndex;
			lastIndex = k;
			currentBoneUnit->PrimCount = icount / 3;

			//bonemap
			u8 bcount = 0;
			for (CBoneUnit::T_BoneIndices::iterator itr = currentBoneUnit->BoneIndices.begin(); 
				itr!=currentBoneUnit->BoneIndices.end(); ++itr)
			{
				currentBoneUnit->local2globalMap[bcount] =(*itr);
				global2localMap[(*itr)] = bcount;
				++bcount;
			}
			currentBoneUnit->BoneCount = bcount;

			//分配临时内存
			SBoneVertEntry entry;
			entry.num = set->VCount;
			entry.aVertices = (SAVertex*)Hunk_AllocateTempMemory(sizeof(SAVertex) * set->VCount);
			boneVertList.push_back(entry);

			currentBoneUnit->BoneVStart = boneVerticesOffset;
			boneVerticesOffset += set->VCount;
			for (u32 m=0; m<set->VCount; ++m)
			{
				SAVertex* v = &m2->AVertices[set->VStart + m];
				for (u32 n=0; n<4; ++n)
				{
					entry.aVertices[m].Weights[n] = v->Weights[n];
					entry.aVertices[m].BoneIndices[n] = global2localMap[v->BoneIndices[n]];
					_ASSERT(entry.aVertices[m].BoneIndices[n] < currentBoneUnit->BoneCount);
				}
			}
		}

		boneVerticesCount += set->BoneUnits.size() * set->VCount;
	}

	NumBoneVertices = boneVerticesCount;

	AVertices = new SAVertex[NumBoneVertices];
	
	//每个boneunit顶点复制到总boneVertices
	u32 nCount = 0;
	for (T_BoneVerticesList::iterator itr=boneVertList.begin(); itr != boneVertList.end(); ++itr)
	{
		SBoneVertEntry entry = (*itr);

		memcpy_s(&AVertices[nCount], entry.num * sizeof(SAVertex), entry.aVertices, entry.num * sizeof(SAVertex));
		nCount += entry.num;
	}

	//释放临时缓存
	while (!boneVertList.empty())
	{
		SBoneVertEntry entry = boneVertList.back();
		Hunk_FreeTempMemory(entry.aVertices);
		boneVertList.pop_back();
	}

	return true;
}
