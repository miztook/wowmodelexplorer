#pragma once

#include "ISceneRenderer.h"

class CMeshRenderer : public ISceneRenderer
{
public:
	CMeshRenderer(u32 quota);
	~CMeshRenderer();

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

			//ÅÅÐò???
			if (ret)
				return ret;
			else
				return Unit->sceneNode < c.Unit->sceneNode;
		}
	};

private:
	E_RENDERINST_TYPE	RenderInstType;

	SRenderUnit*			RenderUnits;
	SEntry*			RenderEntries;
	u32			CurrentRenderCount;
};