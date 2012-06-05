#pragma once

#include "IFileM2.h"
#include <list>

class IVertexBuffer;

class CM2Loader
{
public:
	bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "m2"); }

	IFileM2* loadM2( MPQFile* file );
};

class CFileSkin
{
public:
	CFileSkin();
	~CFileSkin();

	bool loadFile(MPQFile* file, IFileM2* m2);

	u32 NumGeosets;
	u32 NumTexUnit;
	CGeoset*		Geosets;

	u16*	Indices;
	u32		NumIndices;
	S3DVertexBone*	BoneVertices;
	u32		NumBoneVertices;

	//vm
	IIndexBuffer*		IndexBuffer;
	IVertexBuffer*		BoneBuffer;
	IVertexBuffer*		VertexBuffer;

	//ËÑ¼¯¹Ç÷À¶¥µã
	struct SBoneVertEntry
	{
		S3DVertexBone*	vertices;
		u32		num;
	};
	typedef std::list<SBoneVertEntry, qzone_allocator<SBoneVertEntry>>		T_BoneVerticesList;
};

class CFileM2 : public IFileM2
{
public:
	CFileM2();
	~CFileM2();
public:
	virtual bool loadFile(MPQFile* file);
	virtual u8* getFileData() const { return FileData; }
	virtual M2Type getType() const { return Type; }
	virtual const aabbox3df& getBoundingBox() const { return BoundingBox; }
	virtual s16 getAnimationIndex(const c8* name, u32 subIdx = 0);
	virtual u32 getAnimationCount(const c8* name);

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

private:
	void clear();

	void loadVertices();
	void loadBounds();
	void loadTextures();
	void loadAttachments();
	void loadSequences();
	void loadColor();
	void loadTransparency();
	void loadTextureAnimation();
	void loadBones();
	void loadRenderFlags();
	void loadParticleSystems();
	void loadRibbonEmitters();
	void loadModelCameras();

	bool loadSkin(u32 idx);

	void setBoneType(s16 boneIdx);

private:
	u8*			FileData;

	aabbox3df		BoundingBox;

	typedef std::map<string64, s16, std::less<string64>, qzone_allocator<std::pair<string64, s16>>> T_AnimationLookup;
	T_AnimationLookup	AnimationNameLookup;

	M2Type		Type;
};

