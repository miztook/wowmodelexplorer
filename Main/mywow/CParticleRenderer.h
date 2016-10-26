#pragma once

#include "ISceneRenderer.h"
#include <vector>

class CParticleSystemServices;

class CParticleRenderer : public ISceneRenderer
{
public:
	explicit CParticleRenderer(u32 quota);
	~CParticleRenderer();

public:
	void addRenderUnit(const SRenderUnit* unit);
	void render(const SRenderUnit*& currentUnit, ICamera* cam);

	//在实际的渲染前后设置fog, dlight, ambient
	void begin_setupLightFog(ICamera* cam) const;
	void end_setupLightFog() const;

private:
	struct SEntry
	{
		const SRenderUnit* unit;

		bool operator<(const SEntry& c) const				//distance, material, textures, wvp相同的尽量合批次
		{
			//排序
			if(unit->material.MaterialType != c.unit->material.MaterialType)
				return unit->material.MaterialType < c.unit->material.MaterialType;
			else if (unit->material.PsType != c.unit->material.PsType)
				return unit->material.PsType < c.unit->material.PsType;
			else if (unit->textures[0] != c.unit->textures[0])
				return unit->textures[0] < c.unit->textures[0];
			else if (unit->material.EmissiveColor != c.unit->material.EmissiveColor)
				return unit->material.EmissiveColor < c.unit->material.EmissiveColor;
			else
				return unit < c.unit;
		}
	};

	struct SBatch
	{
		SMaterial		material;
		ITexture*		texture0;
		const matrix4*		matWorld;
		const matrix4*		matView;
		const matrix4*		matProjection;

		u32 vbase;
		u32 vcount;

		const SRenderUnit*	firstUnit;			//保存一个unit，为shader参数等使用
	};

private:
	void renderAllBatches(const SRenderUnit*& currentUnit, ICamera* cam);
	bool isInBatch(const SBatch& batch, const SRenderUnit* unit) const;
	bool isBatchExceed(const SBatch& batch, u32 vcount) const;
	void addNewBatch(const SRenderUnit* unit);

private:
	std::vector<SRenderUnit>		RenderUnits;
	std::vector<SEntry>			RenderEntries;
	std::vector<SBatch>		RenderBatches;

	CParticleSystemServices*		ParticleServices;
	u32		Quota;

	friend class CD3D9Driver;
	friend class CD3D11Driver;
	friend class COpenGLDriver;
	friend class CGLES2Driver;
};