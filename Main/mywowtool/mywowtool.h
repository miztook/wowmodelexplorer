#pragma once

#ifdef AUTOMATIC_LINK

#pragma comment(lib, "jsoncpp.lib") 
#pragma comment(lib, "libfbxsdk-mt.lib")

#endif

#include "wow_armory.h"
#include "wow_fbxExporter.h"

void createTool();
void destroyTool();

#include "base.h"

class wow_armory;

class mywowTool
{
private:
	DISALLOW_COPY_AND_ASSIGN(mywowTool);

public:
	mywowTool();
	~mywowTool();

public:
	wow_armory* getWowArmory() const { return WowArmory; }

private:
	wow_armory*		WowArmory;
};

extern mywowTool* g_mwTool;