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
			bool		useBoneMatrix;
			SBoneMatrixArray*		boneMatrixArray;				//骨骼矩阵		
		};

		struct			//for terrain
		{
			void*		chunk;
			void*		adt;
			bool		lowres;
		};
	};

	SBufferParam	bufferParam;
	E_PRIMITIVE_TYPE	primType;
	u32		primCount;
	SDrawParam		drawParam;

	SMaterial		material;
	ITexture*		textures[MATERIAL_MAX_TEXTURES];
	matrix4*		matWorld;
	matrix4*		matView;
	matrix4*		matProjection;
	ISceneNode*			sceneNode;
	f32	distance;
	s8		priority;

	URender	u;
};

class ISceneRenderer
{
public:
	virtual ~ISceneRenderer() {}

public:
	virtual void addRenderUnit(const SRenderUnit* unit) = 0;
	virtual void render(SRenderUnit*& currentUnit, ICamera* cam) = 0;

	//在实际的渲染前后设置fog, dlight, ambient
	virtual void begin_setupLightFog(ICamera* cam) = 0;
	virtual void end_setupLightFog() = 0;
};