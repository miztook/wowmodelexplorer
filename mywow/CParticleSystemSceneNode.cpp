#include "stdafx.h"
#include "CParticleSystemSceneNode.h"
#include "mywow.h"

CParticleSystemSceneNode::CParticleSystemSceneNode( ParticleSystem* ps, IM2SceneNode* parent )
	: IParticleSystemSceneNode(parent), Ps(ps), LastAnim(-1), LastTime(-1)
{
	ParticleSystemServices = g_Engine->getParticleSystemServices();

	ParentM2Node = parent;

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

	CurrentProjection = CurrentView = NULL;

	//color, alpha
	EnableModelColor = false;
	EnableModelAlpha = false;
	ModelAlpha = 1.0f;
}

CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
	for (T_LiveParticles::iterator itr = LiveParticles.begin(); itr != LiveParticles.end(); ++itr)
	{
		ParticleSystemServices->putParticle((*itr));
	}
}

void CParticleSystemSceneNode::registerSceneNode(bool frustumcheck)
{
	update(false);

	if (!Visible)
		return;

	if (frustumcheck && !isNodeEligible())
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this);

	IParticleSystemSceneNode::registerSceneNode(frustumcheck);
}

bool CParticleSystemSceneNode::isNodeEligible()
{
	if (ParentM2Node)
		return true;

	return false;
}

void CParticleSystemSceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	float delta = timeSinceLastFrame * 0.001f * Speed;

	u32 anim = CurrentAnim;
	u32 time = (u32)(CurrentFrame * Speed);

	if (LastAnim != anim || time < LastTime)			//在anim改变或时间轴倒退时清除hint
		::memset(&Hint, 0, sizeof(SHint));

	float grav = 0;
	float deaccel = 0;

	Hint.grav = Ps->gravity.getValue(anim, time, grav, Hint.grav);
	Hint.deaccel = Ps->deacceleration.getValue(anim, time, deaccel, Hint.deaccel);

	if (Emitting)
	{
		float frate = 0;
		float flife = 0;
		float len = 0;
		float width = 0;
		float speed = 0;
		float variation = 0;
		float vert = 0;
		float horz = 0;
		u16 en = 1;

		Hint.frate = Ps->emissionRate.getValue(anim, time, frate, Hint.frate);
		Hint.flife = Ps->lifespan.getValue(anim, time, flife, Hint.flife);
		Hint.width = Ps->emissionAreaWidth.getValue(anim, time, width, Hint.width);
		Hint.len = Ps->emissionAreaLength.getValue(anim, time, len, Hint.len);
		Hint.speed = Ps->emissionSpeed.getValue(anim, time, speed, Hint.speed);
		Hint.variation = Ps->speedVariation.getValue(anim, time, variation, Hint.variation);
		Hint.vert = Ps->verticalRange.getValue(anim, time, vert, Hint.vert);
		Hint.horz = Ps->horizontalRange.getValue(anim, time, horz, Hint.horz);
		Hint.en = Ps->enabled.getValue(anim, time, en, Hint.en);

		frate *= ParticleSystemServices->getParticleDensity();

		f32 ftospawn;
		if (flife != 0)
			ftospawn = min_(50.0f, min_(delta, 0.033f) * frate / flife) + Rem;
		else
			ftospawn = Rem;
		if ( ftospawn < 1.0f )
		{
			Rem = ftospawn;
			if (Rem < 0) 
				Rem = 0;
		}
		else
		{
			u32 tospawn = (u32)ftospawn;

			if (tospawn + LiveParticles.size() > MAX_PARTICLES)
				tospawn = MAX_PARTICLES - LiveParticles.size();

			Rem = ftospawn - tospawn;

			ParticleEmitter::EmitterParam param;
			param.areaWidth = width * 0.5f;
			param.areaLen = len * 0.5f;
			param.speed = speed;
			param.variation = variation;
			param.vertical = vert;
			param.horizontal = horz;
			param.maxlife = flife;

			if (en != 0)
			{
				for (u32 i=0; i<tospawn; ++i)
				{
					Particle* p = ParticleSystemServices->getParticle();
					if (!p)
						break;
					Ps->emitter->emitParticle(ParentM2Node->getM2Instance(), anim, time, param, p);
					LiveParticles.push_back(p);
				}
			}
		}
	}	//end emit
	
	float mspeed = 1.0f;

	for (T_LiveParticles::iterator itr = LiveParticles.begin(); itr != LiveParticles.end();)
	{
		Particle& p = *(*itr);
		p.life += delta;
			
		float rlife = p.life / p.maxlife;
		if (rlife >= 1.0f)					//dead
		{
			ParticleSystemServices->putParticle((*itr));
			itr = LiveParticles.erase(itr);
		}
		else		//affector
		{
			p.speed += ( p.down * grav - p.dir * deaccel ) * delta;

   			if (Ps->slowdown > 0)
   				mspeed = max_(1.0f - Ps->slowdown * p.life, 0.0f);

			p.pos += p.speed * mspeed * delta;

			if (rlife < 0.5f)
			{
				f32 prop = rlife / 0.5f;
				p.size = interpolate(prop, Ps->sizes[0], Ps->sizes[1]);
				p.color = interpolate(prop, Ps->colors[0], Ps->colors[1]);
			}
			else
			{
				f32 prop = (rlife-0.5f) / 0.5f;
				p.size = interpolate(prop, Ps->sizes[1], Ps->sizes[2]);
				p.color = interpolate(prop, Ps->colors[1], Ps->colors[2]);
			}

			++itr;
		}
	}


	LastAnim = anim;
	LastTime = time;
}

