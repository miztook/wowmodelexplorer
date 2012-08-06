#include "stdafx.h"
#include "wow_character.h"
#include "mywow.h"
#include "CImage.h"
#include "CFileM2.h"

const char* regionTextureItemPaths[NUM_REGIONS] =
{
	"",																				//base
	"Item\\TextureComponents\\ArmUpperTexture\\",
	"Item\\TextureComponents\\ArmLowerTexture\\",
	"Item\\TextureComponents\\HandTexture\\",
	"",																				//face upper
	"",																				//face lower
	"Item\\TextureComponents\\TorsoUpperTexture\\",
	"Item\\TextureComponents\\TorsoLowerTexture\\",
	"Item\\TextureComponents\\LegUpperTexture\\",
	"Item\\TextureComponents\\LegLowerTexture\\",
	"Item\\TextureComponents\\FootTexture\\"
};

int slotModels[] =
{
	CS_HEAD,
	CS_SHOULDER,
	CS_HAND_LEFT,
	CS_HAND_RIGHT,
	CS_QUIVER
};

const char* regionHeadPath = "Item\\ObjectComponents\\Head\\";
const char* regionShoulderPath = "Item\\ObjectComponents\\Shoulder\\";
const char* regionShieldPath = "Item\\ObjectComponents\\Shield\\";
const char* regionWeaponPath =  "Item\\ObjectComponents\\Weapon\\";
const char* regionQuiverPath = "Item\\ObjectComponents\\Quiver\\";
const char* regionCapePath = "Item\\ObjectComponents\\Cape\\";

CharTexture::CharTexture()
	: TexPartCount(0)
{
	TextureParts = (CharTexturePart*)Hunk_AllocateTempMemory(sizeof(CharTexturePart) * MAX_TEX_PART_SIZE);
}

CharTexture::~CharTexture()
{
	Hunk_FreeTempMemory(TextureParts);
}

void CharTexture::addLayer( const c8* name, s32 region, s32 layer )
{
	if (name == NULL || strlen(name) == 0)
		return;

	CharTexturePart* part = &TextureParts[TexPartCount++];
	strcpy_s( part->Name, MAX_PATH,  name);
	part->Region = region;
	part->Layer = layer;

	_ASSERT(TexPartCount < MAX_TEX_PART_SIZE);
}

bool CharTexture::addItemLayer( const c8* name, s32 region, u32 gender, s32 layer )
{
	if (name == NULL || strlen(name) == 0)
		return false;

	c8 outname[MAX_PATH];
	bool success = makeItemTexture(region, gender, name, outname);
	if (success)
	{
		CharTexturePart* part = &TextureParts[TexPartCount++];
		part->Region = region;
		part->Layer = layer;
		strcpy_s(part->Name, MAX_PATH, outname);
	}

	return success;
}

bool CharTexture::makeItemTexture(s32 region, u32 gender, const c8* name, c8* outname)
{
	if (strlen(name) < 3)
	{
		strcpy_s(outname, MAX_PATH, "");
		return false;
	}

	strcpy_s(outname, MAX_PATH, regionTextureItemPaths[region]);
	strcat_s(outname, MAX_PATH, name);
	strcat_s(outname, MAX_PATH, "_U.blp");

	bool bOk = false;

	bOk = g_Engine->getWowEnvironment()->exists(outname);
	if(bOk)
		return true;

	//尝试另一个版本, _U 换成 _F _M
	if (strlen(outname) < 5)
	{
		strcpy_s(outname, MAX_PATH, "");
		return false;
	}

	outname[strlen(outname)-5] = gender ? 'F' : 'M';
	bOk = g_Engine->getWowEnvironment()->exists(outname);
	if(bOk)
		return true;

	return false;
}

ITexture* CharTexture::compose()
{
	heapsort<CharTexturePart>(TextureParts, TexPartCount);

	u32* destData = (u32*)Hunk_AllocateTempMemory(REGION_PX * REGION_PX * sizeof(u32));
	memset(destData, 0xff, REGION_PX * REGION_PX * sizeof(u32));

	for (u32 i=0; i<TexPartCount; ++i)
	{
		CharTexturePart* part = &TextureParts[i];
		const CharRegionCoords coords = regions[part->Region];
		IImage* image = g_Engine->getResourceLoader()->loadBLP(part->Name);

		if(image)
		{
			IImage* newImage = image;
			u32* tmpData;			//临时image
			bool needScale = image->getDimension().Width != coords.xsize ||
				image->getDimension().Height != coords.ysize;			//扩大
		
			if (needScale)
			{
				tmpData = (u32*)Hunk_AllocateTempMemory(coords.xsize * coords.ysize * sizeof(u32));
				newImage = new CImage(ECF_A8R8G8B8, dimension2du(coords.xsize, coords.ysize), tmpData, false);
				image->copyToScaling(newImage);
			}

			u32 width = coords.xsize;
			u32 height = coords.ysize;

			_ASSERT(newImage->getDimension().Width == coords.xsize &&
				newImage->getDimension().Height == coords.ysize );

			u32 srcX, srcY, dstX, dstY;
			for ( srcY=0, dstY=coords.ypos; srcY<height; ++srcY, ++dstY)
			{
				for ( srcX=0, dstX=coords.xpos; srcX<width; ++srcX, ++dstX )
				{
					SColor src = newImage->getPixel(srcX, srcY);
					u32* dst = destData + dstY*REGION_PX + dstX;

					SColor d(*dst);
					d = SColor::interpolate(src, d, 1 - src.getAlpha() / 255.0f, true);

					*dst = d.color;
				}
			}

			if (needScale)
			{
				newImage->drop();
				Hunk_FreeTempMemory(tmpData);
			}

			image->drop();
		}
	}

	CImage* composedImage = new CImage(ECF_A8R8G8B8, dimension2du(REGION_PX, REGION_PX), destData, false);
	ITexture* tex = g_Engine->getManualTextureServices()->createTextureFromImage(composedImage, true);
	composedImage->drop();

	Hunk_FreeTempMemory(destData);

	return tex;
}

