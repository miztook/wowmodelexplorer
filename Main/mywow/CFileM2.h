#pragma once

#include "IFileM2.h"
#include <list>
#include <unordered_map>

class IVertexBuffer;
class CFileM2;

class CFileSkin
{
private:
	DISALLOW_COPY_AND_ASSIGN(CFileSkin);

public:
	CFileSkin();
	~CFileSkin();

	bool loadFile(IMemFile* file, CFileM2* m2);

public:
	//ËÑ¼¯¹Ç÷À¶¥µã
	struct SBoneVertEntry
	{
		SVertex_A*		aVertices;
		u32		num;
	};

	struct STexCoord
	{
		vector2df coords;
		bool operator<(const STexCoord& other) const
		{
			if (coords.Y != other.coords.Y)
				return coords.Y < other.coords.Y;
			else
				return coords.X < other.coords.X;
		}
	};

	typedef std::list<SBoneVertEntry, qzone_allocator<SBoneVertEntry> >		T_BoneVerticesList;

	static const u32 MAX_BONE_BLEND = 4;

public:
	u32		NumGeosets;
	u32		NumTexUnit;
	u32		NumIndices;
	u32		NumBoneVertices;

	CGeoset*		Geosets;
	u16*		Indices;
	SVertex_A*		AVertices;
	
	//vm
	IVertexBuffer*		GVertexBuffer;
	IVertexBuffer*		AVertexBuffer;
	IIndexBuffer*		IndexBuffer;
};

class CFileM2 : public IFileM2
{
private:
	DISALLOW_COPY_AND_ASSIGN(CFileM2);

	CFileM2();
	~CFileM2();

	friend class CM2Loader;

public:
	virtual bool loadFile(IMemFile* file);
	virtual M2Type getType() const { return Type; }
	virtual bool isCharacter() const { return IsCharacter; }
	virtual bool isHD() const { return IsHD; }

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

	virtual void clearAllActions();
	virtual bool addAction(wow_m2Action* action);
	virtual wow_m2Action* getAction(const c8* name) const;

	const aabbox3df& getBoundingBox() const { return BoundingBox; }
	s16 getAnimationIndex(const c8* name, u32 subIdx = 0) const;
	u32 getAnimationCount(const c8* name) const;

	u8* getFileData() const { return FileData; }

private:
	bool loadFileMD20(IMemFile* file);
	bool loadFileMD21(IMemFile* file);

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

	u32 getSkinIndex(u32 race, u32 gender, bool isHD);

private:
	u8*			FileData;

	typedef std::unordered_map<string64, s16, string64::string_hash> T_AnimationLookup;
	T_AnimationLookup	AnimationNameLookup;

	//actions
	typedef std::unordered_map<string64, wow_m2Action*, string64::string_hash> T_ActionMap;
	T_ActionMap	ActionMap;

	aabbox3df		BoundingBox;
	M2Type		Type;
	bool		IsCharacter;
	bool		IsHD;
};

