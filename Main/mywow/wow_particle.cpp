#include "stdafx.h"
#include "wow_particle.h"
#include "mywow.h"
#include "wow_m2instance.h"

void PlaneParticleEmitter::emitParticle( const wow_m2instance* character, const matrix4& world, const EmitterParam& param, Particle* p)
{
	memset(p, 0, sizeof(Particle));

	matrix4 mat;			//¹Ç÷À¾ØÕó

	sys->getBoneMatrix(character, world, mat);

	p->basepos = sys->pos;
	mat.transformVect(p->basepos);

	vector3df pos = sys->pos + vector3df(param.areaWidth * random_minus1_to_1(), 0, param.areaLen * random_minus1_to_1());
	mat.transformVect(pos);
	p->pos = pos - p->basepos;

	vector3df dir;
	if ((sys->flags & 0x20020) == 0x20020)
	{
		dir = vector3df(0,1,0);
 		mat.rotateVect(dir);
	}
	else
	{
		u16 d = (sys->flags & 0xf0000) >> 16;
		switch(d)
		{
		case 6:
		case 4:
		case 2:
			dir = vector3df(0,1,0);
			break;
		default:
			dir = vector3df(0, -1, 0);
			break;
		}
	}
	
	p->dir = dir;
	p->speed =  dir * ( param.speed * (1.0f + param.variation * random_minus1_to_1()) );
	p->rotation = 0;

	p->life = 0;
	p->maxlife = param.maxlife;
	if (p->maxlife == 0)
		p->maxlife = 1;
	p->tile = sys->cols - 1;
}

void SphereParticleEmitter::emitParticle( const wow_m2instance* character, const matrix4& world, const EmitterParam& param, Particle* p )
{
	memset(p, 0, sizeof(Particle));

	vector3df dir;

	matrix4 mat;			//¹Ç÷À¾ØÕó
	sys->getBoneMatrix(character, world, mat);

	p->basepos = sys->pos;
	mat.transformVect(p->basepos);

	vector3df bonedir = vector3df(0,1,0);
	mat.rotateVect(bonedir);
	bonedir.normalize();

	matrix4 spread;
	sys->calcSpreadMatrix(param.vertical, param.horizontal, spread);
	vector3df spreaddir = bonedir;
	spread.rotateVect(spreaddir);
	spreaddir.normalize();
	spreaddir = fixCoordinate(spreaddir);

	p->pos = spreaddir *  (param.areaWidth + param.areaLen) * random_minus1_to_1();

	if ((sys->flags & 0xf04f0) == 0x20420)
		dir = bonedir;
	else 
		dir = spreaddir;

	p->speed = dir * param.speed * (1.0f + param.variation * random_minus1_to_1());
	p->dir = dir;
	p->rotation = 0;

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
	Q_memcpy(colors2, sizeof(vector3df)*3, &fileData[mta.p.colors.ofsKeys], sizeof(vector3df)*3);
	for (u32 i=0; i<3; ++i)
	{
		float opacity = *(s16*)(fileData + mta.p.opacity.ofsKeys + i*2);
		colors[i].set((u32)(opacity/128.0f), (u32)colors2[i].X, (u32)colors2[i].Y, (u32)colors2[i].Z);
		sizes[i] = (*(vector2df*)(fileData + mta.p.sizes.ofsKeys + i*sizeof(vector2df)));
	}

	if (!mta.p.scales.isZero())
	{
		scales[0] = mta.p.scales.X;
		scales[1] = mta.p.scales.Y;
		scales[2] = mta.p.scales.Z;
	}
	else
	{
		scales[0] = scales[1] = scales[2] = 1.0f;
	}

	slowdown = mta.p.slowdown;

	rotation = mta.p.rotation;

	pos = fixCoordinate(mta.pos);
	texture = Mesh->getTexture(mta.texture);
	blend = mta.blend;

	tileRotation = mta.TextureTileRotation;
	switch(tileRotation)
	{
	case 1:
		break;
	case 2:
		rotation = - rotation;
		break;
	default:
		rotation = 0;
		break;
	}

	rows = mta.rows ? mta.rows : 1;
	cols = mta.cols ? mta.cols : 1;

	boneIndex = mta.bone;
	if (boneIndex < 0 || boneIndex > (s32)Mesh->NumBones)
		boneIndex = -1;

	billboard = (mta.flags & MODELPARTICLE_FLAGS_DONOTBILLBOARD) == 0;
	ribbontype = (mta.flags & 0x860000) == 0x40000;
//	followParent = (mta.flags & 0x0100) != 0;
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
		ASSERT(false);
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
	otc[3] = b;
	otc[1].X = b.X; otc[1].Y = a.Y;
	otc[2].X = a.X; otc[2].Y = b.Y;

	tc[0] = otc[0];
	tc[1] = otc[1];
	tc[2] = otc[2];
	tc[3] = otc[3];
}

void ParticleSystem::calcSpreadMatrix( float spread1, float spread2, matrix4& mat )
{
	float a[2];
	mat.makeIdentity();

	a[0] = spread1 * random_minus1_to_1();
	a[1] = spread2 * random_minus1_to_1();
	
	// x, z
	mat.setRotationRadians(vector3df(a[0], 0, a[1]));
}

void ParticleSystem::getBoneMatrix( const wow_m2instance* character, const matrix4& world, matrix4& mat )
{
	if(boneIndex != -1)
		mat = character->DynBones[boneIndex].mat;

	if (billboard || !ribbontype)
		mat = mat * world;
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
	texture = Mesh->getTexture(texindex);

	pos = fixCoordinate(mta.pos);

	numsegs = (int)mta.res;
	seglen = mta.length;
}

