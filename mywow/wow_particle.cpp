#include "stdafx.h"
#include "wow_particle.h"
#include "mywow.h"

void PlaneParticleEmitter::emitParticle( const wow_m2instance* character, u32 anim, u32 time, const EmitterParam& param, Particle* p)
{
	memset(p, 0, sizeof(Particle));

	matrix4 mat;			//¹Ç÷À¾ØÕó

	if(sys->boneIndex != -1)
		mat = character->DynBones[sys->boneIndex].mat;
	
	p->pos = sys->pos + vector3df(randfloat(-param.areaWidth, param.areaWidth), 0, randfloat(-param.areaLen, param.areaLen));
	mat.transformVect(p->pos);
	
	vector3df dir;
	
 	dir = vector3df(0,1,0);
 	mat.rotateVect(dir);
	
	p->dir = dir;
	p->down = vector3df(0, -1, 0);
	p->speed =  dir * ( param.speed * (1.0f + randfloat(-param.variation, param.variation)) );


	if (!sys->billboard)
	{
		f32 radius = 1.0f;
		p->corners[0] = vector3df(-radius, 0, radius);				
		mat.rotateVect(p->corners[0]);
		p->corners[1] = vector3df(radius, 0, radius);
		mat.rotateVect(p->corners[1]);
		p->corners[2] = vector3df(radius, 0, -radius);
		mat.rotateVect(p->corners[2]);
		p->corners[3] = vector3df(-radius, 0, -radius);
		mat.rotateVect(p->corners[3]);
	}

	p->life = 0;
	p->maxlife = param.maxlife;
	if (p->maxlife == 0)
		p->maxlife = 1;
	p->tile = sys->cols - 1;
}

void SphereParticleEmitter::emitParticle( const wow_m2instance* character, u32 anim, u32 time, const EmitterParam& param, Particle* p )
{
	memset(p, 0, sizeof(Particle));

	vector3df dir;
	float radius = randfloat(0, 1);

	matrix4 mat;			//¹Ç÷À¾ØÕó

	if(sys->boneIndex != -1)
		mat = character->DynBones[sys->boneIndex].mat;
	matrix4 spread = sys->calcSpreadMatrix(param.vertical * 2, param.horizontal * 2, param.areaWidth, param.areaLen);

	vector3df bonedir = vector3df(0,1,0);
	mat.rotateVect(bonedir);

	vector3df spreaddir = bonedir;
	spread.rotateVect(spreaddir);
	spreaddir = fixCoordinate(spreaddir);

	p->pos = sys->pos;
	mat.transformVect(p->pos);
	p->pos += (spreaddir * radius);

	if (spreaddir.getLength() == 0 &&
		(sys->flags&0x100) != 0x100)
	{
		p->speed.clear();
		dir = bonedir;
	}
	else
	{
		if (sys->flags & 0x100)
			dir = bonedir;
		else
			dir = spreaddir.normalize();

		p->speed = dir * param.speed * (1.0f + randfloat(-param.variation, param.variation));
	}

	p->dir = dir;
	p->down = vector3df(0, -1, 0);

	p->life = 0;
	p->maxlife = param.maxlife;
	if (p->maxlife == 0)
		p->maxlife = 1;
	p->tile = sys->cols - 1;
}

