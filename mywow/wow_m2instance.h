#pragma once

#include "core.h"
#include "wow_enums.h"
#include "IFileM2.h"
#include "SMaterial.h"

class ITexture;
class IVertexBuffer;
class CFileSkin;

struct CharTexturePart				//纹理组合
{
	c8  Name[MAX_PATH];
	s32 Region;
	s32 Layer;

	bool operator<(const CharTexturePart& c) const
	{
		return Layer < c.Layer;
	}
};

struct CharTexture				//使用临时内存
{
	CharTexture();
	~CharTexture();

	void addLayer(const c8* name, s32 region, s32 layer);
	bool addItemLayer(const c8* name, s32 region,  u32 gender, s32 layer);
	bool makeItemTexture(s32 region,  u32 gender, const c8* name, c8* outname);

	ITexture* compose(bool pandaren);

private:
	static const int MAX_TEX_PART_SIZE = 40;

	CharTexturePart*		TextureParts;
	u32		TexPartCount;	
};

struct SDynGeoset
{
	~SDynGeoset()
	{
		for (u32 i=0; i<NumUnits; ++i)
		{
			delete Units[i].BoneMats;
			delete Units[i].BoneMatrixArray;
		}
		delete[] Units;
	}

	u32		Index;
	bool		NoAlpha;
	ITexture*		Texture0;
	matrix4			TextureMatrix;
	bool				UseTextureAnim;

	SColor	emissive;

	struct SUnit
	{
		bool		Enable;
		matrix4A16*		BoneMats;
		SBoneMatrixArray*		BoneMatrixArray;
	};

	SUnit*		Units;
	u32		NumUnits;

	LENTRY		Link;
};

struct SDynBone					//骨骼动态信息
{
	SDynBone()
		: transPivot(0,0,0), trans(0,0,0), scale(1,1,1), rot(0,0,0,1) {}
	matrix4 mat;
	vector3df		transPivot;

	//local
	vector3df trans, scale;
	quaternion rot;		
};

struct SAttachmentEntry
{
	SAttachmentEntry() : slot(-1), attachIndex(-1), scale(1.0f), node(NULL)
	{
		::memset(modelpath, 0, MAX_PATH);
		::memset(texpath, 0, MAX_PATH);
	}
	s32		slot;
	s16		attachIndex;
	c8		modelpath[MAX_PATH];
	c8		texpath[MAX_PATH];
	f32		scale;
	void*		node;
};

//只有Character类型才有的信息
struct SCharacterInfo		 
{
	SCharacterInfo(bool npc) : IsNpc(npc)
	{
		HasRobe = false;
		DeathKnight = false;
		ShowCape = true;
		CapeID = 1;
		Race = 0;
		Blink = false;
		CloseLHand = false;
		CloseRHand = false;

		SkinColor = 0;
		FaceType = 0;
		HairColor = 0;
		HairStyle = 1;
		FacialHair = 0;

		HelmHideFacial1 = false;
		HelmHideFacial2 = false;
		HelmHideFacial3 = false;
		HelmHideHair = false;

		for (s32 i=0; i<NUM_CHAR_SLOTS; ++i)
			Equipments[i] = 0;
	}

	u32		SkinColor;
	u32		FaceType;
	u32		HairColor;
	u32		HairStyle;
	u32		FacialHair;

	bool		DeathKnight;
	bool		HelmHideFacial1;
	bool		HelmHideFacial2;
	bool		HelmHideFacial3;
	bool		HelmHideHair;			//头盔遮挡头发
	bool		ShowCape;
	u32			CapeID;

	bool		Blink;			//眨眼
	bool		CloseRHand;		//握右拳
	bool		CloseLHand;		//握左拳

	int		Equipments[NUM_CHAR_SLOTS];

	u32		Race;
	u32		Gender;

	bool		IsNpc;
	bool		HasRobe;
	int		Geosets[NUM_GEOSETS];
};

//在m2静态数据上的动态角色信息
class wow_m2instance
{
public:
	wow_m2instance(IFileM2* m2, bool npc);
	~wow_m2instance();

public:
	IFileM2* getMesh() const { return Mesh; }

	//character
	void updateCharacter();			//更新模型显示信息
	void animateBones(u32 anim, u32 time, u32 lastingtime, f32 blend, const matrix4* billboardRot);
	void disableBones();
	void animateColors(u32 anim, u32 time);
	void animateTextures(u32 anim, u32 time);
	void solidColors();

	//clothes
	void updateEquipments(s32 slot, s32 itemid);
	s32 getItemSlot(s32 itemid);

	//weapon, attachment
	bool slotHasModel(s32 slot);
	void setM2Equipment(s32 slot, s32 itemid, SAttachmentEntry* entry1, SAttachmentEntry* entry2);

	//startoutfit
	void dressStartOutfit(s32 startid);

	//set
	void dressSet(s32 setid);

	//npc
	bool updateNpc(s32 npcid);

	//item
	void setItemTexture(ITexture* tex) { setReplaceTexture(TEXTURE_ITEM, tex); }

	bool setGeosetMaterial(u32 subset, SMaterial& material);

	void buildVisibleGeosets();

public:
	SCharacterInfo*		CharacterInfo;

	SDynGeoset*	DynGeosets;			
	CFileSkin*		CurrentSkin;
	SDynBone*		DynBones;

	bool*			ShowAttachments;
	bool			ShowParticles;
	bool			ShowRibbons;