wow_character::wow_character( IFileM2* m2, bool npc )
	: Mesh(m2), HasRobe(false), DeathKnight(false), IsNpc(npc), ShowCape(true), CapeID(1), Race(0), BlinkIndex(-1),
	Blink(false), CloseLHand(false), CloseRHand(false), ShowParticles(true), ShowRibbons(true),
	TextureAnimHints(NULL), ColorHints(NULL)
{
	LastBoneAnim = LastColorAnim = LastTextureAnim = -1;
	LastBoneTime = LastColorTime = LastTextureTime = -1;

	for (u32 i=0; i<NUM_TEXTURETYPE; ++i)
		ReplaceTextures[i] = NULL;

	CurrentSkin = Mesh->Skin;

	InitializeListHead(&VisibleGeosetList);

	DynBones = new SDynBone[Mesh->NumBones];
	Calcs = new bool[Mesh->NumBones];
	BoneHints = new SHint[Mesh->NumBones];
	ShowAttachments = new bool[Mesh->NumAttachments];
	::memset(ShowAttachments, 0, sizeof(bool) * Mesh->NumAttachments);

	DynGeosets = NULL;
	if (CurrentSkin)				//有skin
	{
		DynGeosets = new SDynGeoset[CurrentSkin->NumGeosets];
		memset(DynGeosets, 0, sizeof(SDynGeoset) * CurrentSkin->NumGeosets);

		for (u32 i=0; i<CurrentSkin->NumGeosets; ++i)
		{
			CGeoset* set = &CurrentSkin->Geosets[i];
			DynGeosets[i].Index = i;
			DynGeosets[i].NumUnits = set->BoneUnits.size();
			DynGeosets[i].Units = new SDynGeoset::SUnit[DynGeosets[i].NumUnits];
			for (CGeoset::T_BoneUnitList::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[i].Units[itr->Index];
				unit->BoneMats = new matrix4A16[(*itr).BoneCount];
				unit->BoneMatrixArray = new SBoneMatrixArray(
					unit->BoneMats, 
					(*itr).BoneCount,
					set->MaxWeights);
			}
		}

		ColorHints = new SColorHint[CurrentSkin->NumGeosets];
		TextureAnimHints = new SHint[CurrentSkin->NumGeosets];
	}
	
	switch (Mesh->getType())
	{
	case MT_CHARACTER:
		initCharacter();	
		BlinkIndex = getBlinkGeosetIndex();
		break;
	}

	//color, alpha
	EnableModelColor = false;
	EnableModelAlpha = false;
	ModelAlpha = 1.0f;
}

wow_character::~wow_character()
{
	delete[] ColorHints;
	delete[] TextureAnimHints;
	delete[] DynGeosets;

	delete[] ShowAttachments;
	delete[] BoneHints;
	delete[] Calcs;
	delete[] DynBones;

	for (u32 i=0; i<NUM_TEXTURETYPE; ++i)
		if(ReplaceTextures[i])
			ReplaceTextures[i]->drop();
}

void wow_character::initCharacter()
{
	bool isChar = g_Engine->getWowDatabase()->getRaceGender(Mesh->Name, Race, Gender);
	_ASSERT(isChar);

	resetCharacterLook();
}

void wow_character::resetCharacterLook()
{
	SkinColor = 0;
	FaceType = 0;
	HairColor = 0;
	HairStyle = 1;
	FacialHair = 0;
	
	HelmHideHair = false;

	for (s32 i=0; i<NUM_CHAR_SLOTS; ++i)
		Equipments[i] = 0;
}

void wow_character::updateCharacter()
{
	if (Mesh->getType() != MT_CHARACTER)
		return;

	for (u32 i=0; i<NUM_GEOSETS; ++i)
	{
		Geosets[i] = 1;
		CapeID = 1;
	}
	Geosets[CG_HAIRSTYLE] = 0;
	Geosets[CG_GEOSET100] = Geosets[CG_GEOSET200] = Geosets[CG_GEOSET300] = 0;

	//update texture
	CharTexture	charTex;
	
	//纹理装备
	dressupCharacter(charTex);

	//组合纹理
	setReplaceTexture(TEXTURE_BODY, charTex.compose());

	buildVisibleGeosets();
}