void CParticleSystemSceneNode::render()
{
	if (LiveParticles.empty())
		return;

	SRenderUnit unit = {0};

	unit.priority = ParentM2Node->RenderPriority;
	unit.distance = DistanceSq;
	unit.bufferParam = ParticleSystemServices->BufferParam;
	unit.primType = EPT_TRIANGLES;
	unit.sceneNode = this;
	unit.material = Material;
	setMaterial(unit.material);
	unit.textures[0] = Ps->texture;
	unit.matWorld = Ps->billboard ? NULL : &AbsoluteTransformation;			//unit
	unit.matView = CurrentView;
	unit.matProjection = CurrentProjection;

	g_Engine->getSceneRenderServices()->addRenderUnit(&unit, ERT_EFFECT);
}

void CParticleSystemSceneNode::onPreRender()
{
	ICamera* camera = g_Engine->getSceneManager()->getActiveCamera();
	if (!camera)
		return;

	const matrix4& view = CurrentView ? *CurrentView : camera->getViewMatrix();

	vector3df right( view[0], view[4], view[8] );
	vector3df up( view[1], view[5], view[9] );

	right.normalize();
	up.normalize();

	SGVertex_PC* gVertices;
	STVertex_1T* tVertices;
	if (ParticleSystemServices->CurrentOffset + 4 * LiveParticles.size() > ParticleSystemServices->BufferParam.vbuffer0->Size)
	{
		gVertices = (SGVertex_PC*)ParticleSystemServices->GVertices;
		tVertices = (STVertex_1T*)ParticleSystemServices->TVertices;
	}
	else
	{
		gVertices = (SGVertex_PC*)&ParticleSystemServices->GVertices[ParticleSystemServices->CurrentOffset];
		tVertices = (STVertex_1T*)&ParticleSystemServices->TVertices[ParticleSystemServices->CurrentOffset];
	}

	u32 vCount = 0;

	//normal particles
	if (Ps->ParticleType == 0 || Ps->ParticleType == 2)
	{
		if (Ps->billboard)			//world
		{
			for ( T_LiveParticles::iterator i = LiveParticles.begin(); i != LiveParticles.end(); ++i )
			{
				Particle* p = (*i);

				if (p->tile >= Ps->NumTiles || vCount + 4 > ParticleSystemServices->BufferParam.vbuffer0->Size)
					break;

				vector3df worldpos = p->pos;
				AbsoluteTransformation.transformVect(worldpos);

				vector3df w = right * p->size.X * Scale;
				vector3df h = up * p->size.Y * Scale;

				gVertices[vCount + 0].Pos = worldpos - w + h;
				gVertices[vCount + 1].Pos = worldpos + w + h;
				gVertices[vCount + 2].Pos = worldpos + w - h;
				gVertices[vCount + 3].Pos = worldpos - w - h;
				tVertices[vCount + 0].TCoords = Ps->Tiles[p->tile].tc[0];
				tVertices[vCount + 1].TCoords = Ps->Tiles[p->tile].tc[1];
				tVertices[vCount + 2].TCoords = Ps->Tiles[p->tile].tc[2];
				tVertices[vCount + 3].TCoords = Ps->Tiles[p->tile].tc[3];

				if (EnableModelAlpha)
					p->color.setAlpha(p->color.getAlpha() * ModelAlpha);
				gVertices[vCount + 0].Color = 
					gVertices[vCount + 1].Color = 
					gVertices[vCount + 2].Color = 
					gVertices[vCount + 3].Color = p->color.toSColor();

				vCount += 4;
			}
		}
		else
		{
			for ( T_LiveParticles::iterator i = LiveParticles.begin(); i != LiveParticles.end(); ++i )
			{
				Particle* p = (*i);

				if (p->tile >= Ps->NumTiles || vCount + 4 > ParticleSystemServices->BufferParam.vbuffer0->Size)
					break;

				f32 w = p->size.X * Scale;		f32 h = p->size.Y * Scale;

				gVertices[vCount + 0].Pos = p->pos + vector3df(p->corners[0].X * w, 0, p->corners[0].Z * h);			
				gVertices[vCount + 1].Pos = p->pos + vector3df(p->corners[1].X * w, 0, p->corners[1].Z * h);		
				gVertices[vCount + 2].Pos = p->pos + vector3df(p->corners[2].X * w, 0, p->corners[2].Z * h);
				gVertices[vCount + 3].Pos = p->pos + vector3df(p->corners[3].X * w, 0, p->corners[3].Z * h);
				
				tVertices[vCount + 0].TCoords = Ps->Tiles[p->tile].tc[0];
				tVertices[vCount + 1].TCoords = Ps->Tiles[p->tile].tc[1];
				tVertices[vCount + 2].TCoords = Ps->Tiles[p->tile].tc[2];
				tVertices[vCount + 3].TCoords = Ps->Tiles[p->tile].tc[3];

				if (EnableModelAlpha)
					p->color.setAlpha(p->color.getAlpha() * ModelAlpha);
				gVertices[vCount + 0].Color = 
				gVertices[vCount + 1].Color = 
				gVertices[vCount + 2].Color = 
				gVertices[vCount + 3].Color = p->color.toSColor();

				vCount += 4;
			}
		}
	}
	else if (Ps->ParticleType == 1)			//sphere particles
	{
		vector3df bv0 = vector3df(-1,1,0);
		vector3df bv1 = vector3df(1,1,0);
		vector3df bv2 = vector3df(1,-1,0);
		vector3df bv3 = vector3df(-1,-1,0);

		for ( T_LiveParticles::iterator i = LiveParticles.begin(); i != LiveParticles.end(); ++i )
		{
			Particle* p = (*i);

			if (p->tile >= Ps->NumTiles || vCount + 4 > ParticleSystemServices->BufferParam.vbuffer0->Size)
				break;

			f32 w = p->size.X * Scale;		f32 h = p->size.Y * Scale;

			gVertices[vCount + 0].Pos = p->pos + vector3df( bv0.X * w, bv0.Y * h, 0);	
			gVertices[vCount + 1].Pos = p->pos + vector3df( bv1.X * w, bv1.Y * h, 0);		
			gVertices[vCount + 2].Pos = p->pos + vector3df( bv2.X * w, bv2.Y * h, 0);
			gVertices[vCount + 3].Pos = p->pos - vector3df( bv3.X * w, bv3.Y * h, 0);

			tVertices[vCount + 0].TCoords = Ps->Tiles[p->tile].tc[0];
			tVertices[vCount + 1].TCoords = Ps->Tiles[p->tile].tc[1];
			tVertices[vCount + 2].TCoords = Ps->Tiles[p->tile].tc[2];
			tVertices[vCount + 3].TCoords = Ps->Tiles[p->tile].tc[3];

			if (EnableModelAlpha)
				p->color.setAlpha(p->color.getAlpha() * ModelAlpha);
			gVertices[vCount + 0].Color = 
				gVertices[vCount + 1].Color = 
				gVertices[vCount + 2].Color = 
				gVertices[vCount + 3].Color = p->color.toSColor();

			vCount += 4;
		}
	}

	SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	unit->drawParam.baseVertIndex = ParticleSystemServices->CurrentOffset;
	unit->drawParam.numVertices = vCount;
	unit->primCount = vCount / 4 * 2;

	//update
	ParticleSystemServices->updateVertices( vCount, unit->drawParam.baseVertIndex );
}

void CParticleSystemSceneNode::onUpdated()
{
	Scale = AbsoluteTransformation.getScale().X;
}

void CParticleSystemSceneNode::setMaterial( SMaterial& material )
{
	if(EnableModelColor)
		material.setMaterialColor(ModelColor);

	if (EnableModelAlpha)
	{
		material.setMaterialAlpha(ModelAlpha);
		if (ModelAlpha < 1.0f)
		{
			switch(material.MaterialType)
			{
			case EMT_SOLID:
				material.MaterialType = EMT_TRANSPARENT_ALPHA_BLEND;
				break;
			case EMT_ALPHA_TEST:
				material.MaterialType = EMT_TRANSAPRENT_ALPHA_BLEND_TEST;
				break;
			}
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
