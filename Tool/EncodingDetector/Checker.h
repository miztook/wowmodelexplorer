#pragma once

#include "base.h"

void checkEncodingScript();

void checkEncodingInterface();

void checkEncodingSurfaces();

void checkImageFileSize();

void checkScript();

void funcCheckEncodingScript(const c8* filename, void* args);

void funcCheckEncodingInterface(const c8* filename, void* args);

void funcCheckEncodingSurfaces(const c8* filename, void* args);

void funcCheckImageSize(const c8* filename, void* args);

void funcCheckScript(const c8* filename, void* args);