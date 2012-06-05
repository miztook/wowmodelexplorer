#pragma once

#include "base.h"

//texture
//人物的特殊贴图指可以变化的贴图，如头发，肤色等
enum ETextureTypes
{
	TEXTURE_FILENAME=0,			// Texture given in filename
	TEXTURE_BODY,				// Body + clothes
	TEXTURE_CAPE,				// Item, Capes ("Item\ObjectComponents\Cape\*.blp")
	TEXTURE_ITEM=TEXTURE_CAPE,
	TEXTURE_ARMORREFLECT,		// 
	TEXTURE_HAIR=6,				// Hair, bear
	TEXTURE_FUR=8,				// Tauren fur
	TEXTURE_INVENTORY_ART1,		// Used on inventory art M2s (1): inventoryartgeometry.m2 and inventoryartgeometryold.m2
	TEXTURE_QUILL,				// Only used in quillboarpinata.m2. I can't even find something referencing that file. Oo Is it used?
	TEXTURE_GAMEOBJECT1,		// Skin for creatures or gameobjects #1
	TEXTURE_GAMEOBJECT2,		// Skin for creatures or gameobjects #2
	TEXTURE_GAMEOBJECT3,		// Skin for creatures or gameobjects #3
	TEXTURE_INVENTORY_ART2,		// Used on inventory art M2s (2): ui-buffon.m2 and forcedbackpackitem.m2 (LUA::Model:ReplaceIconTexture("texture"))
	TEXTURE_15,					// Patch 12857, Unknown
	TEXTURE_16,					//
	TEXTURE_17,					//
	NUM_TEXTURETYPE,

