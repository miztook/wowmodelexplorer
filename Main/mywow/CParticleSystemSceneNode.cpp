#include "stdafx.h"
#include "CParticleSystemSceneNode.h"
#include "mywow.h"
#include "CSceneRenderServices.h"
#include "CParticleSystemServices.h"

CParticleSystemSceneNode::CParticleSystemSceneNode( ParticleSystem* ps, IM2SceneNode* parent )
	: IParticleSystemSceneNode(parent), Ps(ps), LastAnim(-1), LastTime(-1)
{
	ParticleSystemServices = static_cast<CParticleSystemServices*>(g_Engine->getParticleSystemServices());

	::memset(&Hint, 0, sizeof(Hint));

	ParentM2Node = parent;

	Density = 1.0f;

	Rem = 0;

	Material.Lighting = false;
	Material.ZWriteEnable = false;
	Material.Cull = ECM_BACK;

	switch(Ps->blend)
	{
	case M2::BM_ALPHA_BLEND:
		Material.MaterialType= EMT_TRANSPARENT_ALPHA_BLEND;
		break;
	case M2::BM_ADDITIVE_ALPHA:
		Material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;
		break;
	default:
		Material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;
		break;
	}
	
	Speed = 1.0f;
	Scale = 1.0f;
	CurrentAnim = 0;
	CurrentFrame = 0;
	Emitting = false;

	CurrentProjection = CurrentView = NULL_PTR;

	//color, alpha
	EnableModelColor = false;
	EnableModelAlpha = false;
	ModelAlpha = 1.0f;

	::InitializeListHead(&LiveParticleList);
	LiveParticleCount = 0;
}

CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
	for (PLENTRY e = LiveParticleList.Flink; e != &LiveParticleList; )
	{
		Particle* p = reinterpret_cast<Particle*>CONTAINING_RECORD(e, Particle, Link);
		e = e->Flink;

		ParticleSystemServices->putParticle(p);
	}
	InitializeListHead(&LiveParticleList);
	LiveParticleCount = 0;
}

void CParticleSystemSceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	update(false);

	if (!Visible ||
		(frustumcheck && !isNodeEligible()))
		return;

	if (ParentM2Node)
		Distance = ParentM2Node->Distance;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);

	IParticleSystemSceneNode::registerSceneNode(frustumcheck, sequence);
}

bool CParticleSystemSceneNode::isNodeEligible() const
{
	if (!Ps->texture)
		return false;

	if (ParentM2Node)
		return ParentM2Node->isNodeEligible();

	return true;
}

void CParticleSystemSceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	float delta = timeSinceLastFrame * 0.001f * Speed;

	u32 anim = CurrentAnim;
	u32 time = (u32)(CurrentFrame * Speed);

	if (LastAnim != anim || time < LastTime)			//在anim改变或时间轴倒退时清除hint
		::memset(&Hint, 0, sizeof(SHint));

	if (Emitting)
	{
		float frate = 0;
		float flife = 0;
		u16 en = 1;

		Hint.frate = Ps->emissionRate.getValue(anim, time, frate, Hint.frate);
		Hint.flife = Ps->lifespan.getValue(anim, time, flife, Hint.flife);
		Hint.en = Ps->enabled.getValue(anim, time, en, Hint.en);

		f32 wholeDensity = ParticleSystemServices->getParticleDynamicDensity() * Density;
		frate *= wholeDensity;

		f32 ftospawn;
		if (flife != 0)
			ftospawn = min_(50.0f, min_(delta, 0.033f) * frate / flife) + Rem;
		else
			ftospawn = Rem;
		if ( ftospawn < 1.0f )
		{
			Rem = (ftospawn >= 0) ? ftospawn : 0;
		}
		else
		{
			u32 tospawn = (u32)ftospawn;

			u32 maxParticles = (u32)ceilf(ParticleSystemServices->getBufferQuota() * wholeDensity);

			if (tospawn + LiveParticleCount > maxParticles)
			{
				tospawn = maxParticles > LiveParticleCount ? maxParticles - LiveParticleCount : 0;
				Rem = 0;
			}
			else
				Rem = ftospawn - tospawn;

			if (en != 0 && tospawn)
			{
				float len = 0;
				float width = 0;
				float speed = 0;
				float variation = 0;
				float vert = 0;
				float horz = 0;

				Hint.width = Ps->emissionAreaWidth.getValue(anim, time, width, Hint.width);
				Hint.len = Ps->emissionAreaLength.getValue(anim, time, len, Hint.len);
				Hint.speed = Ps->emissionSpeed.getValue(anim, time, speed, Hint.speed);
				Hint.variation = Ps->speedVariation.getValue(anim, time, variation, Hint.variation);
				Hint.vert = Ps->verticalRange.getValue(anim, time, vert, Hint.vert);
				Hint.horz = Ps->horizontalRange.getValue(anim, time, horz, Hint.horz);

				ParticleEmitter::EmitterParam param;
				param.areaWidth = width * 0.5f;
				param.areaLen = len * 0.5f;
				param.speed = speed;
				param.variation = variation;
				param.vertical = vert;
				param.horizontal = horz;
				param.maxlife = flife;

				for (u32 i=0; i<tospawn; ++i)
				{
					Particle* p = ParticleSystemServices->getParticle();
					if (!p)
						break;
					Ps->emitter->emitParticle(ParentM2Node->getM2Instance(), AbsoluteTransformation, param, p);
					InsertTailList(&LiveParticleList, &p->Link);
					++LiveParticleCount;
				}
			}
		}
	}	//end emit
	
	float mspeed = 1.0f;

	for (PLENTRY e = LiveParticleList.Flink; e != &LiveParticleList; )
	{
		Particle* p = reinterpret_cast<Particle*>CONTAINING_RECORD(e, Particle, Link);
		e = e->Flink;

		p->life += delta;
			
		float rlife = p->life / p->maxlife;
		if (rlife >= 1.0f)					//dead
		{
			RemoveEntryList(&p->Link);
			--LiveParticleCount;
			ParticleSystemServices->putParticle(p);
		}
		else		//affector
		{
			float grav = 0;
			float deaccel = 0;

			Hint.grav = Ps->gravity.getValue(anim, time, grav, Hint.grav);
			Hint.deaccel = Ps->deacceleration.getValue(anim, time, deaccel, Hint.deaccel);

			if (Ps->boneIndex != -1 && Ps->ribbontype)
			{
				matrix4 mat;
				Ps->getBoneMatrix(ParentM2Node->getM2Instance(), AbsoluteTransformation, mat);
				p->basepos = Ps->pos;
				mat.transformVect(p->basepos);
			}

			p->speed += ( -vector3df::UnitY() * grav - p->dir * deaccel ) * delta;

   			if (Ps->slowdown > 0)
   				mspeed = max_(1.0f - Ps->slowdown * p->life, 0.0f);

			p->pos += p->speed * mspeed * delta;

			p->rotation += Ps->rotation * mspeed * delta;

			if (rlife < 0.5f)
			{
				f32 prop = rlife / 0.5f;
				p->size = interpolate(prop, Ps->sizes[0], Ps->sizes[1]) * interpolate(prop, Ps->scales[0], Ps->scales[1]);
				p->color = interpolate(prop, Ps->colors[0], Ps->colors[1]);
			}
			else
			{
				f32 prop = (rlife-0.5f) / 0.5f;
				p->size = interpolate(prop, Ps->sizes[1], Ps->sizes[2]) * interpolate(prop, Ps->scales[1], Ps->scales[2]);;
				p->color = interpolate(prop, Ps->colors[1], Ps->colors[2]);
			}
		}
	}

	LastAnim = anim;
	LastTime = time;
}

void CParticleSystemSceneNode::render() const
{
	if (LiveParticleCount == 0)
		return;

	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	SRenderUnit unit = {0};

	unit.drawParam.numVertices = min_(LiveParticleCount * 4, ParticleSystemServices->getMaxVertexCount());
	unit.distance = Distance;
	unit.bufferParam = ParticleSystemServices->BufferParam;
	unit.primType = EPT_TRIANGLES;
	unit.sceneNode = this;
	unit.material = Material;
	setMaterial(unit.material);
	unit.textures[0] = Ps->texture;
	unit.matWorld = NULL_PTR;			//unit
	unit.matView = CurrentView;
	unit.matProjection = CurrentProjection;

	sceneRenderServices->addRenderUnit(&unit, ERT_PARTICLE);
}

