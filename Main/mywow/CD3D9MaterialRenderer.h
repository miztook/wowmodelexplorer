#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IMaterialRenderer.h"

class CD3D9MaterialRenderer_Solid : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
};

class CD3D9MaterialRenderer_Transparent_Alpha_Blend : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

class CD3D9MaterialRenderer_Transparent_Alpha_Test : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

class CD3D9MaterialRenderer_Terrain_MultiPass : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

#endif