void wow_character::animateBones( u32 anim, u32 time,  u32 lastingtime, f32 blend, const matrix4* billboardRot)
{
	if (LastBoneAnim == anim && time == LastBoneTime)
		return;

	if (blend < 0)
		blend = 1.0f;

	::memset(Calcs, 0, sizeof(bool)*Mesh->NumBones);

	if (LastBoneAnim != anim || time < LastBoneTime)			//在anim改变或时间轴倒退时清除hint
		::memset(BoneHints, 0, sizeof(SHint)*Mesh->NumBones);
	
	if (CurrentSkin)			//有skin
	{
		if (Mesh->getType() == MT_CHARACTER)
		{		
			u32 closeFistIndex = Mesh->AnimationLookup[ANIMATION_HANDSCLOSED];

			for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
			{
				u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

				if (DynGeosets[c].NoAlpha)
					continue;

				u32 realAnim = anim;
				u32 realTime = time;

				//eye blink
				if ((s32)c == BlinkIndex)
				{
					realAnim = 0;
					realTime = lastingtime;
				}

				CGeoset* set = &CurrentSkin->Geosets[c];
				for (CGeoset::T_BoneUnitList::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
				{
					SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
					unit->Enable = true;
					for(u8 k=0; k <(*itr).BoneCount; ++k)
					{
						u8 idx = (*itr).local2globalMap[k];
						SModelBone* b = &Mesh->Bones[idx];

						if ((CloseLHand && b->bonetype == EBT_LEFTHAND) ||
							(CloseRHand && b->bonetype == EBT_RIGHTHAND))
						{
							calcBone(idx, closeFistIndex, 0, blend, true, NULL);				
							unit->BoneMats[k] = *(const matrix4A16*)DynBones[idx].mat.pointer();
						}
						else
						{
							calcBone(idx, realAnim, realTime, blend, true, billboardRot);
							unit->BoneMats[k] = *(const matrix4A16*)DynBones[idx].mat.pointer();
						}				
					}
				}
			}
		}
		else			//not character
		{
			for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
			{
				u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

				if (DynGeosets[c].NoAlpha)
					continue;

				CGeoset* set = &CurrentSkin->Geosets[c];
				for (CGeoset::T_BoneUnitList::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
				{
					SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
					unit->Enable = true;
					for(u8 k=0; k <(*itr).BoneCount; ++k)
					{
						u8 idx = (*itr).local2globalMap[k];

						SModelBone* b = &Mesh->Bones[idx];
						calcBone(idx, anim, time, blend, true, billboardRot);
						unit->BoneMats[k] = *(const matrix4A16*)DynBones[idx].mat.pointer();
					}
				}
			}
		}
	}
		
	//attachments
	for (u32 c=0; c<Mesh->NumAttachments; ++c)
	{
		if (ShowAttachments[c])
		{
			s32 i = Mesh->Attachments[c].boneIndex;
			if(i != -1)
				calcBone(i, anim, time, blend, false, billboardRot);
		}
	}

	//particles
	if (ShowParticles)
	{
		for (u32 c=0; c<Mesh->NumParticleSystems; ++c)
		{
			s16 i = Mesh->ParticleSystems[c].boneIndex;
			if(i != -1)
				calcBone((u8)i, anim, time, blend, false, billboardRot);
		}
	}

	//ribbons
	if (ShowRibbons)
	{
		for (u32 c=0; c<Mesh->NumRibbonEmitters; ++c)
		{
			s16 i = Mesh->RibbonEmitters[c].boneIndex;
			if(i != -1)
				calcBone((u8)i, anim, time, blend, false, billboardRot);
		}
	}

	LastBoneAnim = anim;
	LastBoneTime = time;
}

void wow_character::animateBones( const matrix4* billboardRot )
{
	::memset(Calcs, 0, sizeof(bool)*Mesh->NumBones);

	if (CurrentSkin)			//有skin
	{
		for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
		{
			u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

			if (DynGeosets[c].NoAlpha)
				continue;

			CGeoset* set = &CurrentSkin->Geosets[c];
			for (CGeoset::T_BoneUnitList::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
				unit->Enable = true;
				for(u8 k=0; k <(*itr).BoneCount; ++k)
				{
					u8 idx = (*itr).local2globalMap[k];
					calcBone(idx, billboardRot);
					unit->BoneMats[k] = *(const matrix4A16*)DynBones[idx].mat.pointer();
				}
			}
		}
	}

}

void wow_character::disableBones()
{
	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
	{
		u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

		if (DynGeosets[c].NoAlpha)
			continue;

		CGeoset* set = &CurrentSkin->Geosets[c];
		for (CGeoset::T_BoneUnitList::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
		{
			SDynGeoset::SUnit* unit = &DynGeosets[c].Units[itr->Index];
			unit->Enable = false;
		}
	}
}

void wow_character::calcBone( u8 i, u32 anim, u32 time, f32 blend, bool enableScale, const matrix4* billboardRot)
{
	if (Calcs[i])
		return;

	_ASSERT(i < Mesh->NumBones);
	SModelBone* b = &Mesh->Bones[i];

	matrix4 global, inverseGlobal;
	global.setTranslation(b->pivot);
	inverseGlobal.setTranslation(-b->pivot);

	vector3df t, s;
	quaternion r;

	t.set(0,0,0); 
	s.set(1,1,1);
	r.set(0,0,0,1);

	bool _trans = false;
	bool _rot = false;
	bool _scale = false;

	bool soft = blend < 1.0f;
	
	 BoneHints[i].transHint = Mesh->Bones[i].trans.getValue(anim, time, t, BoneHints[i].transHint);
	_trans =  BoneHints[i].transHint != -1;

	if(soft && _trans)
		t = vector3df::interpolate(DynBones[i].trans, t, blend);
	DynBones[i].trans = t;

	BoneHints[i].rotHint = Mesh->Bones[i].rot.getValue(anim, time, r, BoneHints[i].rotHint);
	_rot = BoneHints[i].rotHint != -1; 
	if(soft && _rot)
		r = quaternion::slerp(DynBones[i].rot, r, blend);
	DynBones[i].rot = r;
	
	if (enableScale)
	{
		BoneHints[i].scaleHint = Mesh->Bones[i].scale.getValue(anim, time, s, BoneHints[i].scaleHint);
		_scale = BoneHints[i].scaleHint != -1;
		if(soft && _scale)
			s = vector3df::interpolate(DynBones[i].scale, s, blend);
		DynBones[i].scale = s;
	}
	
	matrix4 m;

	if (b->billboard && billboardRot)
	{
		m = (*billboardRot);
	}
	else
	{							//变换是基于pivot空间的
		if (_rot)
			r.getRotationMatrix(m);
		if (_trans)
			m.setTranslation(t);
		if	(enableScale && _scale)
			m.setScale(s);		
	}

	m = global * m * inverseGlobal;		//pivot

	if (b->parent != -1)
	{
		calcBone(b->parent, anim, time, blend, enableScale, NULL);
		
		DynBones[i].mat = DynBones[b->parent].mat * m;
	}
	else
	{
		DynBones[i].mat = m;
	}

	DynBones[i].transPivot =b->pivot;
	DynBones[i].mat.transformVect(DynBones[i].transPivot);

	Calcs[i]= true;
}

void wow_character::calcBone( u8 i, const matrix4* billboardRot )
{
	_ASSERT(i < Mesh->NumBones);
	SModelBone* b = &Mesh->Bones[i];

	if (Calcs[i] || !b->billboard)
		return;

	matrix4 global, inverseGlobal;
	global.setTranslation(b->pivot);
	inverseGlobal.setTranslation(-b->pivot);

	matrix4 m;

	if (billboardRot)
	{
		m = (*billboardRot);
	}

	m = global * m * inverseGlobal;		//pivot

	DynBones[i].mat = m;
	DynBones[i].transPivot =b->pivot;
	DynBones[i].mat.transformVect(DynBones[i].transPivot);

	Calcs[i]= true;
}

void wow_character::animateColors(u32 anim, u32 time)
{
	if ( !CurrentSkin || (LastColorAnim == anim && time == LastColorTime))
		return;

	if (LastColorAnim != anim || time < LastColorTime)			//在anim改变或时间轴倒退时清除hint
		::memset(ColorHints, 0, sizeof(SColorHint)*CurrentSkin->NumGeosets);

	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
	{
		u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

		u32 realAnim = anim;

		//eye blink
		if ((s32)c == BlinkIndex)
		{
			realAnim = 0;
		}

		DynGeosets[c].NoAlpha = false;

		CGeoset* set = &CurrentSkin->Geosets[c];
		s16 colorIndex = set->getTexUnit(0)->ColorIndex;
		s16 transIndex = set->getTexUnit(0)->TransIndex;

		SColorf colorf;

		if (colorIndex == -1)
		{
			colorf.set(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			vector3df v;
			float alpha;

			//color
			ColorHints[c].colorHint = Mesh->Colors[colorIndex].colorAnim.getValue(0, time, v, ColorHints[c].colorHint); 
			if( -1 != ColorHints[c].colorHint)
				colorf.set(v.X, v.Y, v.Z);

			//alpha
			ColorHints[c].opacityHint = Mesh->Colors[colorIndex].opacityAnim.getValue(realAnim, time, alpha, ColorHints[c].opacityHint);
 			if ( -1 != ColorHints[c].opacityHint)
 				colorf.a = alpha;
		}		
		
		if (transIndex != -1)
		{
			u16 v;

			ColorHints[c].transparencyHint = Mesh->Transparencies[transIndex].tranAnim.getValue(realAnim, time, v, ColorHints[c].transparencyHint);
			if (-1 != ColorHints[c].transparencyHint)
			{
				colorf.a *= (v / 32767.0f);
			}
		}
	
		DynGeosets[c].emissive = colorf.toSColor();
		DynGeosets[c].NoAlpha = ((s32)c == BlinkIndex && !Blink) ? true : colorf.a < 0.01f;
	}

	LastColorAnim = anim;
	LastColorTime = time;
}

void wow_character::solidColors()
{
	if (!CurrentSkin)
		return;

	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
	{
		u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

		DynGeosets[c].emissive = SColor(0xffffffff);
		DynGeosets[c].NoAlpha = false;
	}
}

void wow_character::animateTextures( u32 anim, u32 time )
{
	if ( !CurrentSkin || (LastTextureAnim == anim && time == LastTextureTime))
		return;

	if (LastTextureAnim != anim || time < LastTextureTime)			//在anim改变或时间轴倒退时清除hint
		::memset(TextureAnimHints, 0, sizeof(SHint)*CurrentSkin->NumGeosets);

	for (PLENTRY p = VisibleGeosetList.Flink; p != &VisibleGeosetList; p = p->Flink)
	{
		u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

		if (DynGeosets[c].NoAlpha)
			continue;

		calcTextureAnim(c, anim, time);
	}

	LastTextureAnim = anim;
	LastTextureTime = time;
}

void wow_character::calcTextureAnim( u32 c, u32 anim, u32 time )
{
	CGeoset* set = &CurrentSkin->Geosets[c];
	s16 texAnimIndex = set->getTexUnit(0)->TexAnimIndex;
	if(texAnimIndex == -1)
	{
		DynGeosets[c].UseTextureAnim = false;
		return;
	}

	SModelTextureAnim& textureAnim = Mesh->TextureAnim[texAnimIndex];

	matrix4 mat;

	vector3df s, t, r;

	bool _trans = false;
	bool _rot = false;
	bool _scale = false;

	TextureAnimHints[c].scaleHint = textureAnim.scale.getValue(anim, time, s, TextureAnimHints[c].scaleHint);
	TextureAnimHints[c].transHint = textureAnim.trans.getValue(anim, time, t, TextureAnimHints[c].transHint); 
	TextureAnimHints[c].rotHint = textureAnim.rot.getValue(anim, time, r, TextureAnimHints[c].rotHint);

	_scale = -1 != TextureAnimHints[c].scaleHint;
	_trans = -1 != TextureAnimHints[c].transHint;
	_rot = -1 != TextureAnimHints[c].rotHint;

	if (_scale || _trans || _rot)
	{
		mat.makeIdentity();
		DynGeosets[c].UseTextureAnim = true;
	}

	if (_scale)
	{
		if (s.X != 0 || s.Y != 0)
		{
			mat.m00 = s.X;
			mat.m11 = s.Y;
			mat.m20 = (-0.5f * s.X) + 0.5f;
			mat.m21 = (-0.5f * s.Y) + 0.5f;
		}
	}

	if (_trans)
	{
		if (t.X != 0 || t.Y != 0)
		{
			matrix4 trans;
			trans.m20 = t.X;
			trans.m21 = t.Y;
			mat *= trans;
		}
	}

	if (_rot )
	{
		if (r.X != 0.0f)
		{
			matrix4 rot;

			f32 theta = r.X;
			f32 cosTheta = cos(theta);
			f32 sinTheta = sin(theta);

			rot.m00 = cosTheta;
			rot.m01 = sinTheta;
			rot.m10 = -sinTheta;
			rot.m11 = cosTheta;

			rot.m20 = 0.5f + (0.5f * (sinTheta - cosTheta));
			rot.m21 = 0.5f + (-0.5f * (sinTheta + cosTheta));

			mat *= rot;
		}
	}
	
	DynGeosets[c].TextureMatrix = mat;
}

void wow_character::updateEquipments( s32 slot, s32 itemid )
{
	_ASSERT(slot >= 0 && slot < NUM_CHAR_SLOTS);
	if (slot >= 0 && slot < NUM_CHAR_SLOTS)
		Equipments[slot]= itemid;
}

s32 wow_character::getItemSlot( s32 itemid )
{
	const SItemRecord* rec = g_Engine->getWowDatabase()->getItemById(itemid);
	if (rec)
		return getSlot(rec->type);
	return -1;
}

bool wow_character::slotHasModel( s32 slot )
{
	return slot==CS_HEAD || slot == CS_SHOULDER || slot == CS_HAND_LEFT ||
		slot == CS_HAND_RIGHT || slot == CS_QUIVER;
}

bool wow_character::updateNpc(s32 npcid)
{
	const SNPCRecord* npcRec = g_Engine->getWowDatabase()->getNPCById(npcid);
	if (!npcRec)
		return false;

	dbc::record rSkin = g_Engine->getWowDatabase()->getCreatureDisplayInfoDB()->getByID(npcRec->model);
	if (!rSkin.isValid())
		return false;

	u32 modelId = rSkin.getUInt(creatureDisplayInfoDB::ModelID);
	u32 npcId = rSkin.getUInt(creatureDisplayInfoDB::NPCID);

	if (Mesh->getType() != MT_CHARACTER)
	{
		dbc::record r = g_Engine->getWowDatabase()->getCreatureModelDB()->getByID(modelId);
		if (!r.isValid())
			return false;

		const c8* filename = r.getString(creatureModelDB::Filename);
		if (!filename || strlen(filename) < 4 || strlen(filename) > 255)
			return false;

		c8 path[MAX_PATH];
		getFileDirA(filename, path, MAX_PATH);
		strcat_s(path, MAX_PATH, "\\");

		{
			const c8* t = rSkin.getString(creatureDisplayInfoDB::Skin );
			if (strlen(t) > 0)
			{
				string256 texPath = path;
				texPath.append(t);
				texPath.append(".blp");
				setReplaceTexture(TEXTURE_GAMEOBJECT1, g_Engine->getResourceLoader()->loadTexture(texPath.c_str()));
			}

			t = rSkin.getString(creatureDisplayInfoDB::Skin + 1);
			if (strlen(t) > 0)
			{
				string256 texPath = path;
				texPath.append(t);
				texPath.append(".blp");
				setReplaceTexture(TEXTURE_GAMEOBJECT2, g_Engine->getResourceLoader()->loadTexture(texPath.c_str()));
			}

			t = rSkin.getString(creatureDisplayInfoDB::Skin + 2);
			if (strlen(t) > 0)
			{
				string256 texPath = path;
				texPath.append(t);
				texPath.append(".blp");
				setReplaceTexture(TEXTURE_GAMEOBJECT3, g_Engine->getResourceLoader()->loadTexture(texPath.c_str()));
			}
		}

		buildVisibleGeosets();
	}
	else			//character
	{
		dbc::record r = g_Engine->getWowDatabase()->getCreatureDisplayInfoExtraDB()->getByID(npcId);

		_ASSERT(r.isValid());
		if (!r.isValid())
			return false;

		SkinColor = r.getUInt(creatureDisplayInfoExtraDB::SkinColor);
		FaceType = r.getUInt(creatureDisplayInfoExtraDB::Face);
		HairColor = r.getUInt(creatureDisplayInfoExtraDB::HairColor);
		HairStyle = r.getUInt(creatureDisplayInfoExtraDB::HairStyle);
		FacialHair = r.getUInt(creatureDisplayInfoExtraDB::FacialHair);

		Equipments[CS_HEAD] = r.getUInt(creatureDisplayInfoExtraDB::HelmID);
		Equipments[CS_SHOULDER] = r.getUInt(creatureDisplayInfoExtraDB::ShoulderID);
		Equipments[CS_SHIRT]  =	r.getUInt(creatureDisplayInfoExtraDB::ShirtID);	
		Equipments[CS_CHEST] = r.getUInt(creatureDisplayInfoExtraDB::ChestID);
		Equipments[CS_BELT] = r.getUInt(creatureDisplayInfoExtraDB::BeltID);
		Equipments[CS_PANTS] = r.getUInt(creatureDisplayInfoExtraDB::PantsID);
		Equipments[CS_BOOTS] = r.getUInt(creatureDisplayInfoExtraDB::BootsID);
		Equipments[CS_BRACERS] = r.getUInt(creatureDisplayInfoExtraDB::BracersID);
		Equipments[CS_GLOVES] = r.getUInt(creatureDisplayInfoExtraDB::GlovesID);
		Equipments[CS_CAPE] = r.getUInt(creatureDisplayInfoExtraDB::CapeID);
		Equipments[CS_TABARD] = r.getUInt(creatureDisplayInfoExtraDB::TabardID);
	}
	
	return true;
}

void wow_character::setReplaceTexture( ETextureTypes type, ITexture* texture )
{
	int idx = (int)type;

	if (ReplaceTextures[idx])
		ReplaceTextures[idx]->drop();

	ReplaceTextures[idx] = texture;
	if (ReplaceTextures[idx])
	{
		ReplaceTextures[idx]->createVideoTexture();
	}
}

void wow_character::dressupCharacter(CharTexture& charTex)
{
	//base
	{
		dbc::record r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::SkinType, 0, SkinColor);
		if(!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::SkinType, 0, 0);
		if (r.isValid())
		{
			const c8* t1 = r.getString(charSectionsDB::Tex1);
			const c8* t2 = r.getString(charSectionsDB::Tex2);

			//base
			charTex.addLayer(t1, CR_BASE, 0);

			if (strlen(t2))			//tauren fur
			{
				ITexture* tex = g_Engine->getResourceLoader()->loadTexture(t2);
				setReplaceTexture(TEXTURE_FUR, tex);
			}
		}
	}

	//underwear
	{
		dbc::record r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::UnderwearType, 0, SkinColor);
		if(!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::UnderwearType, 0, 0);
		if (r.isValid())
		{
			const c8* t1 = r.getString(charSectionsDB::Tex1);
			const c8* t2 = r.getString(charSectionsDB::Tex2);

			charTex.addLayer(t1, CR_PELVIS_UPPER, 1);		//pants
			charTex.addLayer(t2, CR_TORSO_UPPER, 1);		//top
		}
	}

	//face
	{
		dbc::record r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::FaceType, FaceType, SkinColor);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::FaceType, FaceType, 0);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::FaceType, 0, 0);
		if (r.isValid())
		{
			const c8* t1 = r.getString(charSectionsDB::Tex1);
			const c8* t2 = r.getString(charSectionsDB::Tex2);

			charTex.addLayer(t1, CR_FACE_LOWER, 1);
			charTex.addLayer(t2, CR_FACE_UPPER, 1);
		}
	}

	//facial hair
	{
		//texture
		dbc::record r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::FacialHairType, FacialHair, HairColor);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender,charSectionsDB::FacialHairType, FacialHair, 0);
		if (!r.isValid())
			r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::FacialHairType, 0, 0);
		if (r.isValid())
		{
			const c8* t1 = r.getString(charSectionsDB::Tex1);
			const c8* t2 = r.getString(charSectionsDB::Tex2);

			charTex.addLayer(t1, CR_FACE_LOWER, 2);
			charTex.addLayer(t2, CR_FACE_UPPER, 2);
		}
	}	

	//hair texture
	dbc::record r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::HairType, HairStyle, HairColor);
	if (!r.isValid())
		r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::HairType, HairStyle, 0);
	if (!r.isValid())
		r = g_Engine->getWowDatabase()->getCharSectionDB()->getByParams(Race, Gender, charSectionsDB::HairType, 0, 0);
	if (r.isValid())
	{
		const c8* t1 = r.getString(charSectionsDB::Tex1);
		if (strlen(t1) == 0)
		{
			string256 texname;
			string256 s = Mesh->Dir;
			if (Gender == 1)
				s.subString(0, strlen(Mesh->Dir)-7, texname);
			else
				s.subString(0, strlen(Mesh->Dir)-5, texname);

			c8 hairname[32];
			sprintf_s(hairname, 32, "Hair00_%02d.blp", HairColor);
			texname.append(hairname);
			t1 = texname.c_str();
		}

 		ITexture* tex = g_Engine->getResourceLoader()->loadTexture(t1);
 		setReplaceTexture(TEXTURE_HAIR, tex);	

		const c8* t2 = r.getString(charSectionsDB::Tex2);
		const c8* t3 = r.getString(charSectionsDB::Tex3);

		charTex.addLayer(t2, CR_FACE_LOWER, 3);
		charTex.addLayer(t3, CR_FACE_UPPER, 3);	
	}	

	//clothes

	if (Equipments[CS_CHEST] != 0)
	{
		const SItemRecord* item = g_Engine->getWowDatabase()->getItemById(Equipments[CS_CHEST]);
		if (item)
		{
			HasRobe = item->type == IT_ROBE;
		}
	}

	if (Equipments[CS_PANTS] != 0)
	{
		const SItemRecord* item = g_Engine->getWowDatabase()->getItemById(Equipments[CS_PANTS]);
		if (item && item->type == IT_PANTS)
		{
			dbc::record r = g_Engine->getWowDatabase()->getItemDB()->getByID(item->id);
			_ASSERT(r.isValid());
			if (r.getUInt(itemDisplayDB::RobeGeosetFlags) == 1)
				HasRobe = true;
		}
	}

	for (u32 i=0; i<NUM_CHAR_SLOTS; ++i)
	{
		if (Equipments[i] != 0)
			addClothesEquipment(i, Equipments[i], getClothesSlotLayer(i), charTex, !IsNpc);
	}

}