u32 CParticleSystemSceneNode::onFillVertexBuffer( SVertex_PCT* vertices, u32 vertexCount ) const
{
	ICamera* camera = g_Engine->getSceneManager()->getActiveCamera();

	const matrix4& view = CurrentView ? *CurrentView : camera->getViewMatrix();

	vector3df right( view[0], view[4], view[8] );
	vector3df up( view[1], view[5], view[9] );
	vector3df dir( view[2], view[6], view[10]);

	u32 vCount = 0;

	//normal particles
	if (Ps->billboard)			//world
	{
		for (PLENTRY e = LiveParticleList.Flink; e != &LiveParticleList; )
		{
			if (vCount + 4 > vertexCount)
				break;

			Particle* p = reinterpret_cast<Particle*>CONTAINING_RECORD(e, Particle, Link);
			e = e->Flink;

			vector3df w = right * p->size.X * Scale;
			vector3df h = up * p->size.Y * Scale;

			if (p->rotation != 0.0f)
			{
				quaternion q(p->rotation, -dir);
				 fillParticleRect(p, getParticleUVFlag(p), &vertices[vCount], w, h, q);
			}
			else
			{
				fillParticleRect(p, getParticleUVFlag(p), &vertices[vCount], w, h);
			}

			SColor c = p->color;
			if (EnableModelAlpha)
				c.setAlpha((u32)(p->color.getAlpha() * ModelAlpha));

			vertices[vCount + 0].Color = 
				vertices[vCount + 1].Color = 
				vertices[vCount + 2].Color = 
				vertices[vCount + 3].Color = c;

			vCount += 4;
		}
	}
	else
	{
		for (PLENTRY e = LiveParticleList.Flink; e != &LiveParticleList;)
		{
			if (vCount + 4 >= vertexCount)
				break;

			Particle* p = reinterpret_cast<Particle*>CONTAINING_RECORD(e, Particle, Link);
			e = e->Flink;

			f32 w = p->size.X * Scale;		
			f32 h = p->size.Y * Scale;

			if (!Ps->ribbontype)
			{
				fillParticleRect(p, getParticleUVFlag(p), &vertices[vCount], vector3df(w,0,0), vector3df(0,0,h));
			}
			else
			{
				vector3df center = p->basepos;
				AbsoluteTransformation.transformVect(center);
				vector3df pos = p->pos;

				vertices[vCount + 0].Pos = center + pos + vector3df(-w, 0, h);			
				vertices[vCount + 1].Pos = center + pos + vector3df(w, 0, h);		
				vertices[vCount + 2].Pos = center + pos + vector3df(-w, 0,- h);
				vertices[vCount + 3].Pos = center + pos + vector3df(w, 0, -h);
			}
		
			vertices[vCount + 0].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[vCount + 1].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[vCount + 2].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[vCount + 3].TCoords = Ps->Tiles[p->tile].tc[3];

			SColor c = p->color;
			if (EnableModelAlpha)
				c.setAlpha((u32)(p->color.getAlpha() * ModelAlpha));

			vertices[vCount + 0].Color = 
				vertices[vCount + 1].Color = 
				vertices[vCount + 2].Color = 
				vertices[vCount + 3].Color = c;

			vCount += 4;
		}
	}
	return vCount;
}

void CParticleSystemSceneNode::onUpdated()
{
	Scale = AbsoluteTransformation.getScale().X;
}

void CParticleSystemSceneNode::setMaterial( SMaterial& material ) const
{
	if(EnableModelColor)
		material.setMaterialColor(ModelColor);

	if (EnableModelAlpha && ModelAlpha < 1.0f)
	{
		material.setMaterialAlpha(ModelAlpha);
		switch(material.MaterialType)
		{
		case EMT_SOLID:
			material.MaterialType = EMT_TRANSPARENT_ALPHA_BLEND;
			break;
		case EMT_ALPHA_TEST:
			material.MaterialType = EMT_TRANSAPRENT_ALPHA_BLEND_TEST;
			break;
        default:
            break;
		}
	}
}

void CParticleSystemSceneNode::setWholeAlpha( bool enable, f32 val )
{
	f32 v = clamp_(val, 0.0f, 1.0f);
	EnableModelAlpha = enable;
	ModelAlpha = v;
}

