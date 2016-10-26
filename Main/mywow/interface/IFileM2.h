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
	u32	animID;
	u32  animSubID;
	u32	timeLength;

	s16	NextAnimation;
	s16	Index;
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
	s32		parent;
	u32		flags;
	s16		geosetId;
	E_BONE_TYPE  bonetype;			//from bone lookup table
	bool		billboard;

	void		init(const u8* filedata, s32* globalSeq, u32 numGlobalSeq, const M2::bone& b, SAnimFile* animFiles)
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

	void init(const u8* filedata, s32* globalSeq, u32 numGlobalSeq, const M2::colorDef& cd)
	{
		colorAnim.init(&cd.color, filedata, globalSeq, numGlobalSeq);
		opacityAnim.init(&cd.opacity, filedata, globalSeq, numGlobalSeq);
	}
};

struct SModelTransparency
{
	SWowAnimation<u16>	tranAnim;

	void init(const u8* filedata, s32* globalSeq, u32 numGlobalSeq, const M2::transDef& td)
	{
		tranAnim.init(&td.trans, filedata, globalSeq, numGlobalSeq);
	}
};

struct SModelTextureAnim
{
	SWowAnimation<vector3df>	trans, rot, scale;

	void init(const u8* filedata, s32* globalSeq, u32 numGlobalSeq, const M2::texanimDef& t)
	{
		trans.init(&t.trans, filedata, globalSeq, numGlobalSeq);
		rot.init(&t.rot, filedata, globalSeq, numGlobalSeq);
		scale.init(&t.scale, filedata, globalSeq, numGlobalSeq);
	}
};

struct SModelAttachment			//挂点
{
	SModelAttachment() : id(0), boneIndex(-1) {}
	s32 id;
	s32 boneIndex;
	vector3df position;
};

struct SModelCamera
{
	vector3df	position, target;
	f32	nearclip, farclip;
 //	SWowAnimationVec3		positionAnim, targetAnim;
 //	SWowAnimation<f32>		rotationAnim;
//	SWowAnimation<f32>		fovAnim;
	void init(const M2::ModelCameraDefV10& mcd, const u8* filedata, s32* globalSeq)
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

class IIndexBuffer;

class CBoneUnit
{
public:
	CBoneUnit() : BoneVStart(0), TCount(0),  StartIndex(0), BoneCount(0), Index(-1) { }

	u32		BoneVStart;				//在skin bone顶点中的偏移
	u16		TCount;
	u16		StartIndex;				//在skin 中index偏移
	u8	BoneCount;
	s8		Index;

	typedef std::unordered_set<u8> T_BoneIndices;
	T_BoneIndices			BoneIndices;

	typedef std::unordered_map<u8, u8>	T_bone2boneMap;	 
	std::vector<u8>	local2globalMap;					//local -> global
};

struct STexUnit
{
	s16	TexID;				//使用纹理编号
	s16	rfIndex;				//render flag索引
	s16	ColorIndex;
	s16  TransIndex;
	s16	TexAnimIndex;
	u16	Mode;
	u16	Shading;		//shader?
	bool	WrapX;
	bool	WrapY;
	u32  TexFlags;
};

struct SBRect
{
	vector3df center;
	vector2df texcoords[4];
	f32 width;
	f32 height;
	u16 boneIndex;
};

class	CGeoset 
{	
private:
	DISALLOW_COPY_AND_ASSIGN(CGeoset);

public:
	CGeoset() : GeoID(0), BillboardRects(NULL_PTR), VStart(0), VCount(0), IStart(0), 
		ICount(0), MaxWeights(4), 
		BillBoard(true) {}

	~CGeoset()
	{
		delete[] BillboardRects;
	}

	u32		GeoID;	
	
	//tex unit
	typedef std::vector<STexUnit> T_TexUnits;
	T_TexUnits		TexUnits;

	//bone unit
	typedef std::vector<CBoneUnit> T_BoneUnits;
	T_BoneUnits		BoneUnits;

	SBRect*		BillboardRects;

