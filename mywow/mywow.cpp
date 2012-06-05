#include "StdAfx.h"
#include "mywow.h"

void createEngine()
{
	g_Engine = new Engine;
}

void destroyEngine()
{
	delete g_Engine;
}