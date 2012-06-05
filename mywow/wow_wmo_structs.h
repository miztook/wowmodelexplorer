#pragma once

#include "base.h"

namespace WMO
{
	//wow模型定义
#pragma  pack(push,1)

	struct wmoHeader
	{
		u32 nMaterials;
		u32 nGroups;
		u32 nPortals;
		u32 nLights;
		u32 nModels;
		u32 nDoodads;
		u32 nDoodadSets;
		u32 color;
		u32 wmoID;
		vector3df minBox;
		vector3df maxBox;
		u32 liquidType;
	};

	struct wmoMaterial
	{
		u32 flags;
		u32 specular;
		u32 transparent; // Blending: 0 for opaque, 1 for transparent
		s32 tex1;
		u8 col1R;
		u8 col1G;
		u8 col1B;
		u8 col1A;
		u32 flag1;
		s32 tex2;
		u8 col2R;
		u8 col2G;
		u8 col2B;
		u8 col2A;
		u32 flag2;
		u32 col3;
		u32 unknown[2];
		u32 runtime[4];
	};

	struct wmoGroupInfo
	{
		u32 flags;
		vector3df min;
		vector3df max;
		u32 nameIndex;
	};

	struct wmoPortalInfo
	{
		u16 baseIndex;
		u16 numVerts;
		vector3df normal;
		float unknown;
	};

	struct wmoLight
	{
		u8 lighttype;
		u8 type;
		u8 useatten;
		u8 pad;
		u8 colB;
		u8 colG;
		u8 colR;
		u8 colA;
		vector3df pos;
		f32 intensity;
		f32 attenStart;
		f32 attenEnd;
		f32 unk[4];
	};

	struct wmoDoodadSet
	{
		c8 name[20];
		u32 start;			//index of first doodad instance in this set
		u32 count;					//number of doodad instances in this set
		u32 unused;
	};

	struct wmoDoodadDef
	{
		u32 nameIndex;
		vector3df pos;
		f32 rot[4];
		f32 scale;
		u8 colB;
		u8 colG;
		u8 colR;
		u8 colA;
	};

	struct wmoFog
	{
		u32 flags;
		vector3df pos;
		f32 r1;
		f32 r2;
		f32 fogend;
		f32 fogstart;
		u8 colB;
		u8 colG;
		u8 colR;
		u8 colA;
		f32 unknown[2];
		u32 color2;
	};

	struct wmoGroupHeader
	{
		u32 groupNameIndex;
		u32 descriptionNameIndex;
		u32 flags;
		vector3df   minbox;
		vector3df	 maxbox;
		u16 portalStart, portalCount;
		u16 batches[4];
		u8	fogs[4];
		u32 unk1;
		u32 groupid;
		u32 unk[2];
	};

	struct wmoGroupMaterial
	{
		u8	flags;
		u8	matId;
	};

	struct wmoGroupBatch
	{
		c8 name[12];
		u32 indexStart;
		u16 indexCount;
		u16 vertexStart;
		u16 vertexEnd;
		u8 flags;
		u8 matId;
	};

	struct wmoGroupLiquidHeader {
		s32 X;
		s32 Y;
		s32 A;
		s32 B;
		vector3df pos;
		short type;
	};

#pragma pack(pop)

}