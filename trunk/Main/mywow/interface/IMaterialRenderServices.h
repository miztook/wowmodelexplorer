#pragma once

#include "base.h"
#include "SMaterial.h"

class IMaterialRenderer;

/*
SMaterial			//vs, per unit
SOverrideMaterial		//vs, global
SRenderStageBlock		//ps, for IMaterialRenderer

then applyMaterialChanges, apply to render pipeline
*/

class IMaterialRenderServices
{
public:
	virtual ~IMaterialRenderServices(){}
};