#pragma once

#include "ISceneRenderer.h"

class CMeshRenderer : public ISceneRenderer
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMeshRenderer);

public:
	CMeshRenderer(u32 quota);
	~CMeshRenderer();

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
			if (Unit->material.MaterialType != c.Unit->material.MaterialType)
				return Unit->material.MaterialType < c.Unit->material.MaterialType;
			else if (Unit->distance != c.Unit->distance)
				return Unit->distance < c.Unit->distance;			//由近到远
			else
				return Unit->sceneNode < c.Unit->sceneNode;
		}
	};

private:
	E_RENDERINST_TYPE	RenderInstType;

	SRenderUnit*			RenderUnits;
	SEntry*			RenderEntries;
	u32			CurrentRenderCount;

	u32		Quota;
};