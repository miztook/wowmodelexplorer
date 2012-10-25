#include "stdafx.h"
#include "CD3D9SceneStateServices.h"
#include "mywow.h"
#include "CD3D9Driver.h"

#define  DEVICE_SET_RENDER_STATE(prop, d3drs, v)	if (SSCache.##prop != v)		\
		{	pID3DDevice->SetRenderState(d3drs, v);				\
		SSCache.##prop = v;	} 

CD3D9SceneStateServices::CD3D9SceneStateServices()
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	pID3DDevice = Driver->pID3DDevice;

	resetSceneStateCache();

	ResetRenderStates = true;

	setAmbientLight(SColor(255,255,255));
	setFog(SFogParam());
	deleteAllDynamicLights();
}

CD3D9SceneStateServices::~CD3D9SceneStateServices()
{

}


void CD3D9SceneStateServices::deleteAllDynamicLights()
{
	for (u32 i=0; i<SceneState.MAX_LIGTHTS; ++i)
	{
		if (ResetRenderStates || SceneState.LightsOn[i] != false)
			pID3DDevice->LightEnable(i, false);
	}

	SceneState.resetLights();
}

bool CD3D9SceneStateServices::setDynamicLight( u32 index, const SLight& dl )
{
	if(index >= min_(SceneState.MAX_LIGTHTS, (u32)Driver->Caps.MaxActiveLights))
		return false;

	D3DLIGHT9 light;

	switch (dl.Type)
	{
	case ELT_POINT:
		light.Type = D3DLIGHT_POINT;
		break;
	case ELT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		break;
	case ELT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		break;
	}

	light.Position = *(D3DVECTOR*)((void*)(&dl.Position));
	light.Direction = *(D3DVECTOR*)((void*)(&dl.Direction));

	light.Range = min_(dl.Radius, MAX_DLIGHT_RADIUS);
	light.Falloff = dl.Falloff;

	light.Diffuse = *(D3DCOLORVALUE*)((void*)(&dl.DiffuseColor));
	light.Specular = *(D3DCOLORVALUE*)((void*)(&dl.SpecularColor));
	light.Ambient = *(D3DCOLORVALUE*)((void*)(&dl.AmbientColor));

	light.Attenuation0 = dl.Attenuation.X;
	light.Attenuation1 = dl.Attenuation.Y;
	light.Attenuation2 = dl.Attenuation.Z;

	light.Theta = dl.InnerCone * 2.0f * DEGTORAD;
	light.Phi = dl.OuterCone * 2.0f * DEGTORAD;

	if (D3D_OK == pID3DDevice->SetLight(index, &light))
	{
		SceneState.Lights[index] = dl;
		return true;
	}
	return false;
}

void CD3D9SceneStateServices::turnLightOn( u32 lightIndex, bool turnOn )
{
	if(lightIndex >= min_(SceneState.MAX_LIGTHTS, (u32)Driver->Caps.MaxActiveLights))
		return;

	if (ResetRenderStates || SceneState.LightsOn[lightIndex] != turnOn)
	{
		pID3DDevice->LightEnable(lightIndex, turnOn ? TRUE : FALSE);

		SceneState.LightsOn[lightIndex] = turnOn;
	}
}

void CD3D9SceneStateServices::setAmbientLight( SColor color )
{
	DEVICE_SET_RENDER_STATE(Ambient, D3DRS_AMBIENT, color.color);
	SceneState.AmbientLightColor = color;
}

