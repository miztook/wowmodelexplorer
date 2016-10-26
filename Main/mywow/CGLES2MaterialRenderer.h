#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IMaterialRenderer.h"

class CGLES2MaterialRenderer_Solid : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
};

class CGLES2MaterialRenderer_Transparent_Alpha_Blend : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

class CGLES2MaterialRenderer_Transparent_Alpha_Test : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

class CGLES2MaterialRenderer_Terrain_MultiPass : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

#endif