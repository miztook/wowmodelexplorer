#pragma once

#include "ISceneRenderer.h"
#include <vector>

class CTransluscentRenderer : public ISceneRenderer
{
public:
	explicit CTransluscentRenderer(u32 quota);
	~CTransluscentRenderer();

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

		bool operator<(const SEntry& c) const
		{
			//排序
			if (unit->distance != c.unit->distance)
				return unit->distance > c.unit->distance;			//由远到近
			else if(unit->u.priority != c.unit->u.priority)
				return unit->u.priority > c.unit->u.priority;
			else if (unit->material.MaterialType != c.unit->material.MaterialType)
				return unit->material.MaterialType < c.unit->material.MaterialType;
			else if (unit->material.PsType != c.unit->material.PsType)
				return unit->material.PsType < c.unit->material.PsType;
			else if (unit->material.VertexShader != c.unit->material.VertexShader)
				return unit->material.VertexShader < c.unit->material.VertexShader;
			else if (unit->textures[0] != c.unit->textures[0])
				return unit->textures[0] < c.unit->textures[0];
			else
				return unit < c.unit;
		}
	};

private:
	std::vector<SRenderUnit>		RenderUnits;
	std::vector<SEntry>			RenderEntries;

	u32		Quota;

	friend class CD3D9Driver;
	friend class CD3D11Driver;
	friend class COpenGLDriver;
	friend class CGLES2Driver;
};