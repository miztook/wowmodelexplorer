#pragma once

#include "base.h"

namespace ADT
{
	//wow模型定义
#pragma  pack(push,1)

	struct chunkHeader {
		u32 flags;
		u32 ix;
		u32 iy;
		u32 nLayers;
		u32 nDoodadRefs;
		u32 ofsHeight; // MCVT
		u32 ofsNormal; // MCNR
		u32 ofsLayer; // MCLY
		u32 ofsRefs; // MCRF
		u32 ofsAlpha; // MCAL
		u32 sizeAlpha;
		u32 ofsShadow; // MCSH
		u32 sizeShadow;
		u32 areaid;
		u32 nMapObjRefs;
		u32 holes;
		u16 s1; // UINT2[8][8] ReallyLowQualityTextureingMap;	// the content is the layer being on top, I guess.
		u16 s2;
		u32 d1;
		u32 d2;
		u32 d3;
		u32 predTex;
		u32 nEffectDoodad;
		u32 ofsSndEmitters; // MCSE
		u32 nSndEmitters; // will be set to 0 in the client if ofsSndEmitters doesn't point to MCSE!
		u32 ofsLiquid; // MCLQ
		u32 sizeLiquid; // 8 when not used; only read if >8.
		float  zpos;
		float  xpos;
		float  ypos;
		u32 textureId; // MCCV, only with flags&0x40, had UINT32 textureId; in ObscuR's structure.
		u32 props;
		u32 effectId;
	};


	struct SSoundEmitter
	{
		s32 SoundEntriesAdvancedId;
		vector3df position;
		vector3df size; 		
	};

	struct SM2Placement
	{
		u32 m2Index;
		u32 id;
		vector3df pos;
		vector3df dir;
		u16 scale;
		u16 unknown;
	};

	struct SWMOPlacement 
	{
		u32 wmoIndex;
		u32 id;
		vector3df pos;
		vector3df dir;
		vector3df pos2;
		vector3df pos3;
		u16 flags;
		u16 doodadset;
		u16 nameset;
		u16 unknown;
	};

	struct TXBT 
	{
		u32 filenameoffset;
		u32 txmdoffset;
		u8 sizex;
		u8 sizey;
		u8 type;
		u8 flags;
	};

#define	MCLY_USE_ALPHAMAP		0x100
#define	MCLY_ALPHAMAP_COMPRESS	0x200

	struct MCLY
	{
		u32 textureId;
		u32 flags;
		u32 offsetInMCAL;
		u16 effectId;
		u16 padding;
	};

#pragma pack(pop)

}