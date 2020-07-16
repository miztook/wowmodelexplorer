#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_enums.h"
#include "wow_animation.h"
#include "wow_particle.h"
#include "wow_m2_structs.h"
#include "S3DVertex.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class IMemFile;
class ITexture;
class wow_m2Action;

enum E_M2_VER
{
	MD20 = 0,
	MD21,
};

inline vector3df fixCoordinate(const vector3df& v)
{
	return vector3df(v.X, v.Z, v.Y);
}

inline vector3df fixCoordinate2(const vector3df& v)
{
	return vector3df(v.X, v.Z, -v.Y);
}

inline quaternion fixQuat(const quaternion& q)
{
	return quaternion(q.X, q.Z, q.Y, q.W);
}

#define	ANIMATION_HANDSCLOSED	15

struct SModelAnimation
{
	uint32_t	animID;
	uint32_t  animSubID;
	uint32_t	timeLength;

	int16_t	NextAnimation;
	int16_t	Index;
};

#define	RENDERFLAGS_UNLIT		1
#define	RENDERFLAGS_TWOSIDED	4
#define	RENDERFLAGS_BILLBOARD	8
#define	RENDERFLAGS_UNZWRITE		16
#define RENDERFLAGS_ADDCOLOR		256

enum E_BONE_TYPE
{
	EBT_NONE = 0,
	EBT_ROOT,
	EBT_LEFTHAND,
	EBT_RIGHTHAND,

	EBT_COUNT
};

struct SModelBone
{
	SWowAnimationVec3		trans;
	SWowAnimationQuat		rot;
	SWowAnimationVec3		scale;

	vector3df	pivot;
	int32_t		parent;
	uint32_t		flags;
	int16_t		geosetId;
	E_BONE_TYPE  bonetype;			//from bone lookup table
	bool		billboard;

	void		init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::bone& b, SAnimFile* animFiles)
	{
		parent = b._ParentBone;
		billboard = (b._Flags & 8) != 0;
		flags = b._Flags;
		geosetId = b._GeosetID;

		pivot = fixCoordinate(b._PivotPoint);
		trans.init(&b._Translation, filedata, animFiles, globalSeq, numGlobalSeq);
		rot.init(&b._Rotation, filedata, animFiles, globalSeq, numGlobalSeq);
		scale.init(&b._Scaling, filedata, animFiles, globalSeq, numGlobalSeq);
	}
};

struct SModelColor
{
	SWowAnimation<vector3df>	colorAnim;
	SWowAnimationShort		opacityAnim;

	void init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::colorDef& cd)
	{
		colorAnim.init(&cd.color, filedata, globalSeq, numGlobalSeq);
		opacityAnim.init(&cd.opacity, filedata, globalSeq, numGlobalSeq);
	}
};

struct SModelTransparency
{
	SWowAnimation<uint16_t>	tranAnim;

	void init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::transDef& td)
	{
		tranAnim.init(&td.trans, filedata, globalSeq, numGlobalSeq);
	}
};

struct SModelTextureAnim
{
	SWowAnimation<vector3df>	trans, rot, scale;

	void init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::texanimDef& t)
	{
		trans.init(&t.trans, filedata, globalSeq, numGlobalSeq);
		rot.init(&t.rot, filedata, globalSeq, numGlobalSeq);
		scale.init(&t.scale, filedata, globalSeq, numGlobalSeq);
	}
};

struct SModelAttachment			//挂点
{
	SModelAttachment() : id(0), boneIndex(-1) {}
	int32_t id;
	int32_t boneIndex;
	vector3df position;
};

struct SModelCamera
{
	vector3df	position, target;
	float	nearclip, farclip;
 //	SWowAnimationVec3		positionAnim, targetAnim;
 //	SWowAnimation<float>		rotationAnim;
//	SWowAnimation<float>		fovAnim;
	void init(const M2::ModelCameraDefV10& mcd, const uint8_t* filedata, int32_t* globalSeq)
	{
		position = fixCoordinate(mcd.pos);
		target = fixCoordinate(mcd.target);
		nearclip = mcd.nearclip;
		farclip = mcd.farclip;
//  		positionAnim.init(&mcd.transPos, filedata, globalSeq);
//  		targetAnim.init(&mcd.transTarget, filedata, globalSeq);
//  		rotationAnim.init(&mcd.rot, filedata, globalSeq);
//		fovAnim.init(&mcd.fov, filedata, globalSeq);
	}
};

