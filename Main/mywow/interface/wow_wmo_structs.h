#pragma once

#include "base.h"

namespace WMO
{
	//wow模型定义
#	pragma pack (1)

	struct wmoHeader
	{
		uint32_t nMaterials;
		uint32_t nGroups;
		uint32_t nPortals;
		uint32_t nLights;
		uint32_t nModels;
		uint32_t nDoodads;
		uint32_t nDoodadSets;
		uint32_t color;
		uint32_t wmoID;
		vector3df minBox;
		vector3df maxBox;
		uint32_t liquidType;
	};

	struct wmoMaterial
	{
		uint32_t flags;
		uint32_t shadertype;		//enum shaderType
		uint32_t alphatest; // Blending: 0 for opaque, 1 for transparent
		int32_t tex1;
		uint8_t col1R;
		uint8_t col1G;
		uint8_t col1B;
		uint8_t col1A;
		uint32_t flag1;
		int32_t tex2;
		uint8_t col2R;
		uint8_t col2G;
		uint8_t col2B;
		uint8_t col2A;
		uint32_t flag2;
		uint8_t col3R;
		uint8_t col3G;
		uint8_t col3B;
		uint8_t col3A;
		uint32_t unknown[2];
		uint32_t runtime[4];
	};

	struct wmoGroupInfo
	{
		uint32_t flags;
		vector3df min;
		vector3df max;
		int32_t nameIndex;
	};

	struct wmoPortalInfo
	{
		uint16_t baseIndex;
		uint16_t numVerts;
		vector3df normal;
		float d;
	};

	struct wmoPortalRelation
	{
		uint16_t portal;
		uint16_t group;
		int16_t dir;
		int16_t unknown;
	};

	struct wmoLight
	{
		uint8_t lighttype;
		uint8_t type;
		uint8_t useatten;
		uint8_t pad;
		uint8_t colB;
		uint8_t colG;
		uint8_t colR;
		uint8_t colA;
		vector3df pos;
		float intensity;
		float attenStart;
		float attenEnd;
		float unk[4];
	};

	struct wmoDoodadSet
	{
		char name[20];
		uint32_t start;			//index of first doodad instance in this set
		uint32_t count;					//number of doodad instances in this set
		uint32_t unused;
	};

	struct wmoDoodadDef
	{
		uint32_t nameIndex;
		vector3df pos;
		float rot[4];
		float scale;
		uint8_t colB;
		uint8_t colG;
		uint8_t colR;
		uint8_t colA;
	};

	struct wmoFog
	{
		uint32_t flags;
		vector3df pos;
		float r1;
		float r2;
		float fogend;
		float fogstart;
		uint8_t colB;
		uint8_t colG;
		uint8_t colR;
		uint8_t colA;
		float unknown[2];
		uint32_t color2;
	};

	struct wmoGroupHeader
	{
		uint32_t groupNameIndex;
		uint32_t descriptionNameIndex;
		uint32_t flags;
		vector3df   minbox;
		vector3df	 maxbox;
		uint16_t portalStart, portalCount;
		uint16_t batches[4];
		uint8_t	fogs[4];
		uint32_t unk1;
		uint32_t groupid;
		uint32_t unk[2];
	};

	struct wmoTriangleMaterial
	{
		uint8_t	flags;
		uint8_t	matId;
	};

	struct wmoGroupBatch
	{
		char name[12];
		uint32_t indexStart;
		uint16_t indexCount;
		uint16_t vertexStart;
		uint16_t vertexEnd;
		uint8_t flags;
		uint8_t matId;
	};

	struct wmoGroupLiquidHeader {
		int32_t X;
		int32_t Y;
		int32_t A;
		int32_t B;
		vector3df pos;
		short type;
	};

	struct wmoBspNode
	{
		uint16_t planetype;		//4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane?
		int16_t children[2];
		uint16_t numfaces;
		uint16_t firstface;
		uint16_t unknown;
		float distance;
	};

#	pragma pack ()

}