void CD3D9SceneStateServices::setFog( SFogParam fogParam )
{
	DEVICE_SET_RENDER_STATE(FogColor, D3DRS_FOGCOLOR, fogParam.FogColor.color);

	DWORD fogmode = D3DFOG_NONE;
	switch(fogParam.FogType)
	{
	case EFT_FOG_LINEAR:
		fogmode = D3DFOG_LINEAR;
		break;
	case EFT_FOG_EXP:
		fogmode = D3DFOG_EXP;
		break;
	case EFT_FOG_EXP2:
		fogmode = D3DFOG_EXP2;
		break;
	case  EFT_FOG_NONE:
		fogmode = D3DFOG_NONE;
		break;
	}

	if (fogParam.PixelFog)
	{
		DEVICE_SET_RENDER_STATE(FogTableMode, D3DRS_FOGTABLEMODE, fogmode);
	}
	else
	{
		DEVICE_SET_RENDER_STATE(FogVertexMode, D3DRS_FOGVERTEXMODE, fogmode);
	}

	if (fogParam.FogType==EFT_FOG_LINEAR)
	{
		DEVICE_SET_RENDER_STATE(FogStart, D3DRS_FOGSTART, *(DWORD*)(&fogParam.FogStart));

		DEVICE_SET_RENDER_STATE(FogEnd, D3DRS_FOGEND, *(DWORD*)(&fogParam.FogEnd));
	}
	else
	{
		DEVICE_SET_RENDER_STATE(FogDensity, D3DRS_FOGDENSITY, *(DWORD*)(&fogParam.FogDensity));
	}

	DEVICE_SET_RENDER_STATE(RangeFogEnable, D3DRS_RANGEFOGENABLE, (fogParam.RangeFog ? TRUE : FALSE));

	SceneState.FogParam = fogParam;
}

bool CD3D9SceneStateServices::setClipPlane( u32 index, const plane3df& plane )
{
	if (index >= min_((u32)Driver->Caps.MaxUserClipPlanes, SSceneState::MAX_CLIPPLANES))
	{
		_ASSERT(false);
		return false;
	}

	HRESULT hr = S_OK;
	if (ResetRenderStates || SceneState.clipplanes[index] != plane)
	{
		hr = pID3DDevice->SetClipPlane(index, (const float*)&plane);
		if (SUCCEEDED(hr))
			SceneState.clipplanes[index] = plane;
	}
	return SUCCEEDED(hr);
}

void CD3D9SceneStateServices::enableClipPlane( u32 index, bool enable )
{
	if (index >= min_((u32)Driver->Caps.MaxUserClipPlanes, SSceneState::MAX_CLIPPLANES))
	{
		_ASSERT(false);
		return;
	}

	if (ResetRenderStates || SceneState.enableclips[index] != enable)
	{
		DWORD renderstate;
		pID3DDevice->GetRenderState(D3DRS_CLIPPLANEENABLE, &renderstate);
		if (enable)
			renderstate |= (1 << index);
		else
			renderstate &= ~(1 << index);

		DEVICE_SET_RENDER_STATE(ClipPlaneEnable, D3DRS_CLIPPLANEENABLE, renderstate);

		SceneState.enableclips[index] = enable;
	}
}

void CD3D9SceneStateServices::resetSceneStateCache()
{
	pID3DDevice->GetRenderState(D3DRS_AMBIENT, &SSCache.Ambient);
	pID3DDevice->GetRenderState(D3DRS_FOGCOLOR, &SSCache.FogColor);
	pID3DDevice->GetRenderState(D3DRS_FOGTABLEMODE, &SSCache.FogTableMode);
	pID3DDevice->GetRenderState(D3DRS_FOGVERTEXMODE, &SSCache.FogVertexMode);
	pID3DDevice->GetRenderState(D3DRS_FOGSTART, &SSCache.FogStart);
	pID3DDevice->GetRenderState(D3DRS_FOGEND, &SSCache.FogEnd);
	pID3DDevice->GetRenderState(D3DRS_FOGDENSITY, &SSCache.FogDensity);
	pID3DDevice->GetRenderState(D3DRS_RANGEFOGENABLE, &SSCache.RangeFogEnable);
	pID3DDevice->GetRenderState(D3DRS_CLIPPLANEENABLE, &SSCache.ClipPlaneEnable);
}

bool CD3D9SceneStateServices::reset()
{
	ResetRenderStates = true;

	resetSceneStateCache();

	//restore scene states
	setAmbientLight(SceneState.AmbientLightColor);
	for (u32 i=0; i<SSceneState::MAX_LIGTHTS; ++i)
	{
		setDynamicLight(i, SceneState.Lights[i]);
		turnLightOn(i, SceneState.LightsOn[i]);
	}
	setFog(SceneState.FogParam);
	for (u32 i=0; i<SSceneState::MAX_CLIPPLANES; ++i)
	{
		setClipPlane(i, SceneState.clipplanes[i]);
		enableClipPlane(i, SceneState.enableclips[i]);
	}
	
	ResetRenderStates = false;

	return true;
}