void ParticleSystem::init( const M2::ModelParticleEmitterDef& mta, const u8* fileData, s32* globalSeq, u32 numGlobalSeq )
{
	emissionSpeed.init(&mta.EmissionSpeed, fileData, globalSeq, numGlobalSeq);
	speedVariation.init(&mta.SpeedVariation, fileData, globalSeq, numGlobalSeq);
	verticalRange.init(&mta.VerticalRange, fileData, globalSeq, numGlobalSeq);
	horizontalRange.init(&mta.HorizontalRange, fileData, globalSeq, numGlobalSeq);
	gravity.init(&mta.Gravity, fileData, globalSeq, numGlobalSeq);
	lifespan.init(&mta.Lifespan, fileData, globalSeq, numGlobalSeq);
	emissionRate.init(&mta.EmissionRate, fileData, globalSeq, numGlobalSeq);
	emissionAreaLength.init(&mta.EmissionAreaLength, fileData, globalSeq, numGlobalSeq);
	emissionAreaWidth.init(&mta.EmissionAreaWidth, fileData, globalSeq, numGlobalSeq);
	deacceleration.init(&mta.Gravity2, fileData, globalSeq, numGlobalSeq);
	enabled.init(&mta.en, fileData, globalSeq, numGlobalSeq);

	vector3df colors2[3];
	memcpy_s(colors2, sizeof(vector3df)*3, &fileData[mta.p.colors.ofsKeys], sizeof(vector3df)*3);
	for (u32 i=0; i<3; ++i)
	{
		float opacity = *(s16*)(fileData + mta.p.opacity.ofsKeys + i*2);
		colors[i].set(opacity/32767.0f, colors2[i].X/255.0f, colors2[i].Y/255.0f, colors2[i].Z/255.0f);
		sizes[i] = (*(vector2df*)(fileData + mta.p.sizes.ofsKeys + i*sizeof(vector2df)));
	}

	slowdown = mta.p.slowdown;
	rotation = mta.p.rotation;
	pos = fixCoordinate(mta.pos);
	texture = Mesh->Textures[mta.texture];
	blend = mta.blend;

	tileRotation = mta.TextureTileRotation;
	rows = mta.rows ? mta.rows : 1;
	cols = mta.cols ? mta.cols : 1;
	ParticleType = mta.ParticleType;

	_ASSERT(ParticleType == 0 || ParticleType == 1);

	boneIndex = mta.bone;
	if (boneIndex < 0 || boneIndex > (s32)Mesh->NumBones)
		boneIndex = -1;

	billboard = (mta.flags & MODELPARTICLE_FLAGS_DONOTBILLBOARD) == 0;
	EmitterType = mta.EmitterType;
	flags = mta.flags;

	switch (EmitterType)
	{
	case MODELPARTICLE_EMITTER_PLANE:
		emitter = new PlaneParticleEmitter(this);
		break;
	case MODELPARTICLE_EMITTER_SPHERE:
		emitter = new SphereParticleEmitter(this);
		break;
	case MODELPARTICLE_EMITTER_SPLINE:
		emitter = new PlaneParticleEmitter(this);
		break;
	default:
		_ASSERT(false);
	}

	NumTiles = rows * cols;
	if (NumTiles > 0)
	{
		Tiles = new TexCoordSet[NumTiles];
		for(u32 i=0; i<NumTiles; ++i)
		{
			TexCoordSet tc;
			initTile(tc.tc, i);
			Tiles[i] = tc;
		}
	}
	
}

void ParticleSystem::initTile( vector2df* tc, int num )
{
	vector2df otc[4];
	vector2df a, b;
	u32 x = num % cols;
	u32 y = num / cols;

	a.X = x * ( 1.0f / cols );
	b.X = (x+1) * ( 1.0f / cols );
	a.Y = y *( 1.0f / rows );
	b.Y = (y+1) * ( 1.0f / rows );

	otc[0] = a;
	otc[2] = b;
	otc[1].X = b.X; otc[1].Y = a.Y;
	otc[3].X = a.X; otc[3].Y = b.Y;

	tc[0] = otc[0];
	tc[1] = otc[1];
	tc[2] = otc[2];
	tc[3] = otc[3];
}

matrix4 ParticleSystem::calcSpreadMatrix( float spread1, float spread2, float w, float l )
{
	float a[2];
	matrix4		ret;

	a[0] = randfloat(-spread1, spread1) / 2.0f;
	a[1] = randfloat(-spread2, spread2) / 2.0f;
	
	// x, z
	ret.setRotationRadians(vector3df(a[0], 0, a[1]));

	// scale
	float size = abs(cos(a[0])) * l + abs(sin(a[0])) * w;
	ret.setScale(size);

	return ret;
}

void RibbonEmitter::init( const M2::ModelRibbonEmitterDef& mta, const u8* fileData, s32* globalSeq, u32 numGlobalSeq)
{
	color.init(&mta.color, fileData, globalSeq, numGlobalSeq);
	opacity.init(&mta.opacity, fileData, globalSeq, numGlobalSeq);
	above.init(&mta.above, fileData, globalSeq, numGlobalSeq);
	below.init(&mta.below, fileData, globalSeq, numGlobalSeq);

	boneIndex = mta.bone;
	parent = &Mesh->Bones[mta.bone];
	int texindex = *(int*)&fileData[mta.ofsTextures];
	texture = Mesh->Textures[texindex];

	pos = fixCoordinate(mta.pos);

	numsegs = (int)mta.res;
	seglen = mta.length;
}