void wow_character::addClothesEquipment( s32 slot, s32 itemnum, s32 layer, CharTexture& tex, bool lookup/*=true*/ )
{
	if (slot == CS_PANTS && Geosets[CG_TROUSERS]==2)
		return;

	s32 itemID = 0;
	s32 type = 0;
	if (lookup)
	{
		const SItemRecord* item = g_Engine->getWowDatabase()->getItemById(itemnum);
		type = item->type;
		itemID = item->model;
	}
	else
	{
		itemID = itemnum;
	}
	
	dbc::record r = g_Engine->getWowDatabase()->getItemDisplayDB()->getByID(itemID);
	_ASSERT(r.isValid());
	if (r.getUInt(itemDisplayDB::RobeGeosetFlags) == 1)
		HasRobe = true;

	if (slot == CS_CHEST || slot == CS_SHIRT)
	{
		Geosets[CG_WRISTBANDS] = 1 + r.getUInt(itemDisplayDB::GloveGeosetFlags);

		//
		const c8* armUpper = r.getString(itemDisplayDB::TexArmUpper);
		const c8* armLower = r.getString(itemDisplayDB::TexArmLower);
		const c8* torsoUpper = r.getString(itemDisplayDB::TexChestUpper);
		const c8* torsoLower = r.getString(itemDisplayDB::TexChestLower);

		tex.addItemLayer(armUpper, CR_ARM_UPPER, Gender, layer);
		tex.addItemLayer(armLower, CR_ARM_LOWER, Gender, layer);

		tex.addItemLayer(torsoUpper, CR_TORSO_UPPER, Gender, layer);
		tex.addItemLayer(torsoLower, CR_TORSO_LOWER, Gender, layer);

		if(type == IT_ROBE || r.getUInt(itemDisplayDB::RobeGeosetFlags)==1)
		{
			const c8* legUpper = r.getString(itemDisplayDB::TexLegUpper);
			const c8* legLower = r.getString(itemDisplayDB::TexLegLower);

			tex.addItemLayer(legUpper, CR_LEG_UPPER, Gender, layer);
			tex.addItemLayer(legLower, CR_LEG_LOWER, Gender, layer);
		}
	}
	else if (slot == CS_BELT)
	{
		const c8* torsolower = r.getString(itemDisplayDB::TexChestLower);
		const c8* legUpper = r.getString(itemDisplayDB::TexLegUpper);

		tex.addItemLayer(torsolower, CR_TORSO_LOWER, Gender, layer);
		tex.addItemLayer(legUpper, CR_LEG_UPPER, Gender, layer);
	}
	else if (slot == CS_BRACERS)
	{
 		const c8* armLower = r.getString(itemDisplayDB::TexArmLower);
 		tex.addItemLayer(armLower, CR_ARM_LOWER, Gender, layer);
	}
	else if (slot == CS_PANTS)
	{
		Geosets[CG_PANTS2] = 1 + r.getUInt(itemDisplayDB::GloveGeosetFlags);
		Geosets[CG_KNEEPADS] = 1 + r.getUInt(itemDisplayDB::BracerGeosetFlags);

		const c8* legUpper = r.getString(itemDisplayDB::TexLegUpper);
		const c8* legLower = r.getString(itemDisplayDB::TexLegLower);

		tex.addItemLayer(legUpper, CR_LEG_UPPER, Gender, layer);

		if (HasRobe)			//法袍类型的裤子要优先
		{	
			layer = getClothesSlotLayer(CS_BOOTS) + 1;
		}
		tex.addItemLayer(legLower, CR_LEG_LOWER, Gender, layer);
	}
	else if (slot == CS_GLOVES)
	{
		Geosets[CG_GLOVES] = 1 + r.getUInt(itemDisplayDB::GloveGeosetFlags);

		const c8* hand = r.getString(itemDisplayDB::TexHands);
		tex.addItemLayer(hand, CR_HAND, Gender, layer);
		if (Geosets[CG_GLOVES] > 1)				//有模型，这时覆盖法袍,衬衣纹理
		{
			layer = getClothesSlotLayer(CS_CHEST) + 1;
		}
		
		const c8* armLower = r.getString(itemDisplayDB::TexArmLower);	
		tex.addItemLayer(armLower, CR_ARM_LOWER, Gender, layer);
	}
	else if (slot == CS_BOOTS)
	{
 		Geosets[CG_BOOTS] = 1 + r.getUInt(itemDisplayDB::GloveGeosetFlags);
 
 		const c8* legLower = r.getString(itemDisplayDB::TexLegLower);
		const c8* feet = r.getString(itemDisplayDB::TexFeet);

		tex.addItemLayer(legLower, CR_LEG_LOWER, Gender, layer);
		tex.addItemLayer(feet, CR_FOOT, Gender, layer);
	}
	else if (slot == CS_TABARD)
	{
		if (!HasRobe)
		{
			Geosets[CG_TARBARD] = 2;
		}

		const c8* chestUpper = r.getString(itemDisplayDB::TexChestUpper);
		const c8* chestLower = r.getString(itemDisplayDB::TexChestLower);

		tex.addItemLayer(chestUpper, CR_TORSO_UPPER, Gender, layer);
		tex.addItemLayer(chestLower, CR_TORSO_LOWER, Gender, layer);
	}
	else if (slot == CS_CAPE)
	{	
		const c8* tex = r.getString(itemDisplayDB::Skin);
		if (tex)
		{
			c8 path[MAX_PATH];
			strcpy_s(path, MAX_PATH, regionCapePath);
			strcat_s(path, MAX_PATH, tex);
			strcat_s(path, MAX_PATH, ".blp");
			ITexture* cape = g_Engine->getResourceLoader()->loadTexture(path);
			if (cape)
			{
				setReplaceTexture(TEXTURE_CAPE, cape);
				Geosets[CG_CAPE] = 1 + r.getUInt(itemDisplayDB::GloveGeosetFlags);
				CapeID = Geosets[CG_CAPE];
			}		
		}	
	}
	else
	{
		return;
	}

	if (Geosets[CG_TROUSERS] == 1)
		Geosets[CG_TROUSERS] = 1 + r.getUInt(itemDisplayDB::RobeGeosetFlags);
	if (Geosets[CG_TROUSERS] == 2)
	{
		Geosets[CG_BOOTS] = 0;
		Geosets[CG_TARBARD] = 0;
	}

	if (Geosets[CG_GLOVES] > 1)
		Geosets[CG_WRISTBANDS] = 0;
}

