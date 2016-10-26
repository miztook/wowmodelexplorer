#include "stdafx.h"
#include "COpenGLSceneStateServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLHelper.h"

#define  DEVICE_SET_BOOL_STATE(prop, d3drs, v)	if (SSCache.prop != (v))		\
		{	v != GL_FALSE ? glEnable(d3drs) : glDisable(d3drs);				\
		SSCache.prop = (v);	}

#define  DEVICE_SET_FOG_INT_STATE(prop, d3drs, v)	 if (SSCache.prop != (v))		\
		{	glFogi(d3drs, v);				\
		SSCache.prop = (v);	}

#define  DEVICE_SET_FOG_FLOAT_STATE(prop, d3drs, v)	if (SSCache.prop != (v))		\
		{	glFogf(d3drs, v);			\
		SSCache.prop = (v);	}	

#define  DEVICE_SET_HINT_STATE(prop, d3drs, v)	if (SSCache.prop != (v))		\
		{	glHint(d3drs, v);			\
		SSCache.prop = (v);	}



COpenGLSceneStateServices::COpenGLSceneStateServices()
{
	Driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	Extension = Driver->getGLExtension();

	MaxLights = min_((u32)Extension->MaxLights, SSceneState::MAX_LIGTHTS);
	MaxClipPlanes = min_((u32)Extension->MaxUserClipPlanes, SSceneState::MAX_CLIPPLANES);

	resetSceneStateCache();

	ResetRenderStates = true;

	setAmbientLight(SColor(255,255,255));
	setFog(SFogParam());
	deleteAllDynamicLights();
}

COpenGLSceneStateServices::~COpenGLSceneStateServices()
{

}

void COpenGLSceneStateServices::setAmbientLight( SColor color )
{
#ifdef FIXPIPELINE
	GLfloat v[4];
	COpenGLHelper::colorfToGLfloat4(color, v);
	if (memcmp(v, SSCache.Ambient, sizeof(GLfloat) * 4) != 0)
	{
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, v);
		ASSERT_OPENGL_SUCCESS();

		Q_memcpy(SSCache.Ambient, sizeof(GLfloat) * 4, v, sizeof(GLfloat) * 4);
	}
#endif

	SceneState.AmbientLightColor = color;
}

void COpenGLSceneStateServices::deleteAllDynamicLights()
{
#ifdef FIXPIPELINE
	for (u32 i=0; i<MaxLights; ++i)
	{
		if (ResetRenderStates || SceneState.LightsOn[i] != false)
		{
			glDisable(GL_LIGHT0 + i);
			ASSERT_OPENGL_SUCCESS();
		}
	}
#endif

	SceneState.resetLights();
}

bool COpenGLSceneStateServices::setDynamicLight( u32 index, const SLight& light )
{
	if (index >= MaxLights)
		return false;

#ifdef FIXPIPELINE

	GLenum lidx = GL_LIGHT0 + index;

	GLfloat data[4];
	switch (light.Type)
	{
	case ELT_POINT:
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
		break;
	case ELT_SPOT:
		data[0] = light.Direction.X;
		data[1] = light.Direction.Y;
		data[2] = light.Direction.Z;
		data[3] = 0.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, data);

		// set position
		data[0] = light.Position.X;
		data[1] = light.Position.Y;
		data[2] = light.Position.Z;
		data[3] = 1.0f; // 1.0f for positional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, light.Falloff);
		glLightf(lidx, GL_SPOT_CUTOFF, light.OuterCone);
		break;
	case ELT_DIRECTIONAL:
		// set direction
		data[0] = -light.Direction.X;
		data[1] = -light.Direction.Y;
		data[2] = -light.Direction.Z;
		data[3] = 0.0f; // 0.0f for directional light
		glLightfv(lidx, GL_POSITION, data);

		glLightf(lidx, GL_SPOT_EXPONENT, 0.0f);
		glLightf(lidx, GL_SPOT_CUTOFF, 180.0f);
		break;
	default:
		return false;
	}

	// set diffuse color
	COpenGLHelper::colorToGLfloat4(light.DiffuseColor, data);
	glLightfv(lidx, GL_DIFFUSE, data);

	// set specular color
	COpenGLHelper::colorToGLfloat4(light.SpecularColor, data);
	glLightfv(lidx, GL_SPECULAR, data);

	// set ambient color
	COpenGLHelper::colorToGLfloat4(light.AmbientColor, data);
	glLightfv(lidx, GL_AMBIENT, data);

	// 1.0f / (constant + linear * d + quadratic*(d*d);

	// set attenuation
	glLightf(lidx, GL_CONSTANT_ATTENUATION, light.Attenuation.X);
	glLightf(lidx, GL_LINEAR_ATTENUATION, light.Attenuation.Y);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, light.Attenuation.Z);

