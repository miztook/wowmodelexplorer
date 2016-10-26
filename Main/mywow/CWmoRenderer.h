#pragma once

#include "ISceneRenderer.h"

class CWmoRenderer : public ISceneRenderer
{
public:
	explicit CWmoRenderer(u32 quota);
	~CWmoRenderer();

public:
	void addRenderUnit(const SRenderUnit* unit);
	void render(const SRenderUnit*& currentUnit, ICamera* cam);

	//在实际的渲染前后设置fog, dlight, ambient
	void begin_setupLightFog(ICamera* cam) const;
	void end_setupLightFog() const;

private:
	struct SEntry
	{
		SRenderUnit* unit;

		bool operator<(const SEntry& c) const
		{
			if(unit->material.MaterialType != c.unit->material.MaterialType)
				return unit->material.MaterialType < c.unit->material.MaterialType;
			else if (unit->material.PsType != c.unit->material.PsType)
				return unit->material.PsType < c.unit->material.PsType;
			else if (unit->material.VertexShader != c.unit->material.VertexShader)
				return unit->material.VertexShader < c.unit->material.VertexShader;
			else if (unit->bufferParam.vbuffer0 != c.unit->bufferParam.vbuffer0)
					return unit->bufferParam.vbuffer0 < c.unit->bufferParam.vbuffer0;
			else if (unit->distance != c.unit->distance)
				return unit->distance < c.unit->distance;
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