void wow_character::buildVisibleGeosets()
{
	if (!CurrentSkin)
		return;

	InitializeListHead(&VisibleGeosetList);

	if (Mesh->getType() != MT_CHARACTER || Race == RACE_NAGA)
	{
		for (u32 i=0; i<CurrentSkin->NumGeosets; ++i)
		{
			CGeoset* submesh = &CurrentSkin->Geosets[i];
			s16 texID = submesh->getTexUnit(0)->TexID;

			if (texID != -1)
			{
				ETextureTypes texType = Mesh->TextureTypes[texID];
				if (texType == TEXTURE_FILENAME)
					DynGeosets[i].Textures[0] = Mesh->Textures[texID];
				else
					DynGeosets[i].Textures[0] = ReplaceTextures[(u32)texType];
			}
			else
			{
				DynGeosets[i].Textures[0] = NULL;
			}

			InsertTailList(&VisibleGeosetList, &DynGeosets[i].Link);
		}

		return;
	}

	bool isBald = true;

	//eye glow
	if (DeathKnight)
		Geosets[CG_EYEGLOW] = 3;

	//ears
	Geosets[CG_EARS]= 2;

	if (!ShowCape)
		Geosets[CG_CAPE] = 1;
	else
		Geosets[CG_CAPE] = CapeID;

	//hair facial visbility
	dbc::record r = g_Engine->getWowDatabase()->getCharFacialHairDB()->getByParams( Race, Gender, FacialHair );
	if (!r.isValid())
		r = g_Engine->getWowDatabase()->getCharFacialHairDB()->getByParams( Race, Gender, 0 );
	if (r.isValid())
	{
		Geosets[CG_GEOSET100] = r.getUInt(charFacialHairDB::Geoset100V400);		
		Geosets[CG_GEOSET200] = r.getUInt(charFacialHairDB::Geoset200V400);	
		Geosets[CG_GEOSET300] = r.getUInt(charFacialHairDB::Geoset300V400);
	}

	//visibility
	bool* GeoShow = (bool*)Hunk_AllocateTempMemory(sizeof(bool) * CurrentSkin->NumGeosets);
	memset(GeoShow, 0, sizeof(bool) * CurrentSkin->NumGeosets);

	charHairGeosetsDB* hairGeosetDb = g_Engine->getWowDatabase()->getCharHairGeosetDB();
	for (u32 i=0; i<hairGeosetDb->getNumRecords(); ++i)
	{
		dbc::record r = hairGeosetDb->getRecord(i);
		if (r.getUInt(charHairGeosetsDB::Race) == Race &&
			r.getUInt(charHairGeosetsDB::Gender) == Gender)
		{
			u32 id = r.getUInt(charHairGeosetsDB::Geoset);
			u32 style = r.getUInt(charHairGeosetsDB::Section);

			if(id != 0)
			{
				for (u32 i=0; i<CurrentSkin->NumGeosets; ++i)
				{
					CGeoset* set = &CurrentSkin->Geosets[i];
					if (set->GeoID == id)
					{
						GeoShow[i] = (HairStyle == style && !HelmHideHair);
						if (GeoShow[i])
							isBald = false;
					}
				}
			}
		}
	}

	int gloves = 0;
	int wrist = 0;

	//show
	for (u32 i=0; i<CurrentSkin->NumGeosets; ++i)
	{
		CGeoset* submesh = &CurrentSkin->Geosets[i];
		u32 id  = submesh->GeoID;
		s16 texID = submesh->getTexUnit(0)->TexID;

		if (texID != -1)
		{
			ETextureTypes texType = Mesh->TextureTypes[texID];
			if (texType == TEXTURE_FILENAME)
			{
				DynGeosets[i].Textures[0] = Mesh->Textures[texID];
			}
			else
			{
				DynGeosets[i].Textures[0] = ReplaceTextures[(u32)texType];
			}
		}
		else
		{
			DynGeosets[i].Textures[0] = NULL;
		}

		if (id == 0)		
		{
			if (Race == RACE_WORGEN && i >= 3)		//i>=3狼人问题
				GeoShow[i] = false;
			else
				GeoShow[i] = true;
			continue;
		}
	
		if (id == 1)
		{
			GeoShow[i] |= isBald;				//秃头的头顶部
			continue;
		}

		int geotype = (int)(id / 100);
		if (isBald && geotype == CG_HAIRSTYLE)							//隐藏头发
		{
			GeoShow[i] = false;
			continue;
		}

		if (geotype != 0)				//except hair
		{
			GeoShow[i] = (id == geotype * 100 + Geosets[geotype]);
		}

		if (GeoShow[i] && Race == RACE_WORGEN)				//狼人多手问题fix
		{
			if (geotype == CG_GLOVES)
			{
				if (gloves >= 1)
					GeoShow[i] = false;
				else
					++gloves; 
			}
			else
			if (geotype == CG_WRISTBANDS)
			{
				if (wrist >= 1)
					GeoShow[i] = false;
				else
					++wrist; 
			}
		}

		/*	
		ECharGeosets t = (ECharGeosets)geotype;
		if (GeoShow[i].Show)
		{
			int m = 0;	
		}	
	*/	
	}

	for (u32 i=0; i<CurrentSkin->NumGeosets; ++i)
	{
		if (GeoShow[i])
			InsertTailList(&VisibleGeosetList, &DynGeosets[i].Link);
	}

	Hunk_FreeTempMemory(GeoShow);
}

