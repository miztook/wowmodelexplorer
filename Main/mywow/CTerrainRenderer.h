#pragma once

#include "ISceneRenderer.h"
#include <vector>

class CTerrainRenderer : public ISceneRenderer
{
public:
	CTerrainRenderer(u32 lowresQuota, u32 highresQuota);
	~CTerrainRenderer();

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
			if (unit->material.PsType != c.unit->material.PsType)
				return unit->material.PsType < c.unit->material.PsType;
			else if (unit->bufferParam.vbuffer0 != c.unit->bufferParam.vbuffer0)
				return unit->bufferParam.vbuffer0 < c.unit->bufferParam.vbuffer0;			//相同adt的vbuffer相同，可减少切换
			else if (unit->textures[0] != c.unit->textures[0])
				return unit->textures[0] < c.unit->textures[0];
			else if (unit->textures[1] != c.unit->textures[1])
				return unit->textures[1] < c.unit->textures[1];
			else
				return unit < c.unit;
		}
	};

private:
	std::vector<SRenderUnit>		RenderUnits;
	std::vector<SEntry>			LowRenderEntries;
	std::vector<SEntry>			HighRenderEntries;

	u32		LowResQuota;
	u32		HighResQuota;


	friend class CD3D9Driver;
	friend class CD3D11Driver;
	friend class COpenGLDriver;
	friend class CGLES2Driver;
};