	TEXTURE_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum POSITION_SLOTS { // wxString Attach_Names[]
	ATT_LEFT_WRIST = 0, // Mountpoint
	ATT_RIGHT_PALM,
	ATT_LEFT_PALM,
	ATT_RIGHT_ELBOW,
	ATT_LEFT_ELBOW,
	ATT_RIGHT_SHOULDER, // 5
	ATT_LEFT_SHOULDER,
	ATT_RIGHT_KNEE,
	ATT_LEFT_KNEE,
	ATT_RIGHT_HIP,
	ATT_LEFT_HIP, // 10
	ATT_HELMET,
	ATT_BACK,
	ATT_RIGHT_SHOULDER_HORIZONTAL,
	ATT_LEFT_SHOULDER_HORIZONTAL,
	ATT_BUST, // 15
	ATT_BUST2,
	ATT_FACE,
	ATT_ABOVE_CHARACTER,
	ATT_GROUND,
	ATT_TOP_OF_HEAD, // 20
	ATT_LEFT_PALM2,
	ATT_RIGHT_PALM2,
	ATT_PRE_CAST_2L,
	ATT_PRE_CAST_2R,
	ATT_PRE_CAST_3, // 25
	ATT_RIGHT_BACK_SHEATH,
	ATT_LEFT_BACK_SHEATH,
	ATT_MIDDLE_BACK_SHEATH,
	ATT_BELLY,
	ATT_LEFT_BACK, // 30
	ATT_RIGHT_BACK,
	ATT_LEFT_HIP_SHEATH,
	ATT_RIGHT_HIP_SHEATH,
	ATT_BUST3, // Spell Impact
	ATT_PALM3, // 35
	ATT_RIGHT_PALM_UNK2,
	ATT_DEMOLISHERVEHICLE,
	ATT_DEMOLISHERVEHICLE2,
	ATT_VEHICLE_SEAT1,
	ATT_VEHICLE_SEAT2, // 40
	ATT_VEHICLE_SEAT3,
	ATT_VEHICLE_SEAT4,

	ATT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum SheathTypes
{
	SHEATHETYPE_NONE                   = 0,
	SHEATHETYPE_MAINHAND               = 1,
	SHEATHETYPE_LARGEWEAPON            = 2,
	SHEATHETYPE_HIPWEAPON              = 3,
	SHEATHETYPE_SHIELD                 = 4,

	SHEATHETYPE_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum KeyBoneTable { // wxString Bone_Names[]
	//Block F - Key Bone lookup table.
	//---------------------------------
	BONE_LARM = 0,		// 0, ArmL: Left upper arm
	BONE_RARM,			// 1, ArmR: Right upper arm
	BONE_LSHOULDER,		// 2, ShoulderL: Left Shoulder / deltoid area
	BONE_RSHOULDER,		// 3, ShoulderR: Right Shoulder / deltoid area
	BONE_STOMACH,		// 4, SpineLow: (upper?) abdomen
	BONE_WAIST,			// 5, Waist: (lower abdomen?) waist
	BONE_HEAD,			// 6, Head
	BONE_JAW,			// 7, Jaw: jaw/mouth
	BONE_RFINGER1,		// 8, IndexFingerR: (Trolls have 3 "fingers", this points to the 2nd one.
	BONE_RFINGER2,		// 9, MiddleFingerR: center finger - only used by dwarfs.. don't know why
	BONE_RFINGER3,		// 10, PinkyFingerR: (Trolls have 3 "fingers", this points to the 3rd one.
	BONE_RFINGERS,		// 11, RingFingerR: Right fingers -- this is -1 for trolls, they have no fingers, only the 3 thumb like thingys
	BONE_RTHUMB,		// 12, ThumbR: Right Thumb
	BONE_LFINGER1,		// 13, IndexFingerL: (Trolls have 3 "fingers", this points to the 2nd one.
	BONE_LFINGER2,		// 14, MiddleFingerL: Center finger - only used by dwarfs.
	BONE_LFINGER3,		// 15, PinkyFingerL: (Trolls have 3 "fingers", this points to the 3rd one.
	BONE_LFINGERS,		// 16, RingFingerL: Left fingers
	BONE_LTHUMB,		// 17, ThubbL: Left Thumb
	BONE_BTH,			// 18, $BTH: In front of head
	BONE_CSR,			// 19, $CSR: Left hand
	BONE_CSL,			// 20, $CSL: Left hand
	BONE_BREATH,		// 21, _Breath
	BONE_NAME,			// 22, _Name
	BONE_NAMEMOUNT,		// 23, _NameMount
	BONE_CHD,			// 24, $CHD: Head
	BONE_CCH,			// 25, $CCH: Bust
	BONE_ROOT,			// 26, Root: The "Root" bone,  this controls rotations, transformations, etc of the whole model and all subsequent bones.
	BONE_MAX,

	BONE_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};


// Race value is index in ChrRaces.dbc
enum ERaces
{
	RACE_HUMAN              = 1,
	RACE_ORC                = 2,
	RACE_DWARF              = 3,
	RACE_NIGHTELF           = 4,
	RACE_UNDEAD             = 5,
	RACE_TAUREN             = 6,
	RACE_GNOME              = 7,
	RACE_TROLL              = 8,
	RACE_GOBLIN             = 9,
	RACE_BLOODELF           = 10,
	RACE_DRAENEI            = 11,
	RACE_FEL_ORC            = 12,
	RACE_NAGA               = 13,
	RACE_BROKEN             = 14,
	RACE_SKELETON           = 15,
	RACE_VRYKUL             = 16,
	RACE_TUSKARR            = 17,
	RACE_FOREST_TROLL       = 18,
	RACE_TAUNKA             = 19,
	RACE_NORTHREND_SKELETON = 20,
	RACE_ICE_TROLL          = 21,
	RACE_WORGEN             = 22,

	RACE_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum ECharGeosets {
	CG_HAIRSTYLE,
	CG_GEOSET100,
	CG_GEOSET200,
	CG_GEOSET300,
	CG_GLOVES,
	CG_BOOTS, // 5
	CG_EARS = 7,
	CG_WRISTBANDS,
	CG_KNEEPADS,
	CG_PANTS, // 10
	CG_PANTS2,
	CG_TARBARD,
	CG_TROUSERS,
	CG_CAPE = 15, // 15						//后背
	CG_EYEGLOW = 17,
	CG_BELT,

	NUM_GEOSETS,

	CG_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum ECharRegions {
	CR_BASE = 0,
	CR_ARM_UPPER,
	CR_ARM_LOWER,
	CR_HAND,
	CR_FACE_UPPER,
	CR_FACE_LOWER,
	CR_TORSO_UPPER,								//躯干
	CR_TORSO_LOWER,
	CR_PELVIS_UPPER,						//骨盆，其实也是腿的区域
	CR_PELVIS_LOWER,
	CR_FOOT,
	NUM_REGIONS,

	CR_LEG_UPPER = CR_PELVIS_UPPER,			//腿
	CR_LEG_LOWER = CR_PELVIS_LOWER,

	CR_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

struct CharRegionCoords 
{
	u32 xpos;
	u32 ypos;
	u32 xsize;
	u32 ysize;
};

//装备系统的纹理分布
#define	REGION_FAC	2
#define	REGION_PX	(256*REGION_FAC)

const CharRegionCoords regions[NUM_REGIONS] =
{
	{0, 0, 256*REGION_FAC, 256*REGION_FAC},	// base
	{0, 0, 128*REGION_FAC, 64*REGION_FAC},	// arm upper
	{0, 64*REGION_FAC, 128*REGION_FAC, 64*REGION_FAC},	// arm lower
	{0, 128*REGION_FAC, 128*REGION_FAC, 32*REGION_FAC},	// hand
	{0, 160*REGION_FAC, 128*REGION_FAC, 32*REGION_FAC},	// face upper
	{0, 192*REGION_FAC, 128*REGION_FAC, 64*REGION_FAC},	// face lower
	{128*REGION_FAC, 0, 128*REGION_FAC, 64*REGION_FAC},	// torso upper
	{128*REGION_FAC, 64*REGION_FAC, 128*REGION_FAC, 32*REGION_FAC},	// torso lower
	{128*REGION_FAC, 96*REGION_FAC, 128*REGION_FAC, 64*REGION_FAC}, // pelvis upper
	{128*REGION_FAC, 160*REGION_FAC, 128*REGION_FAC, 64*REGION_FAC},// pelvis lower
	{128*REGION_FAC, 224*REGION_FAC, 128*REGION_FAC, 32*REGION_FAC}	// foot
};

enum ECharSlots {
	CS_HEAD = 0,
	CS_NECK,
	CS_SHOULDER,
	CS_BOOTS,
	CS_BELT,
	CS_SHIRT,
	CS_PANTS,
	CS_CHEST,
	CS_BRACERS,
	CS_GLOVES,
	CS_HAND_RIGHT,
	CS_HAND_LEFT,
	CS_CAPE,
	CS_TABARD,
	CS_QUIVER,

	NUM_CHAR_SLOTS,

	CS_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

// 装备类型
enum EItemTypes {
	IT_ALL = 0,
	IT_HEAD,
	IT_NECK,
	IT_SHOULDER,
	IT_SHIRT,
	IT_CHEST,
	IT_BELT,
	IT_PANTS,
	IT_BOOTS,
	IT_BRACERS,
	IT_GLOVES,
	IT_RINGS,
	IT_ACCESSORY,
	IT_DAGGER,
	IT_SHIELD,
	IT_BOW,
	IT_CAPE,
	IT_2HANDED,
	IT_QUIVER,
	IT_TABARD,
	IT_ROBE,
	IT_RIGHTHANDED, // IT_1HANDED
	IT_LEFTHANDED, // IT_CLAW
	IT_OFFHAND,
	IT_AMMO, // unused?
	IT_THROWN,
	IT_GUN,
	IT_UNUSED,
	IT_RELIC,

	NUM_ITEM_TYPES,

	IT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum TEXTUREFLAG {
	TEXTURE_WRAPX = 1,
	TEXTURE_WRAPY = 2,

	TEXTURE_WRAP_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

//模型位置对应装备
inline bool isCorrectType(int type, int slot)
{
	if (type == IT_ALL) 
		return true;

	switch (slot) {
	case CS_HEAD:		return (type == IT_HEAD);
	case CS_NECK:		return (type == IT_NECK);
	case CS_SHOULDER:	return (type == IT_SHOULDER);
	case CS_SHIRT:		return (type == IT_SHIRT);
	case CS_CHEST:		return (type == IT_CHEST || type == IT_ROBE);
	case CS_BELT:		return (type == IT_BELT);
	case CS_PANTS:		return (type == IT_PANTS);
	case CS_BOOTS:		return (type == IT_BOOTS);
	case CS_BRACERS:	return (type == IT_BRACERS);
	case CS_GLOVES:		return (type == IT_GLOVES);

		// Slight correction.  Type 21 = Lefthand weapon, Type 22 = Righthand weapon
		//case CS_HAND_RIGHT:	return (type == IT_1HANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER);
		//case CS_HAND_LEFT:	return (type == IT_1HANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_HAND_RIGHT:	return (type == IT_RIGHTHANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_DAGGER);
	case CS_HAND_LEFT:	return (type == IT_LEFTHANDED || type == IT_BOW || type == IT_SHIELD || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_CAPE:		return (type == IT_CAPE);
	case CS_TABARD:		return (type == IT_TABARD);
	case CS_QUIVER:		return (type == IT_QUIVER);
	}
	return false;
}

inline bool isTwoHanded(int type)
{
	return type == IT_DAGGER;
}

enum M2Type
{
	MT_NONE = 0,
	MT_CHARACTER,
	MT_CREATRUE,
	MT_ITEM,
	MT_SPELLS,
	MT_PARTCILES,
	MT_INTERFACE,
	MT_WORLD,
	MT_COUNT,

	MT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

static const char* M2TypeString[] =
{
	"",
	"character",
	"creature",
	"item",
	"spells",
	"particles",
	"interface",
	"world",
};

inline M2Type getM2Type(const char* dir)
{
	for (u32 i=1; i<MT_COUNT; ++i)
	{
		if ( 0 == _strnicmp(M2TypeString[i], dir, strlen(M2TypeString[i])) )
			return (M2Type)i;
	}
	return MT_NONE;
}
