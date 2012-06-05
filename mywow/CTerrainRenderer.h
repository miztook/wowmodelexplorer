#pragma once

#include "ISceneRenderer.h"

class CTerrainRenderer : public ISceneRenderer
{
public:
	CTerrainRenderer(u32 quota);
	~CTerrainRenderer();

public:
	virtual void addRenderUnit(const SRenderUnit* unit);
	virtual void render(SRenderUnit*& currentUnit, ICamera* cam);

private:
	struct SEntry
	{
		SRenderUnit*		Unit;

		bool operator<(const SEntry& c) const
		{
			if(Unit->u.lowRes == c.Unit->u.lowRes) 
				return Unit->material.MaterialType < c.Unit->material.MaterialType;
			else
				return Unit->u.lowRes;
		}
	};

private:
	SRenderUnit*			RenderUnits;
	SEntry*			RenderEntries;
	u32			CurrentRenderCount;
	bool		WireFrame;
};