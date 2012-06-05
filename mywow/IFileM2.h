#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_enums.h"
#include "wow_animation.h"
#include "wow_particle.h"
#include "wow_m2_structs.h"
#include "S3DVertex.h"
#include <list>
#include <map>
#include <set>

class MPQFile;
class ITexture;

inline vector3df fixCoordinate(const vector3df& v)
{
	return vector3df(v.X, v.Z, v.Y);
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
#define	RENDERFLAGS_UNFOGGED	2
#define	RENDERFLAGS_TWOSIDED	4
#define	RENDERFLAGS_BILLBOARD	8
#define	RENDERFLAGS_UNZWRITE		16
#define RENDERFLAGS_ADDCOLOR		256

enum E_BONE_TYPE
{
	EBT_NONE = 0,
	EBT_LEFTHAND,
	EBT_RIGHTHAND
};

struct SModelBone
{
	SWowAnimationVec3		trans;
	SWowAnimationQuat		rot;
	SWowAnimationVec3		scale;

	vector3df	pivot;
	s32		parent;
	bool		billboard;
	u32		flags;
	E_BONE_TYPE  bonetype;			//from bone lookup table

	void		init(const u8* filedata, s32* globalSeq, u32 numGlobalSeq, const M2::bone& b, SAnimFile* animFiles)
	{
		parent = b._ParentBone;
		billboard = (b._Flags & 8) != 0;
		flags = b._Flags;

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
	vector3df position;
	s32 boneIndex;
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
	CBoneUnit() : BoneCount(0), PrimCount(0), Index(-1), BoneVStart(0), StartIndex(0) {}

	typedef std::map<u8, u8, std::less<u8>, qzone_allocator<std::pair<u8,u8>>>	T_bone2boneMap;	 
	T_bone2boneMap		local2globalMap;					//local -> global

	u32		BoneVStart;				//在skin bone顶点中的偏移
	u32		PrimCount;
	u16		StartIndex;				//在skin 中index偏移

	typedef std::set<u8, std::less<u8>, qzone_allocator<u8>> T_BoneIndices;
	T_BoneIndices			BoneIndices;

	u8	BoneCount;
	s8		Index;
};

struct STexUnit
{
	STexUnit() : TexID(-1), rfIndex(-1), ColorIndex(-1), TransIndex(-1), TexAnimIndex(-1) {}

	s16	TexID;				//使用纹理编号
	s16	rfIndex;				//render flag索引
	s16	ColorIndex;
	s16  TransIndex;
	s16	TexAnimIndex;
	u32  TexFlags;
	bool	WrapX;
	bool	WrapY;
};

class	CGeoset 
{	
public:
	CGeoset() : VStart(0), VCount(0), IStart(0), ICount(0), GeoID(0), MaxWeights(4) {}

	u16		VStart;
	u16		VCount;
	u16		IStart;
	u16		ICount;
	u32		GeoID;	
	u16		MaxWeights;

	//tex unit
	typedef std::list<STexUnit, qzone_allocator<STexUnit>> T_TexUnitList;
	T_TexUnitList		TexUnits;

	u32 getTexUnitCount() const { return TexUnits.size(); }
	const STexUnit* getTexUnit(u8 index) const 
	{
		u8 i = 0;
		for(T_TexUnitList::const_iterator itr=TexUnits.begin(); itr != TexUnits.end(); ++itr)
		{
			if (index == i)
				return &(*itr);
			++i;
		}
		return NULL;
	}

	//bone unit
	typedef std::list<CBoneUnit, qzone_allocator<CBoneUnit>> T_BoneUnitList;
	T_BoneUnitList		BoneUnits;

};

class CFileSkin;

#define	MAX_M2_TEXTURES	64

//对应m2文件，静态数据，加载m2同时也加载对应的skin和anim文件
class IFileM2 : public IReferenceCounted<IFileM2>
{
protected:
	virtual void onRemove() 
	{
		releaseVideoResources(); 
	}

public:
	IFileM2()
		: VideoBuilt(false), ContainsBillboardBone(false)
	{
		m2Header = NULL;
		Vertices = NULL;
		VBones = NULL;
		Bounds = NULL;
		BoundTris = NULL;
		TextureFlags = NULL;
		TextureTypes = NULL;
		Textures = NULL;
		TexLookup = NULL;
		Attachments = NULL;
		AttachLookup = NULL;
		GlobalSequences = NULL;
		Colors = NULL;
		Transparencies = NULL;
		TransparencyLookup = NULL;
		TextureAnim = NULL;
		Bones = NULL;
		BoneLookup = NULL;
		Animations = NULL;
		AnimationLookup = NULL;
		RenderFlags = NULL;
		Skin = NULL;
		ParticleSystems = NULL;
		RibbonEmitters = NULL;
		ModelCameras = NULL;
	}

	virtual ~IFileM2()  { }

public:
	virtual bool loadFile(MPQFile* file) = 0;
	virtual u8* getFileData() const = 0;
	virtual M2Type getType() const = 0;
	virtual const aabbox3df& getBoundingBox() const = 0;
	virtual s16 getAnimationIndex(const c8* name, u32 subIdx = 0)= 0;
	virtual u32 getAnimationCount(const c8* name)= 0;

	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

public:
	c8		Name[DEFAULT_SIZE];
	c8		Dir[MAX_PATH];

	bool	VideoBuilt;

	M2::header*		m2Header;

	//verts
	S3DVertexModel*		Vertices;
	S3DVertexBone*			VBones;
	u32			NumVertices;

	//bounds
	u32		NumBoundingVerts;
	u32		NumBoundingTriangles;
	vector3df*		Bounds;
	u16*		BoundTris;
	aabbox3df	CollisionAABBox;			//和周围的潜在场景碰撞
	float		CollisionRadius;
	aabbox3df	BoundingAABBox;			//自身的包围
	float		BoundingRadius;

	//texture
	u32			NumTextures;
	u32*		TextureFlags;
	ETextureTypes*		TextureTypes;							//是否是可替换的纹理，这样的纹理不在mesh中
	ITexture**		Textures;					//常规纹理，由mesh指定的路径加载
	s16*		TexLookup;
	u32			NumTexLookup;

	//attachment
	SModelAttachment*			Attachments;
	u32		NumAttachments;
	s16*		AttachLookup;
	u32		NumAttachLookup;

	//anim
	s32*		GlobalSequences;
	u32			NumGlobalSequences;

	SModelColor*		Colors;
	u32		NumColors;

	SModelTransparency*		Transparencies;
	u32		NumTransparencies;
	s16*		TransparencyLookup;
	u32		NumTranparencyLookukp;
	SModelTextureAnim*		TextureAnim;
	u32		NumTexAnim;

	//actions
	SModelAnimation*		Animations;
	u32		NumAnimations;
	s16*		AnimationLookup;
	u32		NumAnimationLookup;

	//bones
	SModelBone*		Bones;
	u32		NumBones;
	s16*		BoneLookup;
	u32		NumBoneLookup;

	bool	ContainsBillboardBone;

	//render flags
	struct SRenderFlag
	{
		bool lighting;
		bool zwrite;
		bool frontCulling;
		bool invisible;				

		//raw
		u16	flags;
		u16	blend;
	};

	SRenderFlag*		RenderFlags;
	u32		NumRenderFlags;

	//paritcle
	ParticleSystem*		ParticleSystems;
	u32		NumParticleSystems;

	//ribbon
	RibbonEmitter*			RibbonEmitters;
	u32		NumRibbonEmitters;

	//camera
	SModelCamera*			ModelCameras;
	u32		NumModelCameras;

	//skin 0
	CFileSkin*		Skin;
};