bool wow_character::setGeosetMaterial(u32 subset, SMaterial& material)
{
	CGeoset* set = &CurrentSkin->Geosets[subset];
	s16 rfIndex = set->getTexUnit(0)->rfIndex;
	if (rfIndex == -1)
		return false;

	const IFileM2::SRenderFlag& renderflag = Mesh->RenderFlags[rfIndex];

	if (renderflag.invisible)
		return false;

	material.ZWriteEnable = renderflag.zwrite;
	material.Lighting = renderflag.lighting;
	material.FrontfaceCulling = renderflag.frontCulling;

	material.EmissiveColor = DynGeosets[subset].emissive;
// 	material.setMaterialColor(SColor(0,0,255));
//		material.setMaterialAlpha(0.5f);

	switch(renderflag.blend)
	{
	case M2::BM_OPAQUE:
		material.MaterialType = EMT_SOLID;
		break;
	case M2::BM_ALPHA_TEST:
		material.MaterialType =  EMT_ALPHA_TEST;
		break;
	case M2::BM_ALPHA_BLEND:
		material.MaterialType= EMT_TRANSPARENT_ALPHA_BLEND;
		break;
	case M2::BM_ADDITIVE_ALPHA:
		material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;
		break;
	case M2::BM_ADDITIVE_COLOR:
		material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;
		break;
	case M2::BM_MODULATE:
	case M2::BM_MODULATEX2:
		material.MaterialType = EMT_TRANSPARENT_ADD_ALPHA;
		break;
	default:
		_ASSERT(false);
	}

	if(EnableModelColor)
		material.setMaterialColor(ModelColor);

	if (EnableModelAlpha)
	{
		material.setMaterialAlpha(ModelAlpha);
		material.MaterialType = EMT_TRANSPARENT_ALPHA_BLEND;
	}

	material.VertexShader = g_Engine->getDriver()->getShaderServices()->getVertexShader(EVST_HWSKINNING);
	material.TextureLayer[0].TextureWrapU = set->getTexUnit(0)->WrapX ? ETC_CLAMP : ETC_REPEAT;
	material.TextureLayer[0].TextureWrapV = set->getTexUnit(0)->WrapY ? ETC_CLAMP : ETC_REPEAT;

	return true;
}