	u16		VStart;
	u16		VCount;
	u16		IStart;
	u16		ICount;
	u16		MaxWeights;
	bool		BillBoard;

	u32 getTexUnitCount() const { return (u32)TexUnits.size(); }
	const STexUnit* getTexUnit(u8 index) const 
	{
		if (index >= TexUnits.size())
			return NULL_PTR;
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
		m2Header = NULL_PTR;
		m2HeaderEx = NULL_PTR;
		GVertices = NULL_PTR;
		AVertices = NULL_PTR;
		Bounds = NULL_PTR;
		BoundTris = NULL_PTR;
		TextureFlags = NULL_PTR;
		TextureTypes = NULL_PTR;
		Textures = NULL_PTR;
		TexLookup = NULL_PTR;
		Attachments = NULL_PTR;
		AttachLookup = NULL_PTR;
		GlobalSequences = NULL_PTR;
		Colors = NULL_PTR;
		Transparencies = NULL_PTR;
		TransparencyLookup = NULL_PTR;
		TextureAnim = NULL_PTR;
		Bones = NULL_PTR;
		BoneLookup = NULL_PTR;
		Animations = NULL_PTR;
		AnimationLookup = NULL_PTR;
		RenderFlags = NULL_PTR;
		Skin = NULL_PTR;
		ParticleSystems = NULL_PTR;
		RibbonEmitters = NULL_PTR;
		ModelCameras = NULL_PTR;

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
		u16	flags;
		u16	blend;
		
		bool lighting;
		bool zwrite;
		bool frontCulling;
		bool invisible;				
	};

public:
	virtual bool loadFile(IMemFile* file) = 0;
	virtual M2Type getType() const = 0;
	virtual bool isCharacter() const = 0;
	virtual bool isHD() const = 0;

#ifdef FULL_INTERFACE	
	virtual const aabbox3df& getBoundingBox() const = 0;
#endif

	virtual s16 getAnimationIndex(const c8* name, u32 subIdx = 0) const= 0;
	virtual u32 getAnimationCount(const c8* name) const = 0;

	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

	virtual void clearAllActions() = 0;
	virtual bool addAction(wow_m2Action* action) = 0;
	virtual wow_m2Action* getAction(const c8* name) const = 0;

	vector3df getBoundingAACenter() const { return BoundingAABBox.getCenter(); }
	ITexture* getTexture(u32 idx) const
	{ 
		if( idx >= NumTextures) 
			return NULL_PTR; 
		return Textures[idx]; 
	}

public:
	c8		Name[DEFAULT_SIZE];
	c8		Dir[QMAX_PATH];
	c8		FileName[DEFAULT_SIZE];

	E_M2_VER	M2Version;
	u32		NumVertices;
	u32		NumBoundingVerts;
	u32		NumBoundingTriangles;
	u32		NumTextures;
	u32		NumTexLookup;
	u32		NumAttachments;
	u32		NumAttachLookup;
	u32		NumGlobalSequences;
	u32		NumColors;
	u32		NumTransparencies;
	u32		NumTranparencyLookukp;
	u32		NumTexAnim;
	u32		NumAnimations;
	u32		NumAnimationLookup;
	u32		NumBones;
	u32		NumBoneLookup;
	u32		NumRenderFlags;
	u32		NumParticleSystems;
	u32		NumRibbonEmitters;
	u32		NumModelCameras;

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
	u16*		BoundTris;

	//texture
	u32*		TextureFlags;
	ETextureTypes*		TextureTypes;							//是否是可替换的纹理，这样的纹理不在mesh中
	ITexture**		Textures;					//常规纹理，由mesh指定的路径加载
	s16*		TexLookup;

	//attachment
	SModelAttachment*			Attachments;	
	s16*		AttachLookup;
	
	//anim
	s32*		GlobalSequences;
	SModelColor*		Colors;
	SModelTransparency*		Transparencies;
	s16*		TransparencyLookup;
	SModelTextureAnim*		TextureAnim;
	
	//actions
	SModelAnimation*		Animations;
	s16*		AnimationLookup;
	
	//bones
	SModelBone*		Bones;
	s16*		BoneLookup;
	
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
