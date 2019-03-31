#pragma once

#include "IParticleSystemSceneNode.h"
#include "wow_particle.h"
#include "SMaterial.h"

class CParticleSystemServices;

class CParticleSystemSceneNode : public IParticleSystemSceneNode
{
public:
	CParticleSystemSceneNode(ParticleSystem* ps, IM2SceneNode* parent);
	virtual ~CParticleSystemSceneNode();

public:
	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual bool isNodeEligible() const;
	virtual aabbox3df getBoundingBox() const { return aabbox3df::Zero(); }
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual void onUpdated();

	uint32_t onFillVertexBuffer(SVertex_PCT* vertices, uint32_t vcount) const;

	virtual void setEmitting(bool on) { Emitting = on; }
	virtual void setAnimationFrame(uint32_t anim, uint32_t frame) { CurrentAnim = anim; CurrentFrame = frame; }
	virtual void setSpeed(float speed) { Speed = speed; }
	virtual void setOwnView(matrix4* view) { CurrentView = view; }
	virtual void setOwnProjection(matrix4* projection) { CurrentProjection = projection; }

	virtual void setWholeAlpha(bool enable, float val);
	virtual void setWholeColor(bool enable, SColor color);

	virtual void setEmitDensity(float density) { Density = density; }

protected:
	void setMaterial(SMaterial& material) const;

	void fillParticleRect(Particle* p, uint32_t uvflag, SVertex_PCT* vertices, const vector3df& w, const vector3df& h) const;
	void fillParticleRect(Particle* p, uint32_t uvflag, SVertex_PCT* vertices, const vector3df& w, const vector3df& h, const quaternion& q) const;

	uint32_t getParticleUVFlag(Particle* p) const;

private:
	struct SHint
	{
		int32_t grav;
		int32_t deaccel;
		int32_t frate;
		int32_t flife;
		int32_t len;
		int32_t width;
		int32_t speed;
		int32_t variation;
		int32_t vert;
		int32_t horz;
		int32_t en;
	};

private:
	CParticleSystemServices*	ParticleSystemServices;
	IM2SceneNode*		ParentM2Node;
	ParticleSystem*		Ps;
	matrix4*		CurrentProjection;
	matrix4*		CurrentView;

	LENTRY		LiveParticleList;
	uint32_t		LiveParticleCount;
	uint32_t		CurrentAnim;
	uint32_t		CurrentFrame;
	uint32_t		LastAnim;
	uint32_t		LastTime;

	float		Density;
	float		Rem;
	float		Speed;
	float		Scale;
	float			ModelAlpha;

	SMaterial		Material;
	SHint	Hint;

	SColor	ModelColor;
	bool		EnableModelAlpha;
	bool		EnableModelColor;
	bool		Emitting;
};