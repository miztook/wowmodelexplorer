#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "ISceneStateServices.h"

class COpenGLDriver;
class COpenGLExtension;

class COpenGLSceneStateServices : public ISceneStateServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLSceneStateServices);

public:
	COpenGLSceneStateServices();
	~COpenGLSceneStateServices();

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
		GLfloat Ambient[4];
		GLfloat FogColor[4];
		GLint FogMode;
		GLfloat FogStart;
		GLfloat FogEnd;
		GLfloat FogDensity;
		GLint	RangeFogEnable;
		GLint	PixelFogEnable;
		GLint	ClipPlaneEnable[SSceneState::MAX_CLIPPLANES];
		GLdouble	Planes[SSceneState::MAX_CLIPPLANES][4];
	};

private:
	COpenGLDriver*	Driver;
	COpenGLExtension*	Extension;

	SSceneStateCache	SSCache;
	SSceneState		SceneState;

	u32		MaxLights;
	u32		MaxClipPlanes;
	bool		ResetRenderStates;
};

#endif