#pragma once

#include "core.h"
#include "SMaterial.h"

class ISceneNode;
class IVertexBuffer;
class IIndexBuffer;
class ITexture;
class ICamera;

struct SRenderUnit
{
	union	URender
	{
		struct		//mesh
		{
			bool		useBoneMatrix;
			SBoneMatrixArray*		boneMatrixArray;				//¹Ç÷À¾ØÕó
		};

		struct			//for terrain
		{
			void*		chunk;
			bool		lowRes;
		};
	};

	IVertexBuffer*		vbuffer;				//1 stream
	IVertexBuffer*		vbuffer2;			//2 stream
	IIndexBuffer*		ibuffer;
	E_PRIMITIVE_TYPE	primType;
	u32		primCount;
	SDrawParam		drawParam;
	SMaterial		material;
	ITexture*		textures[MATERIAL_MAX_TEXTURES];
	matrix4*		matWorld;
	matrix4*		matView;
	matrix4*		matProjection;
	ISceneNode*			sceneNode;
	
	URender	u;
};

class ISceneRenderer
{
public:
	ISceneRenderer(u32 quota) : Quota(quota) {}
	virtual ~ISceneRenderer() {}

public:
	virtual void addRenderUnit(const SRenderUnit* unit) = 0;
	virtual void render(SRenderUnit*& currentUnit, ICamera* cam) = 0;

protected:
	u32		Quota;
};