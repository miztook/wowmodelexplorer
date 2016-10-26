#pragma once

#include "editor_base.h"

MW_API void m2Move_getDir(wow_m2Move* move, vector3df* dir);
MW_API void m2Move_setDir(wow_m2Move* move, const vector3df& dir);
MW_API void m2Move_getPos(wow_m2Move* move, vector3df* pos);
MW_API void m2Move_setPos(wow_m2Move* move, const vector3df& pos);
MW_API void m2Move_getScale(wow_m2Move* move, vector3df* scale);
MW_API void m2Move_setScale(wow_m2Move* move, const vector3df& scale);