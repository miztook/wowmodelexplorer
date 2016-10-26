#pragma once

#include "core.h"
#include "SColor.h"
#include "wow_m2_structs.h"
#include "wow_animation.h"

class ITexture;
class IFileM2;
struct SModelBone;
class wow_m2instance;

struct Particle
{
	LENTRY	Link;			//live particles link

	vector3df		basepos;
	vector3df		pos;
	vector3df		speed;
	vector3df		dir;

	vector2df		size;
	f32  life;
	f32	maxlife;
	f32  rotation;
	SColor	color;
	u32	tile;
};

class ParticleSystem;

class ParticleEmitter
{	
public:
	struct EmitterParam
	{
		float areaLen;
		float areaWidth;
		float speed;
		float variation;
		float vertical;
		float horizontal;
		float maxlife;
	};

	ParticleEmitter(ParticleSystem* sys) : sys(sys) {}
    
    virtual ~ParticleEmitter() {}
    
public:
	
	virtual void emitParticle(const wow_m2instance* character, const matrix4& world, const EmitterParam& param, Particle* p) = 0;

protected:
	ParticleSystem* sys;
};

class PlaneParticleEmitter : public ParticleEmitter
{
public:
	PlaneParticleEmitter(ParticleSystem* sys) : ParticleEmitter(sys) {}
	virtual void emitParticle(const wow_m2instance* character, const matrix4& world, const EmitterParam& param, Particle* p);
};

class SphereParticleEmitter : public ParticleEmitter
{
public:
	SphereParticleEmitter(ParticleSystem* sys) : ParticleEmitter(sys) {}
	virtual void emitParticle(const wow_m2instance* character, const matrix4& world, const EmitterParam& param, Particle* p);
};

class ParticleSystem
{
private:
	DISALLOW_COPY_AND_ASSIGN(ParticleSystem);

friend class PlaneParticleEmitter;
friend class SphereParticleEmitter;

public:
	ParticleSystem()
		: Mesh(NULL_PTR)
	{
		tileRotation = 0;
		emitter = NULL_PTR;
		blend = 0;
		rows = cols = 0;

		boneIndex = -1;
		texture = NULL_PTR;
		billboard = false;
		ribbontype = false;

		memset(scales, 0, sizeof(scales));
		slowdown = 0;
		rotation = 0;

		EmitterType = 0;
		flags = 0;
		NumTiles = 0;

		Tiles = NULL_PTR;
	}

	~ParticleSystem()
	{
		delete emitter;
		delete[] Tiles;
	}

	struct TexCoordSet 
	{
		vector2df tc[4];
	};

public:
	void init(const M2::ModelParticleEmitterDef& mta, const u8* fileData, s32* globalSeq, u32 numGlobalSeq);

	void calcSpreadMatrix(float spread1, float spread2, matrix4& mat);

	void initTile(vector2df* tc, int num);

	void getBoneMatrix(const wow_m2instance* character, const matrix4& world, matrix4& mat);

public:
	SWowAnimation<float>		emissionSpeed;
	SWowAnimation<float>		speedVariation;
	SWowAnimation<float>		verticalRange;
	SWowAnimation<float>		horizontalRange;
	SWowAnimation<float>		gravity;
	SWowAnimation<float>		lifespan;
	SWowAnimation<float>		emissionRate;
	SWowAnimation<float>		emissionAreaWidth;				//x
	SWowAnimation<float>		emissionAreaLength;				//z
	SWowAnimation<float>		deacceleration;
	SWowAnimation<u16>		enabled;

	ParticleEmitter*		emitter;
	IFileM2*	Mesh;
	ITexture*		texture;
	TexCoordSet*		Tiles;

	SColor		colors[3];
	vector2df		sizes[3];
	float		scales[3];
	float		slowdown;
	float		rotation;
	vector3df		pos;

	int		blend;
	s32		flags;
	u32		NumTiles;
	s16		tileRotation;
	s16		rows, cols;
	s16		boneIndex;
	s16		EmitterType;
	bool		billboard;
	bool		ribbontype;
//	bool		followParent;
};


struct RibbonSegment
{
	LENTRY	Link;			//live segment link

	vector3df	pos;
	vector3df up;
	vector3df back;
	float len;
	float len0;
	s32		time;
};

class RibbonEmitter
{
private:
	DISALLOW_COPY_AND_ASSIGN(RibbonEmitter);

public:
	RibbonEmitter()
	: Mesh(NULL_PTR), texture(NULL_PTR)
	{
		parent = 0;
		boneIndex = 0;
		texture = 0;
		seglen = 0;
		numsegs = 0;
	}

public:
	void init(const M2::ModelRibbonEmitterDef& mta, const u8* fileData, s32* globalSeq, u32 numGlobalSeq);

public:
	SWowAnimation<vector3df>	color;
	SWowAnimationShort	opacity;
	SWowAnimation<float>		above, below;

	IFileM2*	Mesh;
	SModelBone* parent;
	ITexture* texture;

	vector3df pos;
	float seglen;
	int		numsegs;
	int		boneIndex;
};