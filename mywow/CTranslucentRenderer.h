#pragma once

#include "ISceneRenderer.h"

class CTransluscentRenderer : public ISceneRenderer
{
public:
	CTransluscentRenderer(u32 quota);
	~CTransluscentRenderer();

public:
	virtual void addRenderUnit(const SRenderUnit* unit);
	virtual void render(SRenderUnit*& currentUnit, ICamera* cam);

private:
	struct SEntry
	{
		SRenderUnit*		Unit;

		bool operator<(const SEntry& c) const
		{
			bool ret = Unit->material.MaterialType < c.Unit->material.MaterialType;

			if (ret)
				return ret;
			else
				return Unit->sceneNode < c.Unit->sceneNode;
		}
	};

private:
	SRenderUnit*			RenderUnits;
	SEntry*			RenderEntries;
	u32			CurrentRenderCount;;
};