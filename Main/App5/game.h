#pragma once

#include "mywow.h"

extern bool g_bExit;
extern bool g_bBackMode;
extern IM2SceneNode* g_DeathWingSceneNode;

class MyMessageHandler : public IMessageHandler
{
public:
	virtual void onExit(window_type hwnd) { g_bExit = true; }
	virtual void onSize(window_type hwnd, int width, int height);
};

void createScene();
void destroyScene();
void processResource();

void createInput();
void destroyInput();
void processInput();

void idleTick();