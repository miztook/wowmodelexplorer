#include "stdafx.h"
#include "CD3D9Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9SceneStateServices.h"

#include "CMeshRenderer.h"
#include "CParticleRenderer.h"
#include "CRibbonRenderer.h"
#include "CTerrainRenderer.h"
#include "CTransluscentRenderer.h"
#include "CWmoRenderer.h"
#include "CMeshDecalRenderer.h"
#include "CTransluscentDecalRenderer.h"
#include "CAlphaTestMeshRenderer.h"
#include "CAlphaTestDecalRenderer.h"
#include "CAlphaTestWmoRenderer.h"
#include "CMeshDecalServices.h"
#include "CParticleSystemServices.h"
#include "CRibbonEmitterServices.h"


void CD3D9Driver::helper_render( CMeshRenderer* meshRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CMeshRenderer::SEntry>& renderEntries = meshRenderer->RenderEntries;
	u32 size = (u32)meshRenderer->RenderEntries.size();

	for (u32 i=0; i<size; ++i)
	{
		const SRenderUnit* unit  = renderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures(
			unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->matView ? *unit->matView: view,
			unit->matProjection ? *unit->matProjection : projection,
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}
}

void CD3D9Driver::helper_render( CTerrainRenderer* terrainRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	CD3D9SceneStateServices* sceneService = D3D9SceneStateServices;

	plane3df clipPlane = cam->getTerrainClipPlane();

	setTransform(cam->getViewMatrix(), cam->getProjectionMatrix());

	std::vector<CTerrainRenderer::SEntry>& highRenderEntries = terrainRenderer->HighRenderEntries;
	std::vector<CTerrainRenderer::SEntry>& lowRenderEntries = terrainRenderer->LowRenderEntries;

	u32 highSize = (u32)highRenderEntries.size();
	u32 lowSize = (u32)lowRenderEntries.size(); 

#ifndef FIXPIPELINE
	cam->makeClipPlane(clipPlane, clipPlane);
#endif

	sceneService->setClipPlane(0, clipPlane);
	sceneService->enableClipPlane(0, true);

	for (u32 i=0; i<highSize; ++i)
	{
		const SRenderUnit* unit  = highRenderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures(
			unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);	
	}

	sceneService->enableClipPlane(0, false);

	for (u32 i=0; i<lowSize; ++i)
	{
		const SRenderUnit* unit  = lowRenderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures( unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}
}

void CD3D9Driver::helper_render( CTransluscentRenderer* transluscentRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	std::vector<CTransluscentRenderer::SEntry>& renderEntries = transluscentRenderer->RenderEntries;
	u32 size = (u32)transluscentRenderer->RenderEntries.size();

	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	for (u32 i=0; i<size; ++i)
	{
		const SRenderUnit* unit  = renderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures(
			unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->matView ? *unit->matView: view,
			unit->matProjection ? *unit->matProjection : projection,
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}
}

void CD3D9Driver::helper_render( CWmoRenderer* wmoRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	CD3D9SceneStateServices* sceneService = D3D9SceneStateServices;
	std::vector<CWmoRenderer::SEntry>& renderEntries = wmoRenderer->RenderEntries;

	plane3df clipPlane = cam->getTerrainClipPlane();

	setTransform(cam->getViewMatrix(), cam->getProjectionMatrix());

#ifndef FIXPIPELINE
	cam->makeClipPlane(clipPlane, clipPlane);
#endif

	sceneService->setClipPlane(0, clipPlane);
	sceneService->enableClipPlane(0, true);

	u32 size = (u32)renderEntries.size();
	for (u32 i=0; i<size; ++i)
	{
		SRenderUnit* unit  = renderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures( 
			unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}

	sceneService->enableClipPlane(0, false);
}

void CD3D9Driver::helper_render( CAlphaTestMeshRenderer* meshRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CAlphaTestMeshRenderer::SEntry>& renderEntries = meshRenderer->RenderEntries;
	u32 size = (u32)meshRenderer->RenderEntries.size();

	for (u32 i=0; i<size; ++i)
	{
		const SRenderUnit* unit  = renderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures(
			unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->matView ? *unit->matView: view,
			unit->matProjection ? *unit->matProjection : projection,
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}
}

void CD3D9Driver::helper_render( CAlphaTestWmoRenderer* wmoRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	CD3D9SceneStateServices* sceneService = D3D9SceneStateServices;
	std::vector<CAlphaTestWmoRenderer::SEntry>& renderEntries = wmoRenderer->RenderEntries;

	plane3df clipPlane = cam->getTerrainClipPlane();

	setTransform(cam->getViewMatrix(), cam->getProjectionMatrix());

#ifndef FIXPIPELINE
	cam->makeClipPlane(clipPlane, clipPlane);
#endif

	sceneService->setClipPlane(0, clipPlane);
	sceneService->enableClipPlane(0, true);

	u32 size = (u32)renderEntries.size();
	for (u32 i=0; i<size; ++i)
	{
		const SRenderUnit* unit  = renderEntries[i].unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		setTransform_Material_Textures( 
			unit->matWorld ? *unit->matWorld : matrix4::Identity(),
			unit->material,
			unit->textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}

	sceneService->enableClipPlane(0, false);
}

void CD3D9Driver::helper_renderAllBatches( CParticleRenderer* particleRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CParticleRenderer::SBatch>& renderBatches = particleRenderer->RenderBatches;
	CParticleSystemServices* particleRenderServices = particleRenderer->ParticleServices;
	u32 size = (u32)renderBatches.size();

	//render batches
	for (u32 i=0; i<size; ++i)
	{
		const CParticleRenderer::SBatch& batch = renderBatches[i];
		u32 primCount = batch.vcount / 2;

		if (!primCount)
			continue;

		currentUnit = batch.firstUnit;

		SDrawParam drawParam = {0};

#ifdef USE_WITH_GLES2
		drawParam.voffset0 =  batch.vbase;
#else
		drawParam.baseVertIndex = batch.vbase;
#endif
		drawParam.numVertices = batch.vcount;

		ITexture* textures[MATERIAL_MAX_TEXTURES] = {0};
		textures[0] = batch.texture0;

		setTransform_Material_Textures(
			batch.matWorld ? *batch.matWorld : matrix4::Identity(),
			batch.matView ? *batch.matView : view,
			batch.matProjection ? *batch.matProjection : projection,
			batch.material,
			textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(particleRenderServices->BufferParam, EPT_TRIANGLES, primCount, drawParam);
	}

	renderBatches.clear();
}

void CD3D9Driver::helper_renderAllBatches( CRibbonRenderer* ribbonRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CRibbonRenderer::SBatch>& renderBatches = ribbonRenderer->RenderBatches;
	CRibbonEmitterServices* ribbonServices = ribbonRenderer->RibbonServices;
	u32 size = (u32)renderBatches.size();

	//render batches
	for (u32 i=0; i<size; ++i)
	{
		CRibbonRenderer::SBatch& batch = renderBatches[i];

		currentUnit = batch.firstUnit;

		SDrawParam drawParam = {0};

#ifdef USE_WITH_GLES2
		drawParam.voffset0 =  batch.vbase;
#else
		drawParam.baseVertIndex = batch.vbase;
#endif
		drawParam.numVertices = batch.vcount;

		u32 primCount = batch.vcount / 2;

		ITexture* textures[MATERIAL_MAX_TEXTURES] = {0};
		textures[0] = batch.texture0;

		setTransform_Material_Textures(
			batch.matWorld ? *batch.matWorld : matrix4::Identity(),
			batch.matView ? *batch.matView : view,
			batch.matProjection ? *batch.matProjection : projection,
			batch.material,
			textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(ribbonServices->BufferParam, EPT_TRIANGLE_STRIP, primCount, drawParam);
	}

	renderBatches.clear();
}

void CD3D9Driver::helper_renderAllBatches( CMeshDecalRenderer* meshDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CMeshDecalRenderer::SBatch>& renderBatches = meshDecalRenderer->RenderBatches;
	CMeshDecalServices* meshDecalServices = meshDecalRenderer->MeshDecalServices;
	u32 size = (u32)renderBatches.size();

	//render batches
	for (u32 i=0; i<size; ++i)
	{
		const CMeshDecalRenderer::SBatch& batch = renderBatches[i];
		u32 primCount = batch.vcount / 2;

		if (!primCount)
			continue;

		currentUnit = batch.firstUnit;

		SDrawParam drawParam = {0};

#ifdef USE_WITH_GLES2
		drawParam.voffset0 =  batch.vbase;
#else
		drawParam.baseVertIndex = batch.vbase;
#endif

		drawParam.numVertices = batch.vcount;

		ITexture* textures[MATERIAL_MAX_TEXTURES] = {0};
		textures[0] = batch.texture0;

		setTransform_Material_Textures(
			batch.matWorld ? *batch.matWorld : matrix4::Identity(),
			batch.matView ? *batch.matView : view,
			batch.matProjection ? *batch.matProjection : projection,
			batch.material,
			textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(meshDecalServices->BufferParam, EPT_TRIANGLES, primCount, drawParam);
	}

	renderBatches.clear();
}

void CD3D9Driver::helper_renderAllBatches( CTransluscentDecalRenderer* transDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CTransluscentDecalRenderer::SBatch>& renderBatches = transDecalRenderer->RenderBatches;
	CMeshDecalServices* meshDecalServices = transDecalRenderer->MeshDecalServices;
	u32 size = (u32)renderBatches.size();

	//render batches
	for (u32 i=0; i<size; ++i)
	{
		const CTransluscentDecalRenderer::SBatch& batch = renderBatches[i];
		u32 primCount = batch.vcount / 2;

		if (!primCount)
			continue;

		currentUnit = batch.firstUnit;

		SDrawParam drawParam = {0};

#ifdef USE_WITH_GLES2
		drawParam.voffset0 =  batch.vbase;
#else
		drawParam.baseVertIndex = batch.vbase;
#endif

		drawParam.numVertices = batch.vcount;

		ITexture* textures[MATERIAL_MAX_TEXTURES] = {0};
		textures[0] = batch.texture0;

		setTransform_Material_Textures(
			batch.matWorld ? *batch.matWorld : matrix4::Identity(),
			batch.matView ? *batch.matView : view,
			batch.matProjection ? *batch.matProjection : projection,
			batch.material,
			textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(meshDecalServices->BufferParam, EPT_TRIANGLES, primCount, drawParam);
	}

	renderBatches.clear();
}

void CD3D9Driver::helper_renderAllBatches( CAlphaTestDecalRenderer* meshDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam )
{
	matrix4 view = cam->getViewMatrix();
	matrix4 projection = cam->getProjectionMatrix();

	std::vector<CAlphaTestDecalRenderer::SBatch>& renderBatches = meshDecalRenderer->RenderBatches;
	CMeshDecalServices* meshDecalServices = meshDecalRenderer->MeshDecalServices;
	u32 size = (u32)renderBatches.size();

	//render batches
	for (u32 i=0; i<size; ++i)
	{
		const CAlphaTestDecalRenderer::SBatch& batch = renderBatches[i];
		u32 primCount = batch.vcount / 2;

		if (!primCount)
			continue;

		currentUnit = batch.firstUnit;

		SDrawParam drawParam = {0};

#ifdef USE_WITH_GLES2
		drawParam.voffset0 =  batch.vbase;
#else
		drawParam.baseVertIndex = batch.vbase;
#endif

		drawParam.numVertices = batch.vcount;

		ITexture* textures[MATERIAL_MAX_TEXTURES] = {0};
		textures[0] = batch.texture0;

		setTransform_Material_Textures(
			batch.matWorld ? *batch.matWorld : matrix4::Identity(),
			batch.matView ? *batch.matView : view,
			batch.matProjection ? *batch.matProjection : projection,
			batch.material,
			textures,
			MATERIAL_MAX_TEXTURES);

		draw3DMode(meshDecalServices->BufferParam, EPT_TRIANGLES, primCount, drawParam);
	}

	renderBatches.clear();
}

#endif