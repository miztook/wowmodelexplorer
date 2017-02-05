#pragma once

#include "core.h"
#include "wow_enums.h"
#include "wow_def.h"
#include "IFileM2.h"
#include "SMaterial.h"

class ITexture;
class IVertexBuffer;
class CFileSkin;

struct CharTexturePart				//纹理组合
{
	c8  Name[QMAX_PATH];
	s32 Region;
	s32 Layer;

	bool operator<(const CharTexturePart& c) const
	{
		return Layer < c.Layer;
	}
};

struct CharTexture				//使用临时内存
{
	explicit CharTexture(bool isHD);
	~CharTexture();

	void addLayer(const c8* name, s32 region, s32 layer);
	bool addItemLayer(const c8* name, s32 region,  u32 gender, s32 layer);
	bool makeItemTexture(s32 region,  u32 gender, const c8* name, c8* outname);

	ITexture* compose(bool pandarenOrHD);

	static const int MAX_TEX_PART_SIZE = 40;

	CharTexturePart*		TextureParts;
	u32		TexPartCount;	
	bool		IsHD;
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

	struct SUnit
	{
		bool		Enable;
		matrix4*		BoneMats;
		SBoneMatrixArray*		BoneMatrixArray;
	};

	LENTRY		Link;
	ITexture*		Texture0;
	ITexture*		Texture1;
	ITexture*		Texture2;
	SUnit*		Units;
	matrix4			TextureMatrix;
	SColor		emissive;
	SColor		finalColor;
	u32			NumUnits;

	bool				NoAlpha;
	bool				UseTextureAnim;
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


//只有Character类型才有的信息
struct SCharacterInfo		 
{
	explicit SCharacterInfo(bool npc) : IsNpc(npc)
	{
		IsHD = false;
		HasRobe = false;
		DeathKnight = false;
		ShowCape = true;
		CapeID = 1;
		Race = 0;
		Gender = 0;
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

		::memset(Equipments, 0, sizeof(Equipments));
		::memset(Geosets, 0, sizeof(Geosets));
#ifdef MW_EDITOR
		::memset(BodyTextureFileNames, 0, sizeof(BodyTextureFileNames));
#endif
	}

	u32		SkinColor;
	u32		FaceType;
	u32		HairColor;
	u32		HairStyle;
	u32		FacialHair;

	u32		CapeID;
	u32		Race;
	u32		Gender;

	s32		Equipments[NUM_CHAR_SLOTS];
	s32		Geosets[NUM_GEOSETS];

	bool		DeathKnight;
	bool		HelmHideFacial1;
	bool		HelmHideFacial2;
	bool		HelmHideFacial3;
	bool		HelmHideHair;			//头盔遮挡头发
	bool		ShowCape;
	bool		IsNpc;
	bool		HasRobe;
	bool		Blink;			//眨眼
	bool		CloseRHand;		//握右拳
	bool		CloseLHand;		//握左拳
	bool		IsHD;

#ifdef MW_EDITOR
	c8			BodyTextureFileNames[NUM_REGIONS][QMAX_PATH];
#endif
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
	void animateBones(u32 anim, u32 time, u32 lastingtime, f32 blend);
	void disableBones();
	void animateColors(u32 anim, u32 time);
	void animateTextures(u32 anim, u32 time);
	void solidColors();

	//clothes
	void updateEquipments(s32 slot, s32 itemid);
	s32 getItemSlot(s32 itemid) const;
	s32 getSlotItemId(s32 slot) const;

	//weapon, attachment
	bool slotHasModel(s32 slot) const;
	void setM2Equipment(s32 slot, s32 itemid, bool sheath, SAttachmentEntry* entry1, SAttachmentInfo* info1, SAttachmentEntry* entry2, SAttachmentInfo* info2);

	//mount
	void setM2Mount(SAttachmentEntry* entry);

	//head decal
	void setHeadDecal(SAttachmentEntry* entry);

	//spellvisualkit
	void setSpellVisualKit(u32 spellvkId, SSpellVKInfo* svkInfo);

	//startoutfit
	void dressStartOutfit(s32 startid);

	//set
	void dressSet(s32 setid);

	//npc
	bool updateNpc(s32 npcid);

	//item
	void setItemTexture(ITexture* tex) { setReplaceTexture(TEXTURE_ITEM, tex); }
	bool getItemInfo(s32 itemid, bool isNpc, SItemInfo& itemInfo) const;
	bool isBothHandsDagger(s32 itemid) const;
	bool isOrdinaryDagger(s32 itemid) const;

	bool setGeosetMaterial(u32 subset, SMaterial& material);

	void buildVisibleGeosets();

public:
	SCharacterInfo*		CharacterInfo;

	SDynGeoset*	DynGeosets;			
	CFileSkin*		CurrentSkin;
	SDynBone*		DynBones;

	s8*			UseAttachments;
	ITexture*		ReplaceTextures[NUM_TEXTURETYPE];				//可替换的纹理

	LENTRY		VisibleGeosetList;
	aabbox3df		AnimatedBox;

	//
	f32			ModelAlpha;
	SColor	ModelColor;

	bool		EnableModelAlpha;
	bool		EnableModelColor;
	bool			ShowParticles;
	bool			ShowRibbons;

private:
	void setReplaceTexture(ETextureTypes type, ITexture* texture);

	//character
	//设置服装类型的装备(不需要另外的模型)
	void addClothesEquipment(s32 slot, s32 itemnum, s32 layer, CharTexture& tex, bool npc);
	void dressupCharacter(CharTexture& charTex);
	s32 getSlot(s32 type) const;
	static int getClothesSlotLayer( int slot )	;

	void calcAttachmentBone(u8 i, u32 anim, u32 time, f32 blend);
	void calcBone(u8 i, u32 anim, u32 time, f32 blend, bool enableScale, aabbox3df* animatedBox=NULL_PTR);				//i: indexinGlobal

	bool isBlinkGeoset(u32 index) const;
	void getEquipScale(f32& head, f32& shoulder, f32& weapon, f32& waist) const;

	void calcTextureAnim(u32 c, u32 anim, u32 time);

	bool setMaterialShaders(SMaterial& material, const STexUnit* texUnit, bool billboard);

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
	u32		LastBoneAnim, LastColorAnim, LastTextureAnim;
	u32		LastBoneTime, LastColorTime, LastTextureTime;

	IFileM2*		Mesh;
	SHint*		BoneHints;
	SHint*		TextureAnimHints;
	SColorHint*		ColorHints;
	bool*		Calcs;
};
