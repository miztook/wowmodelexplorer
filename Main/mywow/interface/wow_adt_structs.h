#pragma once

#include "base.h"

namespace ADT
{
	//wow模型定义
#	pragma pack (1)

	struct chunkHeader {
		uint32_t flags;
		uint32_t ix;
		uint32_t iy;
		uint32_t nLayers;
		uint32_t nDoodadRefs;
		uint32_t ofsHeight; // MCVT
		uint32_t ofsNormal; // MCNR
		uint32_t ofsLayer; // MCLY
		uint32_t ofsRefs; // MCRF
		uint32_t ofsAlpha; // MCAL
		uint32_t sizeAlpha;
		uint32_t ofsShadow; // MCSH
		uint32_t sizeShadow;
		uint32_t areaid;
		uint32_t nMapObjRefs;
		uint32_t holes;
		uint16_t s1; // UINT2[8][8] ReallyLowQualityTextureingMap;	// the content is the layer being on top, I guess.
		uint16_t s2;
		uint32_t d1;
		uint32_t d2;
		uint32_t d3;
		uint32_t predTex;
		uint32_t nEffectDoodad;
		uint32_t ofsSndEmitters; // MCSE
		uint32_t nSndEmitters; // will be set to 0 in the client if ofsSndEmitters doesn't point to MCSE!
		uint32_t ofsLiquid; // MCLQ
		uint32_t sizeLiquid; // 8 when not used; only read if >8.
		float  zpos;
		float  xpos;
		float  ypos;
		uint32_t textureId; // MCCV, only with flags&0x40, had UINT32 textureId; in ObscuR's structure.
		uint32_t props;
		uint32_t effectId;
	};


	struct SSoundEmitter
	{
		int32_t SoundEntriesAdvancedId;
		vector3df position;
		vector3df size; 		
	};

	struct SM2Placement
	{
		uint32_t m2Index;
		uint32_t id;
		vector3df pos;
		vector3df dir;
		uint16_t scale;
		uint16_t unknown;
	};

	struct SWMOPlacement 
	{
		uint32_t wmoIndex;
		uint32_t id;
		vector3df pos;
		vector3df dir;
		vector3df pos2;
		vector3df pos3;
		uint16_t flags;
		uint16_t doodadset;
		uint16_t nameset;
		uint16_t unknown;
	};

	struct TXBT 
	{
		uint32_t filenameoffset;
		uint32_t txmdoffset;
		uint8_t sizex;
		uint8_t sizey;
		uint8_t type;
		uint8_t flags;
	};

#define	MCLY_USE_ALPHAMAP		0x100
#define	MCLY_ALPHAMAP_COMPRESS	0x200

	struct MCLY
	{
		uint32_t textureId;
		uint32_t flags;
		uint32_t offsetInMCAL;
		uint16_t effectId;
		uint16_t padding;
	};

#	pragma pack ()

}