class CIndexBuffer;

class CBoneUnit
{
public:
	CBoneUnit() : BoneVStart(0), TCount(0),  StartIndex(0), BoneCount(0), Index(-1) { }

	uint32_t		BoneVStart;				//在skin bone顶点中的偏移
	uint16_t		TCount;
	uint16_t		StartIndex;				//在skin 中index偏移
	uint8_t	BoneCount;
	int8_t		Index;

	typedef std::unordered_set<uint8_t> T_BoneIndices;
	T_BoneIndices			BoneIndices;

	typedef std::unordered_map<uint8_t, uint8_t>	T_bone2boneMap;	 
	std::vector<uint8_t>	local2globalMap;					//local -> global
};

struct STexUnit
{
	int16_t	TexID;				//使用纹理编号
	int16_t	rfIndex;				//render flag索引
	int16_t	ColorIndex;
	int16_t  TransIndex;
	int16_t	TexAnimIndex;
	uint16_t	Mode;
	uint16_t	Shading;		//shader?
	uint32_t  TexFlags;

	bool WrapX() const { return TexAnimIndex == -1 && (TexFlags & TEXTURE_WRAPX) == 0; }
	bool WrapY() const { return TexAnimIndex == -1 && (TexFlags & TEXTURE_WRAPY) == 0; }
};

struct SBRect
{
	vector3df center;
	vector2df texcoords[4];
	float width;
	float height;
	uint16_t boneIndex;
};

class	CGeoset 
{	
private:
	DISALLOW_COPY_AND_ASSIGN(CGeoset);

public:
	CGeoset() : GeoID(0), BillboardRects(nullptr), VStart(0), VCount(0), IStart(0), 
		ICount(0), MaxWeights(4), 
		BillBoard(true) {}

	~CGeoset()
	{
		delete[] BillboardRects;
	}

	uint32_t		GeoID;	
	
	//tex unit
	typedef std::vector<STexUnit> T_TexUnits;
	T_TexUnits		TexUnits;

	//bone unit
	typedef std::vector<CBoneUnit> T_BoneUnits;
	T_BoneUnits		BoneUnits;

	SBRect*		BillboardRects;

	uint16_t		VStart;
	uint16_t		VCount;
	uint16_t		IStart;
	uint16_t		ICount;
	uint16_t		MaxWeights;
	bool		BillBoard;

	uint32_t getTexUnitCount() const { return (uint32_t)TexUnits.size(); }
	const STexUnit* getTexUnit(uint8_t index) const 
	{
		if (index >= TexUnits.size())
			return nullptr;
		return &TexUnits[index];
	}
};

class CFileSkin;

//对应m2文件，静态数据，加载m2同时也加载对应的skin和anim文件
class IFileM2 : public IReferenceCounted<IFileM2>
{
protected:
	virtual void onRemove() 
	{
		releaseVideoResources(); 
	}

	virtual ~IFileM2()  { }

public:
	IFileM2()
		: VideoBuilt(false)
	{
		M2Version = MD20;
		m2Header = nullptr;
		m2HeaderEx = nullptr;
		GVertices = nullptr;
		AVertices = nullptr;
		Bounds = nullptr;
		BoundTris = nullptr;
		TextureFlags = nullptr;
		TextureTypes = nullptr;
		Textures = nullptr;
		TexLookup = nullptr;
		Attachments = nullptr;
		AttachLookup = nullptr;
		GlobalSequences = nullptr;
		Colors = nullptr;
		Transparencies = nullptr;
		TransparencyLookup = nullptr;
		TextureAnim = nullptr;
		Bones = nullptr;
		BoneLookup = nullptr;
		Animations = nullptr;
		AnimationLookup = nullptr;
		RenderFlags = nullptr;
		Skin = nullptr;
		ParticleSystems = nullptr;
		RibbonEmitters = nullptr;
		ModelCameras = nullptr;

		NumVertices =
		NumBoundingTriangles =
		NumBoundingVerts =
		NumTextures =
		NumTexLookup =
		NumAttachments =
		NumAttachLookup =
		NumBones = 
		NumBoneLookup =
		NumAnimations =
		NumAnimationLookup =
		NumGlobalSequences =
		NumColors =
		NumTransparencies =
		NumTranparencyLookukp =
		NumTexAnim = 
		NumRenderFlags = 
		NumParticleSystems =
		NumRibbonEmitters =
		NumModelCameras = 0;

		CollisionRadius = BoundingRadius = 0;

		::memset(Name, 0, sizeof(Name));
		::memset(Dir, 0, sizeof(Dir));
		::memset(FileName, 0, sizeof(FileName));
	}

