#include "StdAfx.h"
#include "mywowtool.h"
#include "wow_armory.h"

mywowTool* g_mwTool = NULL;

void createTool()
{
	g_mwTool = new mywowTool;	
}

void destroyTool()
{
	delete g_mwTool;
}

mywowTool::mywowTool()
{
	WowArmory = new wow_armory;
}

mywowTool::~mywowTool()
{
	delete WowArmory;
}