	ITexture*		ReplaceTextures[NUM_TEXTURETYPE];				//可替换的纹理

	LENTRY		VisibleGeosetList;

	aabbox3df		AnimatedBox;

	//
	f32		ModelAlpha;
	bool		EnableModelAlpha;
	SColor	ModelColor;
	bool		EnableModelColor;

private:
	void setReplaceTexture(ETextureTypes type, ITexture* texture);

	//character
	//设置服装类型的装备(不需要另外的模型)
	void addClothesEquipment(s32 slot, s32 itemnum, s32 layer, CharTexture& tex, bool lookup=true);
	void dressupCharacter(CharTexture& charTex);
	int getSlot(int type);
	int getClothesSlotLayer( int slot )	;

	void calcBone(u8 i, u32 anim, u32 time, f32 blend, bool enableScale, const matrix4* billboardRot, aabbox3df* animatedBox=NULL);				//i: indexinGlobal

	bool isBlinkGeoset(u32 index);
	bool isDeathKnightBlinkGeoset(u32 index);
	void getEquipScale(f32& shoulder, f32& weapon);

	void calcTextureAnim(u32 c, u32 anim, u32 time);

private:
	struct SHint
	{
		s32		transHint;
		s32		scaleHint;
		s32		rotHint;
	};

	struct SColorHint
	{
		s32		colorHint;
		s32		opacityHint;
		s32		transparencyHint;
	};

private:
	IFileM2*		Mesh;

	u32		LastBoneAnim, LastColorAnim, LastTextureAnim;
	u32		LastBoneTime, LastColorTime, LastTextureTime;

	SHint*		BoneHints;
	SHint*		TextureAnimHints;
	SColorHint*		ColorHints;
	bool*		Calcs;
};

inline int wow_m2instance::getSlot(int type)
{
	int* Equipments = CharacterInfo->Equipments;
	switch(type)
	{
	case IT_HEAD:
		return CS_HEAD;
	case IT_NECK:
		return CS_NECK;
	case IT_SHIRT:
		return CS_SHIRT;
	case IT_SHOULDER:
		return CS_SHOULDER;
	case IT_CHEST:
	case IT_ROBE:
		return CS_CHEST;
	case IT_BELT:
		return CS_BELT;
	case IT_PANTS:
		return CS_PANTS;
	case IT_BOOTS:
		return CS_BOOTS;
	case IT_BRACERS:
		return CS_BRACERS;
	case IT_GLOVES:
		return CS_GLOVES;
	case IT_RIGHTHANDED:
	case IT_GUN:
		return CS_HAND_RIGHT;
	case IT_LEFTHANDED:
	case IT_BOW:
	case IT_SHIELD:
	case IT_OFFHAND:
		return CS_HAND_LEFT;
	case IT_CAPE:
		return CS_CAPE;
	case IT_TABARD:
		return CS_TABARD;
	case IT_QUIVER:
		return CS_QUIVER;
	case IT_2HANDED:
		return CS_HAND_RIGHT;

	case IT_DAGGER:
		if (Equipments[CS_HAND_RIGHT] != 0 && Equipments[CS_HAND_LEFT] == 0)
			return CS_HAND_LEFT;
		else
			return CS_HAND_RIGHT;

	case IT_THROWN:
			return -1;			//投掷武器不挂在挂点上
	default:
		return -1;
	}
}

inline int wow_m2instance::getClothesSlotLayer( int slot )			//装备的叠加顺序
{
	const int base = 5;
	switch(slot)
	{
	case CS_BELT:
		return base + 8;
	case CS_TABARD:
		return base + 7;
	case CS_CHEST:
		return base + 6;
	case CS_BRACERS:
		return base + 5;
	case CS_GLOVES:
		return base + 4;
	case CS_SHIRT:
		return base + 3;
	case CS_BOOTS:
		return base + 2;
	case CS_PANTS:
		return base + 1;
	default:
		return base;
	}
}

inline void wow_m2instance::getEquipScale(f32& shoulder, f32& weapon)
{
	u32 Race = CharacterInfo->Race;
	u32 Gender = CharacterInfo->Gender;
	shoulder = weapon = 1.0f;

	switch(Race)
	{
	case RACE_HUMAN:
	case RACE_BLOODELF:
	case RACE_UNDEAD:
	case RACE_DWARF:
		{
			if (Gender == 1)
			{
				shoulder = 0.6f;
				weapon = 0.8f;
			}
		}
		break;
	case RACE_NIGHTELF:
	case RACE_TROLL:
		{
			if (Gender == 1)
			{
				shoulder = 0.8f;
			}
		}
		break;
	case RACE_DRAENEI:
		{
			if (Gender == 0)
			{
				shoulder = 1.5f;
			}
			else
			{
				shoulder = 0.8f;
			}
		}
		break;
	case RACE_WORGEN:
	case RACE_PANDAREN:
		{
			if (Gender == 0)
			{
				shoulder = 1.3f;
				weapon = 1.2f;
			}
		}
		break;
	case RACE_TAUREN:
	case RACE_ORC:
		{
			if (Gender == 0)
			{
				shoulder = 1.5f;
				weapon = 1.2f;
			}
		}
		break;
	case RACE_GNOME:
	case RACE_GOBLIN:
		{
			shoulder = 0.6f;
			weapon = 0.6f;
		}
		break;
	}
}