	//render flags
	struct SRenderFlag
	{
		//raw
		uint16_t	flags;
		uint16_t	blend;
		
		bool lighting() const { return (flags & RENDERFLAGS_UNLIT) == 0; }
		bool zwrite() const { return (flags & RENDERFLAGS_UNZWRITE) == 0; }
		bool frontCulling() const { return (flags & RENDERFLAGS_TWOSIDED) == 0; }
		bool invisible() const { return (flags & 256) != 0; }
	};

public:
	virtual bool loadFile(IMemFile* file) = 0;
	virtual M2Type getType() const = 0;
	virtual bool isCharacter() const = 0;
	virtual bool isHD() const = 0;

	virtual int16_t getAnimationIndex(const char* name, uint32_t subIdx = 0) const= 0;
	virtual uint32_t getAnimationCount(const char* name) const = 0;

	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

	virtual void clearAllActions() = 0;
	virtual bool addAction(wow_m2Action* action) = 0;
	virtual wow_m2Action* getAction(const char* name) const = 0;

	vector3df getBoundingAACenter() const { return BoundingAABBox.getCenter(); }
	ITexture* getTexture(uint32_t idx) const
	{ 
		if( idx >= NumTextures) 
			return nullptr; 
		return Textures[idx]; 
	}

public:
	char		Name[DEFAULT_SIZE];
	char		Dir[QMAX_PATH];
	char		FileName[DEFAULT_SIZE];

	E_M2_VER	M2Version;
	uint32_t		NumVertices;
	uint32_t		NumBoundingVerts;
	uint32_t		NumBoundingTriangles;
	uint32_t		NumTextures;
	uint32_t		NumTexLookup;
	uint32_t		NumAttachments;
	uint32_t		NumAttachLookup;
	uint32_t		NumGlobalSequences;
	uint32_t		NumColors;
	uint32_t		NumTransparencies;
	uint32_t		NumTranparencyLookukp;
	uint32_t		NumTexAnim;
	uint32_t		NumAnimations;
	uint32_t		NumAnimationLookup;
	uint32_t		NumBones;
	uint32_t		NumBoneLookup;
	uint32_t		NumRenderFlags;
	uint32_t		NumParticleSystems;
	uint32_t		NumRibbonEmitters;
	uint32_t		NumModelCameras;

	aabbox3df		CollisionAABBox;			//和周围的潜在场景碰撞
	float		CollisionRadius;
	aabbox3df	BoundingAABBox;			//自身的包围
	float		BoundingRadius;

	M2::Header20*		m2Header;
	M2::Header21*		m2HeaderEx;

	//verts
	SVertex_PNT2W*		GVertices;
	SVertex_A*			AVertices;

	//bounds
	vector3df*		Bounds;
	uint16_t*		BoundTris;

	//texture
	uint32_t*		TextureFlags;
	ETextureTypes*		TextureTypes;							//是否是可替换的纹理，这样的纹理不在mesh中
	ITexture**		Textures;					//常规纹理，由mesh指定的路径加载
	int16_t*		TexLookup;

	//attachment
	SModelAttachment*			Attachments;	
	int16_t*		AttachLookup;
	
	//anim
	int32_t*		GlobalSequences;
	SModelColor*		Colors;
	SModelTransparency*		Transparencies;
	int16_t*		TransparencyLookup;
	SModelTextureAnim*		TextureAnim;
	
	//actions
	SModelAnimation*		Animations;
	int16_t*		AnimationLookup;
	
	//bones
	SModelBone*		Bones;
	int16_t*		BoneLookup;
	
	SRenderFlag*		RenderFlags;

	//paritcle
	ParticleSystem*		ParticleSystems;

	//ribbon
	RibbonEmitter*			RibbonEmitters;

	//camera
	SModelCamera*			ModelCameras;
	
	//skin 0
	CFileSkin*		Skin;

	bool		VideoBuilt;
};
