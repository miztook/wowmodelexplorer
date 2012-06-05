#pragma once

#include "base.h"
#include "SMaterial.h"

//根据不同的material设置不同的渲染状态
class IMaterialRenderer
{
public:
	virtual ~IMaterialRenderer() {}

public:
	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderStates ) {}

	virtual void OnUnsetMaterial() {}

	//for fp
	virtual u32 getNumPasses() const { return 1; }

	virtual void OnRender(const SMaterial& material, u32 pass) {}

public:
	SRenderStateBlock		MaterialBlock;
};

