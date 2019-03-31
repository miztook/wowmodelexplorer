#pragma once

#include "IFileM2.h"
#include <list>
#include <unordered_map>
#include <map>

class CVertexBuffer;
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
		uint32_t		num;
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

	static const uint32_t MAX_BONE_BLEND = 4;

public:
	uint32_t		NumGeosets;
	uint32_t		NumTexUnit;
	uint32_t		NumIndices;
	uint32_t		NumBoneVertices;

	CGeoset*		Geosets;
	uint16_t*		Indices;
	SVertex_A*		AVertices;
	
	//vm
	CVertexBuffer*		GVertexBuffer;
	CVertexBuffer*		AVertexBuffer;
	CIndexBuffer*		IndexBuffer;
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
	virtual wow_m2Action* getAction(const char* name) const;

	const aabbox3df& getBoundingBox() const { return BoundingBox; }
	int16_t getAnimationIndex(const char* name, uint32_t subIdx = 0) const;
	uint32_t getAnimationCount(const char* name) const;

	uint8_t* getFileData() const { return FileData; }

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

	bool loadSkin(uint32_t idx);

	void setBoneType(int16_t boneIdx);

	uint32_t getSkinIndex(uint32_t race, uint32_t gender, bool isHD);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<string64, int16_t, std::less<string64>, qzone_allocator<std::pair<string64, int16_t>>> T_AnimationLookup;
	typedef std::map<string64, wow_m2Action*, std::less<string64>, qzone_allocator<std::pair<string64, wow_m2Action*>>> T_ActionMap;
#else
	typedef std::unordered_map<string64, int16_t, string64::string_hash> T_AnimationLookup;
	typedef std::unordered_map<string64, wow_m2Action*, string64::string_hash> T_ActionMap;
#endif

private:
	uint8_t*			FileData;

	T_AnimationLookup	AnimationNameLookup;

	//actions
	T_ActionMap	ActionMap;

	aabbox3df		BoundingBox;
	M2Type		Type;
	bool		IsCharacter;
	bool		IsHD;
};

