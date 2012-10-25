#pragma once

#include "ISceneStateServices.h"

class CD3D9Driver;

class CD3D9SceneStateServices : public ISceneStateServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9SceneStateServices);

public:
	CD3D9SceneStateServices();
	~CD3D9SceneStateServices();

public:
	virtual SColor getAmbientLight() const { return SceneState.AmbientLightColor; }
	virtual void setAmbientLight( SColor color );
	virtual void deleteAllDynamicLights();
	virtual bool setDynamicLight( u32 index, const SLight& light );
	virtual u32 getDynamicLightCount() const  { return SceneState.LightCount; }
	virtual const SLight& getDynamicLight(u32 index) const { return SceneState.Lights[index]; }
	virtual void turnLightOn(u32 lightIndex, bool turnOn);

	virtual void setFog(SFogParam fogParam);
	virtual SFogParam getFog() const { return SceneState.FogParam; }

	virtual bool setClipPlane( u32 index, const plane3df& plane );
	virtual void enableClipPlane( u32 index, bool enable );

	virtual bool reset();

private:
	void resetSceneStateCache();
	
private:
	CD3D9Driver*	Driver;
	IDirect3DDevice9*	pID3DDevice;

	struct SSceneStateCache			//SceneStateµÄ»º´æ
	{
		DWORD		Ambient;
		DWORD		FogColor;
		DWORD		FogTableMode;
		DWORD		FogVertexMode;
		DWORD		FogStart;
		DWORD		FogEnd;
		DWORD		FogDensity;
		DWORD		RangeFogEnable;
		DWORD		ClipPlaneEnable;
	};
	SSceneStateCache	SSCache;

	SSceneState		SceneState;

	bool		ResetRenderStates;
};