void CParticleSystemSceneNode::setWholeColor( bool enable, SColor color )
{
	EnableModelColor = enable;
	ModelColor = color;
}

void CParticleSystemSceneNode::fillParticleRect( Particle* p, u32 uvflag, SVertex_PCT* vertices, const vector3df& w, const vector3df& h ) const
{
	vector3df worldpos = p->basepos + p->pos;

	switch(uvflag)
	{
	case 0:
		{
			vertices[0].Pos = worldpos - w + h;
			vertices[1].Pos = worldpos +  w + h;
			vertices[2].Pos = worldpos -  w - h;
			vertices[3].Pos = worldpos + w - h;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[3];
		}
		break;
	case 1:
		{
			vertices[0].Pos = p->basepos + h;
			vertices[1].Pos = worldpos +  w + h;
			vertices[2].Pos = p->basepos - h;
			vertices[3].Pos = worldpos + w - h;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[3];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[1];
		}
		break;
	case 2:
		{
			vertices[0].Pos = p->basepos - w;
			vertices[1].Pos = p->basepos + w;
			vertices[2].Pos = worldpos - w - h;
			vertices[3].Pos = worldpos + w - h;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[3];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[0];
		}
		break;
	case 3:
		{
			vertices[0].Pos = worldpos - w + h;
			vertices[1].Pos = p->basepos + h;
			vertices[2].Pos = worldpos - w - h;
			vertices[3].Pos = p->basepos - h;

			vertices[ 0].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[3];
		}
		break;
	case 4:
		{
			vertices[0].Pos = worldpos- w + h;
			vertices[1].Pos = worldpos +  w + h;
			vertices[2].Pos = p->basepos - w;
			vertices[3].Pos = p->basepos + w;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[3];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[2];
		}
		break;
	}
}

void CParticleSystemSceneNode::fillParticleRect( Particle* p, u32 uvflag, SVertex_PCT* vertices, const vector3df& w, const vector3df& h, const quaternion& q ) const
{
	vector3df worldpos = p->basepos + p->pos;

	vector3df a = q * (w - h);
	vector3df b = q * (w + h);
	vector3df c = q * w;
	vector3df d = q * h;

	switch(uvflag)
	{
	case 0:
		{
			vertices[0].Pos = worldpos - a;
			vertices[1].Pos = worldpos +  b;
			vertices[2].Pos = worldpos - b;
			vertices[3].Pos = worldpos +  a;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[3];
		}
		break;
	case 1:
		{
			vertices[0].Pos = p->basepos + d;
			vertices[1].Pos = worldpos +  b;
			vertices[2].Pos = p->basepos - d;
			vertices[3].Pos = worldpos + a;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[3];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[1];
		}
		break;
	case 2:
		{
			vertices[0].Pos = p->basepos - c;
			vertices[1].Pos = p->basepos + c;
			vertices[2].Pos = worldpos - b;
			vertices[3].Pos = worldpos + a;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[3];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[0];
		}
		break;
	case 3:
		{
			vertices[0].Pos = worldpos - a;
			vertices[1].Pos = p->basepos + d;
			vertices[2].Pos = worldpos - b;
			vertices[3].Pos = p->basepos - d;

			vertices[ 0].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[2];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[3];
		}
		break;
	case 4:
		{
			vertices[0].Pos = worldpos- a;
			vertices[1].Pos = worldpos +  b;
			vertices[2].Pos = p->basepos - c; 
			vertices[3].Pos = p->basepos + c;

			vertices[0].TCoords = Ps->Tiles[p->tile].tc[3];
			vertices[1].TCoords = Ps->Tiles[p->tile].tc[0];
			vertices[2].TCoords = Ps->Tiles[p->tile].tc[1];
			vertices[3].TCoords = Ps->Tiles[p->tile].tc[2];
		}
		break;
	}
}

u32 CParticleSystemSceneNode::getParticleUVFlag( Particle* p ) const
{
	if (!Ps->ribbontype)
		return 0;

	if(p->pos.X > 0.0f)
	{
		if(p->pos.X > fabs(p->pos.Z))
			return 1;
		else
			return 2;
	}
	else
	{
		if(-p->pos.X > fabs(p->pos.Z))
			return 3;
		else
			return 4;
	}
}

