#pragma once

#include "core.h"
#include "wow_enums.h"
#include "wow_def.h"
#include "IFileM2.h"
#include "SMaterial.h"

class ITexture;
class CVertexBuffer;
class CFileSkin;

struct CharTexturePart				//纹理组合
{
	char  Name[QMAX_PATH];
	int32_t Region;
	int32_t Layer;

	bool operator<(const CharTexturePart& c) const
	{
		return Layer < c.Layer;
	}
};

struct CharTexture				//使用临时内存
{
	explicit CharTexture(bool isHD);
	~CharTexture();

	void addLayer(const char* name, int32_t region, int32_t layer);
	bool addItemLayer(const char* name, int32_t region,  uint32_t gender, int32_t layer);
	bool makeItemTexture(int32_t region,  uint32_t gender, const char* name, char* outname);

	ITexture* compose(bool pandarenOrHD);

	static const int MAX_TEX_PART_SIZE = 40;

	CharTexturePart*		TextureParts;
	uint32_t		TexPartCount;	
	bool		IsHD;
};

struct SDynGeoset
{
	~SDynGeoset()
	{
		for (uint32_t i=0; i<NumUnits; ++i)
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
	uint32_t			NumUnits;

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

	uint32_t		SkinColor;
	uint32_t		FaceType;
	uint32_t		HairColor;
	uint32_t		HairStyle;
	uint32_t		FacialHair;

	uint32_t		CapeID;
	uint32_t		Race;
	uint32_t		Gender;

	int32_t		Equipments[NUM_CHAR_SLOTS];
	int32_t		Geosets[NUM_GEOSETS];

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
	char			BodyTextureFileNames[NUM_REGIONS][QMAX_PATH];
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
	void animateBones(uint32_t anim, uint32_t time, uint32_t lastingtime, float blend);
	void disableBones();
	void animateColors(uint32_t anim, uint32_t time);
	void animateTextures(uint32_t anim, uint32_t time);
	void solidColors();

	//clothes
	void updateEquipments(int32_t slot, int32_t itemid);
	int32_t getItemSlot(int32_t itemid) const;
	int32_t getSlotItemId(int32_t slot) const;

	//weapon, attachment
	bool slotHasModel(int32_t slot) const;
	void setM2Equipment(int32_t slot, int32_t itemid, bool sheath, SAttachmentEntry* entry1, SAttachmentInfo* info1, SAttachmentEntry* entry2, SAttachmentInfo* info2);

	//mount
	void setM2Mount(SAttachmentEntry* entry);

	//head decal
	void setHeadDecal(SAttachmentEntry* entry);

	//startoutfit
	void dressStartOutfit(int32_t startid);

	//set
	void dressSet(int32_t setid);

	//npc
	bool updateNpc(int32_t npcid);

	//item
	void setItemTexture(ITexture* tex) { setReplaceTexture(TEXTURE_ITEM, tex); }
	bool getItemInfo(int32_t itemid, bool isNpc, SItemInfo& itemInfo) const;
	bool isBothHandsDagger(int32_t itemid) const;
	bool isOrdinaryDagger(int32_t itemid) const;

	bool setGeosetMaterial(uint32_t subset, SMaterial& material);

	void buildVisibleGeosets();

public:
	SCharacterInfo*		CharacterInfo;

	SDynGeoset*	DynGeosets;			
	CFileSkin*		CurrentSkin;
	SDynBone*		DynBones;

	int8_t*			UseAttachments;
	ITexture*		ReplaceTextures[NUM_TEXTURETYPE];				//可替换的纹理

	LENTRY		VisibleGeosetList;
	aabbox3df		AnimatedBox;

	//
	float			ModelAlpha;
	SColor	ModelColor;

	bool		EnableModelAlpha;
	bool		EnableModelColor;
	bool			ShowParticles;
	bool			ShowRibbons;

private:
	void setReplaceTexture(ETextureTypes type, ITexture* texture);

	//character
	//设置服装类型的装备(不需要另外的模型)
	void addClothesEquipment(int32_t slot, int32_t itemnum, int32_t layer, CharTexture& tex, bool npc);
	void dressupCharacter(CharTexture& charTex);
	int32_t getSlot(int32_t type) const;
	static int getClothesSlotLayer( int slot )	;

	void calcAttachmentBone(uint8_t i, uint32_t anim, uint32_t time, float blend);
	void calcBone(uint8_t i, uint32_t anim, uint32_t time, float blend, bool enableScale, aabbox3df* animatedBox=nullptr);				//i: indexinGlobal

	bool isBlinkGeoset(uint32_t index) const;
	void getEquipScale(float& head, float& shoulder, float& weapon, float& waist) const;

	void calcTextureAnim(uint32_t c, uint32_t anim, uint32_t time);

	bool setMaterialShaders(SMaterial& material, const STexUnit* texUnit, bool billboard);

private:
	struct SHint
	{
		int32_t		transHint;
		int32_t		scaleHint;
		int32_t		rotHint;
	};

	struct SColorHint
	{
		int32_t		colorHint;
		int32_t		opacityHint;
		int32_t		transparencyHint;
	};

private:
	uint32_t		LastBoneAnim, LastColorAnim, LastTextureAnim;
	uint32_t		LastBoneTime, LastColorTime, LastTextureTime;

	IFileM2*		Mesh;
	SHint*		BoneHints;
	SHint*		TextureAnimHints;
	SColorHint*		ColorHints;
	bool*		Calcs;
};
