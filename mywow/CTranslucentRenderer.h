#pragma once

#include "ISceneRenderer.h"

class CTransluscentRenderer : public ISceneRenderer
{
private:
	DISALLOW_COPY_AND_ASSIGN(CTransluscentRenderer);

public:
	CTransluscentRenderer(u32 quota);
	~CTransluscentRenderer();

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
			//排序
			if (Unit->distance != c.Unit->distance)
				return Unit->distance > c.Unit->distance;			//由远到近
			else if(Unit->priority != c.Unit->priority)
				return Unit->priority > c.Unit->priority;
			else if (Unit->material.MaterialType != c.Unit->material.MaterialType)
				return Unit->material.MaterialType < c.Unit->material.MaterialType;
			else
				return Unit->sceneNode < c.Unit->sceneNode;
		}
	};

private:
	SRenderUnit*			RenderUnits;
	SEntry*			RenderEntries;
	u32			CurrentRenderCount;

	u32		Quota;
};