#endif

	SceneState.Lights[index] = light;
	return true;
}

const SLight* COpenGLSceneStateServices::getDynamicLight( u32 index ) const
{
	if(index >= MaxLights)
		return NULL_PTR;

	return &SceneState.Lights[index];
}

void COpenGLSceneStateServices::turnLightOn( u32 lightIndex, bool turnOn )
{
	if(lightIndex >= MaxLights)
		return;

	if (ResetRenderStates || SceneState.LightsOn[lightIndex] != turnOn)
	{
#ifdef FIXPIPELINE
		if (turnOn)
			glEnable(GL_LIGHT0 + lightIndex);
		else
			glDisable(GL_LIGHT0 + lightIndex);
#endif
		SceneState.LightsOn[lightIndex] = turnOn;
	}
}

void COpenGLSceneStateServices::setFog( const SFogParam& fogParam )
{
#ifdef FIXPIPELINE
	GLfloat data[4];
	COpenGLHelper::colorToGLfloat4(fogParam.FogColor, data);
	if (memcmp(data, SSCache.FogColor, sizeof(GLfloat) * 4) != 0)
	{
		glFogfv(GL_FOG_COLOR, data);
		Q_memcpy(SSCache.FogColor, sizeof(GLfloat) * 4, data, sizeof(GLfloat) * 4);
	}

	if (Extension->queryOpenGLFeature(IRR_EXT_fog_coord))
		glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);

	GLint fogmode = GL_NONE;
	switch(fogParam.FogType)
	{
	case EFT_FOG_LINEAR:
		fogmode = GL_LINEAR;
		break;
	case EFT_FOG_EXP:
		fogmode = GL_EXP;
		break;
	case EFT_FOG_EXP2:
		fogmode = GL_EXP2;
		break;
	case  EFT_FOG_NONE:
		fogmode = GL_NONE;
		break;
	}

	DEVICE_SET_FOG_INT_STATE(FogMode, GL_FOG_MODE, fogmode);

	if (Extension->queryOpenGLFeature(IRR_NV_fog_distance))
	{
		if (fogParam.RangeFog)
		{
			DEVICE_SET_FOG_INT_STATE(RangeFogEnable, GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);
		}
		else
		{
			DEVICE_SET_FOG_INT_STATE(RangeFogEnable, GL_FOG_DISTANCE_MODE_NV, GL_EYE_PLANE_ABSOLUTE_NV);
		}
	}

	if (fogParam.PixelFog)
	{
		DEVICE_SET_HINT_STATE(PixelFogEnable, GL_FOG_HINT, GL_NICEST);
	}
	else
	{
		DEVICE_SET_HINT_STATE(PixelFogEnable, GL_FOG_HINT, GL_FASTEST);
	}

	if (fogParam.FogType==EFT_FOG_LINEAR)
	{
		DEVICE_SET_FOG_FLOAT_STATE(FogStart, GL_FOG_START, fogParam.FogStart);

		DEVICE_SET_FOG_FLOAT_STATE(FogEnd, GL_FOG_END, fogParam.FogEnd);
	}
	else
	{
		DEVICE_SET_FOG_FLOAT_STATE(FogDensity, GL_FOG_DENSITY, fogParam.FogDensity);
	}

