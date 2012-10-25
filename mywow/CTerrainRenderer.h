#pragma once

#include "ISceneRenderer.h"

class CTerrainRenderer : public ISceneRenderer
{
private:
	DISALLOW_COPY_AND_ASSIGN(CTerrainRenderer);

public:
	CTerrainRenderer(u32 lowresQuota, u32 highresQuota);
	~CTerrainRenderer();

public:
	virtual void addRenderUnit(const SRenderUnit* unit);
	virtual void render(SRenderUnit*& currentUnit, ICamera* cam);

	//在实际的渲染前后设置fog, dlight, ambient
	virtual void begin_setupLightFog(ICamera* cam);
	virtual void end_setupLightFog();

private:
	struct SEntry
	{
		SRenderUnit*		Unit;

		bool operator<(const SEntry& c) const
		{
			if (Unit->bufferParam.vbuffer0 != c.Unit->bufferParam.vbuffer0)
				return Unit->bufferParam.vbuffer0 < c.Unit->bufferParam.vbuffer0;			//相同adt的vbuffer相同，可减少切换
			else
				return Unit->sceneNode < c.Unit->sceneNode;
		}
	};

private:
	SRenderUnit*			RenderUnits;
	SEntry*			LowRenderEntries;
	SEntry*			HighRenderEntries;
	u32			LowCount, HighCount;
	u32			CurrentRenderCount;

	u32		LowResQuota;
	u32		HighResQuota;
};