#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

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
	virtual void setLight_Fog(u32 lightIndex, bool enable, const SLight& dirLight, 
		SColor color, const SFogParam& fogParam)
	{
		turnLightOn(lightIndex, enable);
		setDynamicLight(lightIndex, dirLight);
		setAmbientLight(color);
		setFog(fogParam);
	}

	virtual void setLight(u32 lightIndex, bool enable, const SLight& dirLight, SColor color)
	{
		turnLightOn(lightIndex, enable);
		setDynamicLight(lightIndex, dirLight);
		setAmbientLight(color);
	}

public:
	bool setClipPlane( u32 index, const plane3df& plane );
	void enableClipPlane( u32 index, bool enable );
	SColor getAmbientLight() const { return SceneState.AmbientLightColor; }
	void setAmbientLight( SColor color );
	void deleteAllDynamicLights();
	bool setDynamicLight( u32 index, const SLight& light );
	const SLight* getDynamicLight(u32 index) const;
	void turnLightOn(u32 lightIndex, bool turnOn);

	void setFog(const SFogParam& fogParam);
	const SFogParam& getFog() const { return SceneState.FogParam; }

	bool isClipPlaneEnable( u32 index ) const;
	bool getClipPlane( u32 index, plane3df& plane ) const;
	bool reset();

private:
	void resetSceneStateCache();
	
private:
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
		
		plane3df	Planes[SSceneState::MAX_CLIPPLANES];
	};

private:
	CD3D9Driver*	Driver;
	IDirect3DDevice9*	pID3DDevice;
	
	SSceneStateCache	SSCache;
	SSceneState		SceneState;

	bool		ResetRenderStates;
};

#endif