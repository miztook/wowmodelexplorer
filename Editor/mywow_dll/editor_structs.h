#pragma once

 namespace editor
 {
	struct SItem
	{
		char16_t name[DEFAULT_SIZE];
		int32_t id;
		int32_t type;
		char16_t subclassname[DEFAULT_SIZE];	
	};

	struct SNpc
	{
		char16_t name[DEFAULT_SIZE];
		int32_t modelDisplayId;
		int32_t modelId;
		char16_t type[DEFAULT_SIZE];
	};

	struct SStartOutfit
	{
		char16_t name[DEFAULT_SIZE];
		char16_t shortname[DEFAULT_SIZE];
		int32_t id;
	};

	struct SEntry
	{
		char16_t name[DEFAULT_SIZE];
		int32_t id;
	};

	struct SMap
	{
		int32_t id;
		char16_t name[DEFAULT_SIZE];
		int32_t type;
	};

	struct SRidable
	{
		int32_t npcid;
		uint32_t mountflag;
	};

	struct SAnimation
	{
		char  name[DEFAULT_SIZE];
		uint32_t subIndex;
		uint32_t length;
		uint32_t animIndex;
		int32_t	next;
	};

	struct SCharFeature
	{
		uint32_t		skinColor;
		uint32_t		faceType;
		uint32_t		hairColor;
		uint32_t		hairStyle;
		uint32_t		facialHair;
	};

	struct SCharacterArmoryInfo
	{
		char16_t		Name[DEFAULT_SIZE];
		uint32_t		Race;
		uint32_t		Gender;
		char16_t		ClassShortName[DEFAULT_SIZE];

		uint32_t		SkinColor;
		uint32_t		FaceType;
		uint32_t		HairColor;
		uint32_t		HairStyle;
		uint32_t		FacialHair;

		int32_t		Head;
		int32_t		Shoulder;
		int32_t		Boots;
		int32_t		Belt;
		int32_t		Shirt;
		int32_t		Pants;
		int32_t		Chest;
		int32_t		Bracers;
		int32_t		Gloves;
		int32_t		HandRight;
		int32_t		HandLeft;
		int32_t		Cape;
		int32_t		Tabard;
	};

	enum E_SCENE_DEBUG_PART		//场景的debug信息
	{
		ESDP_BASE = 0,
		ESDP_M2_GEOSETS,
		ESDP_M2_PARTICLES,

		ESDP_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
	};

	enum E_MODEL_PART			//模型的显示/隐藏部分
	{
		EMP_ALL = 0,
		EMP_HEAD,
		EMP_CAPE,
		EMP_SHOULDER,
		EMP_LEFTHAND,
		EMP_RIGHTHAND,

		EMP_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
	};

	enum E_MODEL_EDIT_PART				//模型的编辑显示
	{
		EEP_MODEL = 0,
		EEP_PARTICLES,
		EEP_RIBBONS,
		EEP_BOUNDINGBOX,
		EEP_BONES,
		EEP_BOUNDINGAABOX,
		EEP_COLLISIONAABOX,

		EEP_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
	};

	enum E_WMO_EDIT_PART
	{
		EWP_WMO = 0,
		EWP_DOODADS,

		EWP_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
	};

	enum E_OVERRIDE_WIREFRAME
	{
		EOW_NONE = 0,
		EOW_WIREFRAME,
		EOW_WIREFRAME_SOLID,
		EOW_WIREFRAME_ONECOLOR,

		EOW_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
	};

	enum E_MODEL_REPLACE_TEXTURE				//模型的纹理
	{
		EMRT_BODY = 0,
		EMRT_CAPE,
		EMRT_HAIR,
		EMRT_FUR,
		EMRT_NPC1,
		EMRT_NPC2,
		EMRT_NPC3,
		EMRT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
	};

	struct SFileM2
	{
		char		name[DEFAULT_SIZE];
		char		longname[256];
		uint32_t		numVertices;
		uint32_t		numBoundingVerts;
		uint32_t		numBoundingTriangles;
		uint32_t		numTextures;
		uint32_t		numTexLookup;
		uint32_t		numAttachments;
		uint32_t		numAttachLookup;
		uint32_t		numGlobalSequences;
		uint32_t		numColors;
		uint32_t		numTransparencies;
		uint32_t		numTransparencyLookup;
		uint32_t		numTexAnim;
		uint32_t		numAnimations;
		uint32_t		numBones;
		uint32_t		numRenderFlags;
		uint32_t		numParticleSystems;
		uint32_t		numRibbonEmitters;

		//skin
		uint32_t		numGeosets;
		uint32_t		numTexUnit;
	};

	struct SRenderFlag
	{
		uint16_t	flags;
		uint16_t blend;

		bool lighting;
		bool zwrite;
	};

	struct SGeoset 
	{
		uint32_t		VCount;
		uint32_t		ICount;
		uint32_t		GeoID;

		int16_t		TexID;
		uint16_t		rfIndex;
		int16_t		ColorIndex;
		int16_t		TransIndex;
		int16_t		TexAnimIndex;
	};

	struct SM2ChildSceneNodes
	{
		IM2SceneNode* head;
		IM2SceneNode* leftShoulder;
		IM2SceneNode* rightShoulder;
		IM2SceneNode* leftHand;
		IM2SceneNode* rightHand;
	};

	struct SFileWMO
	{
		char		name[DEFAULT_SIZE];
		char		longname[256];
		uint32_t		numMaterials;
		uint32_t		numGroups;
		uint32_t		numPortals;
		uint32_t		numLights;
		uint32_t		numModels;
		uint32_t		numDoodads;
		uint32_t		numDoodadSets;
		uint32_t		wmoID;
	};

	struct SWMOGroup
	{
		char	name[DEFAULT_SIZE];
		uint32_t numBatches;
		uint32_t numLights;
		uint32_t numDoodads;
		uint32_t ICount;
		uint32_t VCount;
		bool Indoor;
	};

	struct SWMOPortal
	{
		uint32_t frontGroupIndex;
		uint32_t backGroupIndex;
	};

 }