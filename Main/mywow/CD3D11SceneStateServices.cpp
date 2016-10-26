#include "stdafx.h"
#include "CD3D11SceneStateServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"

CD3D11SceneStateServices::CD3D11SceneStateServices()
{
	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	pID3DDevice = Driver->pID3DDevice11;

	setAmbientLight(SColor(255,255,255));
	setFog(SFogParam());
	deleteAllDynamicLights();
}

void CD3D11SceneStateServices::setAmbientLight( SColor color )
{
	SceneState.AmbientLightColor = color;
}

void CD3D11SceneStateServices::deleteAllDynamicLights()
{
	SceneState.resetLights();
}

bool CD3D11SceneStateServices::setDynamicLight( u32 index, const SLight& light )
{
	if(index >= SceneState.MAX_LIGTHTS)
		return false;

	SceneState.Lights[index] = light;
	return true;
}

void CD3D11SceneStateServices::turnLightOn( u32 lightIndex, bool turnOn )
{
	if(lightIndex >= SceneState.MAX_LIGTHTS)
		return;

	SceneState.LightsOn[lightIndex] = turnOn;
}

const SLight* CD3D11SceneStateServices::getDynamicLight( u32 index ) const
{
	if(index >= SceneState.MAX_LIGTHTS)
		return NULL_PTR;

	return &SceneState.Lights[index];
}

void CD3D11SceneStateServices::setFog( const SFogParam& fogParam )
{
	SceneState.FogParam = fogParam;
}

bool CD3D11SceneStateServices::setClipPlane( u32 index, const plane3df& plane )
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return false;
	}

	SceneState.clipplanes[index] = plane;
	return true;
}

void CD3D11SceneStateServices::enableClipPlane( u32 index, bool enable )
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return;
	}

	SceneState.enableclips[index] = enable;
}

bool CD3D11SceneStateServices::isClipPlaneEnable( u32 index ) const
{
	if (index >= SSceneState::MAX_CLIPPLANES)
	{
		ASSERT(false);
		return false;
	}

	return SceneState.enableclips[index];
}

bool CD3D11SceneStateServices::getClipPlane( u32 index, plane3df& plane ) const
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