#pragma once

#pragma comment(lib, "stormlib.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "q_memory.h"
#include "core.h"
#include "io.h"
#include "video.h"
#include "wow.h"
#include "scene.h"
#include "renderer.h"

#include "engine.h"

void createEngine();

void destroyEngine();
