#pragma once

#include "core.h"
#include "SMaterial.h"
#include "VertexIndexBuffer.h"

class ISceneNode;
class ITexture;
class ICamera;

struct SRenderUnit
{
	union	URender
	{
		struct		//mesh
		{
			const SBoneMatrixArray*		boneMatrixArray;				//¹Ç÷À¾ØÕó		
			u16		geoset;
			bool	useBoneMatrix;
			s8		priority;
		};

		struct			//for terrain
		{
			const void*		chunk;
			const void*		adt;
			bool		lowres;
		};

		struct		//for decal
		{
			s32		decalType;			//E_DECAL_TYPE
			s32		fontTextureIndex;		//font 1,2,3
		};
	};

	SBufferParam	bufferParam;
	SDrawParam		drawParam;
	SMaterial		material;
	ITexture*		textures[MATERIAL_MAX_TEXTURES];
	const matrix4*		matWorld;
	const matrix4*		matView;
	const matrix4*		matProjection;
	const ISceneNode*			sceneNode;
	u32		primCount;
	f32		distance;
	E_PRIMITIVE_TYPE	primType;

	URender	u;
};

class ISceneRenderer
{
public:
	virtual ~ISceneRenderer() {}

public:

};