#endif

	SceneState.FogParam = fogParam;
}

bool COpenGLSceneStateServices::setClipPlane( u32 index, const plane3df& plane )
{
	if (index >= MaxClipPlanes)
	{
		ASSERT(false);
		return false;
	}

	if (ResetRenderStates || SceneState.clipplanes[index] != plane)
	{
		GLdouble clip_plane[4];
		clip_plane[0] = plane.Normal.X;
		clip_plane[1] = plane.Normal.Y;
		clip_plane[2] = plane.Normal.Z;
		clip_plane[3] = plane.D;

		if (memcmp(SSCache.Planes[index], clip_plane, sizeof(GLdouble) * 4) != 0)
		{
#ifdef FIXPIPELINE
			glClipPlane(GL_CLIP_PLANE0 + index, clip_plane);
			ASSERT_OPENGL_SUCCESS();
#endif
			Q_memcpy(SSCache.Planes[index], sizeof(GLdouble) * 4, clip_plane, sizeof(GLdouble) * 4);
		}
		
		SceneState.clipplanes[index] = plane;
	}
	return true;
}

void COpenGLSceneStateServices::enableClipPlane( u32 index, bool enable )
{
	if (index >= MaxClipPlanes)
	{
		ASSERT(false);
		return;
	}

	if (ResetRenderStates || SceneState.enableclips[index] != enable)
	{
		GLint v = enable ? GL_TRUE : GL_FALSE;
		DEVICE_SET_BOOL_STATE(ClipPlaneEnable[index], GL_CLIP_PLANE0 + index, v);
		SceneState.enableclips[index] = enable;
	}
}

bool COpenGLSceneStateServices::isClipPlaneEnable( u32 index ) const
{
	if (index >= MaxClipPlanes)
	{
		ASSERT(false);
		return false;
	}

	return SceneState.enableclips[index];
}

bool COpenGLSceneStateServices::getClipPlane( u32 index, plane3df& plane ) const
{
	if (index >= MaxClipPlanes)
	{
		ASSERT(false);
		return false;
	}

	plane = SceneState.clipplanes[index];
	return true;
}

bool COpenGLSceneStateServices::reset()
{
	ResetRenderStates = true;

	SSceneState lastSceneState = SceneState;

	resetSceneStateCache();

	//restore scene states
	setAmbientLight(lastSceneState.AmbientLightColor);
	for (u32 i=0; i<SSceneState::MAX_LIGTHTS; ++i)
	{
		setDynamicLight(i, lastSceneState.Lights[i]);
		turnLightOn(i, lastSceneState.LightsOn[i]);
	}
	setFog(lastSceneState.FogParam);
	for (u32 i=0; i<SSceneState::MAX_CLIPPLANES; ++i)
	{
		setClipPlane(i, lastSceneState.clipplanes[i]);
		enableClipPlane(i, lastSceneState.enableclips[i]);
	}

	ResetRenderStates = false;

	return true;
}

void COpenGLSceneStateServices::resetSceneStateCache()
{
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, SSCache.Ambient);

#ifdef FIXPIPELINE
	glGetFloatv(GL_FOG_COLOR, SSCache.FogColor);
	glGetIntegerv(GL_FOG_MODE, &SSCache.FogMode);
	glGetFloatv(GL_FOG_START, &SSCache.FogStart);
	glGetFloatv(GL_FOG_END, &SSCache.FogEnd);
	glGetFloatv(GL_FOG_DENSITY, &SSCache.FogDensity);
	glGetIntegerv(GL_FOG_DISTANCE_MODE_NV, &SSCache.RangeFogEnable);
	glGetIntegerv(GL_FOG_HINT, &SSCache.PixelFogEnable);
#endif

	for (u32 i=0; i<MaxClipPlanes; ++i)
	{
		glGetIntegerv(GL_CLIP_PLANE0 + i, &SSCache.ClipPlaneEnable[i]);
		glGetClipPlane(GL_CLIP_PLANE0 + i, (GLdouble*)&SSCache.Planes[i]);
	}
}

#endif