void wow_character::setM2Equipment( s32 slot, s32 itemid, SAttachmentEntry* entry1, SAttachmentEntry* entry2 )
{
	entry1->attachIndex = -1;
	entry2->attachIndex = -1;

	if (itemid == 0 || Mesh->getType() != MT_CHARACTER)
		return;

	s32 id = -1;
	s32 itemType = 0;
	s32 sheath = 0;

	f32 equipScale = 1.0f;

	if (!IsNpc)
	{
		const SItemRecord* item = g_Engine->getWowDatabase()->getItemById(itemid);
		if (!item)
			return;

		id = item->model;
		itemType = item->type;
		sheath = item->sheath;
	}
	else
	{
		dbc::record r = g_Engine->getWowDatabase()->getItemDB()->getByID(itemid);
		
		id = itemid;
		if (r.isValid())
		{
			itemType = r.getInt(itemDB::InventorySlot);
			sheath = r.getInt(itemDB::Sheath);
		}	
	}

	if (Mesh->getType() == MT_CHARACTER)
	{
		//
		f32 shoulder, weapon;
		getEquipScale(shoulder, weapon);
		switch(slot)
		{
		case CS_SHOULDER:
			equipScale = shoulder;
			break;
		case CS_HAND_RIGHT:
		case CS_HAND_LEFT:
			equipScale = weapon;
			break;
		default:
			break;
		}
	}

	dbc::record display = g_Engine->getWowDatabase()->getItemDisplayDB()->getByID(id);
	if (!display.isValid())
		return;

	s32 id1 = -1;
	s32 id2 = -1;

	string256 path;
	if (slot == CS_HEAD)
	{
		id1 = ATT_HELMET;
		path = regionHeadPath;

		u32 hair = 0;

		if (Gender == 0)
		{
			u32 vis1 = display.getUInt(itemDisplayDB::GeosetVisID1);
			dbc::record geo1 = g_Engine->getWowDatabase()->getHelmGeosetDB()->getByID(vis1);
			if (geo1.isValid())
				hair = geo1.getUInt(helmGeosetDB::Hair);

			HelmHideHair = hair != 0;
		}
		else
		{
			u32 vis2 = display.getUInt(itemDisplayDB::GeosetVisID2);
			dbc::record geo2 = g_Engine->getWowDatabase()->getHelmGeosetDB()->getByID(vis2);
			if (geo2.isValid())
				hair = geo2.getUInt(helmGeosetDB::Hair);

			HelmHideHair = hair != 0;
		}
	}
	else if(slot == CS_SHOULDER)
	{
		id1 = ATT_LEFT_SHOULDER;
		id2 = ATT_RIGHT_SHOULDER;
		path = regionShoulderPath;	
	}
	else if (slot == CS_HAND_LEFT)
	{
		id1 = ATT_LEFT_PALM;	
	}
	else if (slot == CS_HAND_RIGHT)
	{
		id1 = ATT_RIGHT_PALM;
	}
	else if (slot == CS_QUIVER)
	{
		id1 = ATT_RIGHT_BACK_SHEATH;
		path = regionQuiverPath;
	}
	else
	{
		return;
	}

	if (slot == CS_HAND_LEFT || slot == CS_HAND_RIGHT)
	{
		if (itemType == IT_SHIELD)
		{
			path = regionShieldPath;
			id1 = ATT_LEFT_WRIST;
		}
		else
		{
			path = regionWeaponPath;
		}
	}

	if (false && sheath > SHEATHETYPE_NONE)
	{
		id1 = sheath;
		if (slot == CS_HAND_LEFT)
		{
			// make the weapon cross
			if (id1 == ATT_LEFT_BACK_SHEATH)
				id1 = ATT_RIGHT_BACK_SHEATH;
			if (id1 == ATT_LEFT_BACK)
				id1 = ATT_RIGHT_BACK;
			if (id1 == ATT_LEFT_HIP_SHEATH)
				id1 = ATT_RIGHT_HIP_SHEATH;
		}
	}

	string256 modelPath;
	string256 texPath;
	if (id1 >= 0)				//
	{
		modelPath = path;
		modelPath.append(display.getString(itemDisplayDB::Model));

		if (slot == CS_HEAD)
		{	
			string256 headPath;
			if (modelPath.endWith(".mdx"))
			{

				modelPath.subString(0, modelPath.length()-4, headPath);
				headPath.append("_");
				dbc::record race = g_Engine->getWowDatabase()->getCharRacesDB()->getByID(Race);
				_ASSERT(race.isValid());

				headPath.append(race.getString(charRacesDB::ShortName));
				headPath.append(Gender ? "F" : "M");
				headPath.append(".M2");

				modelPath = headPath;
			}
			else
			{
				HelmHideHair = false;
				id1 = -1;
			}
		}
		else
		{
			string256 headPath;
			if (modelPath.endWith(".mdx"))
			{
				modelPath.subString(0, modelPath.length()-4, headPath);
				headPath.append(".M2");
			}		
			modelPath = headPath;
		}

		if (id1 != -1)
		{
			texPath = path;
			texPath.append(display.getString(itemDisplayDB::Skin));
			texPath.append(".blp");

			entry1->slot = slot;
			entry1->scale = equipScale;
			entry1->attachIndex = (id1 >= 0 && id1 < (s32)Mesh->NumAttachLookup) ? Mesh->AttachLookup[id1] : -1;
			strcpy_s(entry1->modelpath, MAX_PATH, modelPath.c_str());
			strcpy_s(entry1->texpath, MAX_PATH, texPath.c_str());
		}
	}

	if (id2 >= 0)
	{
		modelPath = path;
		modelPath.append(display.getString(itemDisplayDB::Model2));

		string256 headPath;
		if (modelPath.endWith(".mdx"))
		{
			modelPath.subString(0, modelPath.length()-4, headPath);
			headPath.append(".M2");

			modelPath = headPath;

			texPath = path;
			texPath.append(display.getString(itemDisplayDB::Skin2));
			texPath.append(".blp");

			entry2->slot = slot;
			entry2->scale = equipScale;
			entry2->attachIndex = (id2 >= 0 && id2 < (s32)Mesh->NumAttachLookup) ? Mesh->AttachLookup[id2] : -1;
			strcpy_s(entry2->modelpath, MAX_PATH, modelPath.c_str());
			strcpy_s(entry2->texpath, MAX_PATH, texPath.c_str());
		}	
	}
}

