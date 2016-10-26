#include "stdafx.h"
#include "editor_SceneEnvironment.h"

void SceneEnvironment_setLightDir( vector3df dir )
{
	g_Engine->getSceneEnvironment()->LightDir = dir;
}

void SceneEnvrionment_getLightDir( vector3df* dir )
{
	*dir = g_Engine->getSceneEnvironment()->LightDir;
}