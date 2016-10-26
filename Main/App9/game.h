#pragma once

#include "mywow.h"

extern bool g_bExit;
extern bool g_bBackMode;
extern IFileWDT* fileWDT;
extern IMapTileSceneNode* g_mapTileSceneNode;

class MyMessageHandler : public IMessageHandler
{
public:
	virtual void onExit(window_type hwnd) { g_bExit = true; }
	virtual void onSize(window_type hwnd, int width, int height);
};

void createScene();
void destroyScene();

void createInput();
void destroyInput();
void processInput();

void idleTick();