void wow_character::dressStartOutfit( s32 startid )
{
	dbc::record r = g_Engine->getWowDatabase()->getStartOutfitDB()->getByID(startid);
	if (!r.isValid())
		return;

	for (s32 i=0; i<NUM_CHAR_SLOTS; ++i)
		Equipments[i] = 0;
	HelmHideHair = false;
	DeathKnight = false;

	for (u32 i=0; i<startOutfitDB::NumItems; ++i)
	{
		int id = r.getInt(startOutfitDB::ItemIDBase + i);
		if (id == 0)
			continue;

		const SItemRecord* item = g_Engine->getWowDatabase()->getItemById(id);
		if (item && item->type > 0)
		{
			int slot = getItemSlot(id);

			if (slot != -1)
				updateEquipments(slot, id);
		}
	}
}

void wow_character::dressSet( s32 setid )
{
	dbc::record r = g_Engine->getWowDatabase()->getItemSetDB()->getByID(setid);
	if (!r.isValid())
		return;

	for (u32 i=0; i<itemSetDB::NumItems; ++i)
	{
		s32 id = r.getInt(itemSetDB::ItemIDBaseV400 + i);
		if (id == 0)
			continue;

		const SItemRecord* item = g_Engine->getWowDatabase()->getItemById(id);
		if (item && item->type > 0)
		{
			int slot = getItemSlot(id);
			if (slot != -1)
				updateEquipments(slot, id);
		}
	}
}

