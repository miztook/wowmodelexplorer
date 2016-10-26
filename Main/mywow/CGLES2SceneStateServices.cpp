#include "stdafx.h"
#include "CGLES2SceneStateServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2Helper.h"

CGLES2SceneStateServices::CGLES2SceneStateServices()
{
	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	Extension = Driver->getGLExtension();

	setAmbientLight(SColor(255,255,255));
	setFog(SFogParam());
	deleteAllDynamicLights();
}

CGLES2SceneStateServices::~CGLES2SceneStateServices()
{

}

void CGLES2SceneStateServices::setAmbientLight( SColor color )
{
	SceneState.AmbientLightColor = color;
}

void CGLES2SceneStateServices::deleteAllDynamicLights()
{
	SceneState.resetLights();
}

bool CGLES2SceneStateServices::setDynamicLight( u32 index, const SLight& light )
{
	if (index >= SceneState.MAX_LIGTHTS)
		return false;

	SceneState.Lights[index] = light;
	return true;
}

const SLight* CGLES2SceneStateServices::getDynamicLight( u32 index ) const
{
	if(index >= SceneState.MAX_LIGTHTS)
		return NULL;

	return &SceneState.Lights[index];
}

void CGLES2SceneStateServices::turnLightOn( u32 lightIndex, bool turnOn )
{
	if(lightIndex >= SceneState.MAX_LIGTHTS)
		return;

	SceneState.LightsOn[lightIndex] = turnOn;
}

void CGLES2SceneStateServices::setFog( const SFogParam& fogParam )
{
	SceneState.FogParam = fogParam;
}

bool CGLES2SceneStateServices::setClipPlane( u32 index, const plane3df& plane )
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return false;
	}

	SceneState.clipplanes[index] = plane;
	return true;
}

void CGLES2SceneStateServices::enableClipPlane( u32 index, bool enable )
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return;
	}

	SceneState.enableclips[index] = enable;
}

bool CGLES2SceneStateServices::isClipPlaneEnable( u32 index ) const
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return false;
	}

	return SceneState.enableclips[index];
}

bool CGLES2SceneStateServices::getClipPlane( u32 index, plane3df& plane ) const
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return false;
	}

	plane = SceneState.clipplanes[index];
	return true;
}



#endif