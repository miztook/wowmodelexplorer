#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API bool m2Appearance_takeItem(wow_m2appearance* appearance, s32 itemId);
MW_API void m2Appearance_loadStartOutfit(wow_m2appearance* appearance, s32 startId, bool deathknight);
MW_API void m2Appearance_loadSet(wow_m2appearance* appearance, s32 setid);

MW_API void m2Appearance_sheathLeftWeapon(wow_m2appearance* appearance, bool sheath);
MW_API void m2Appearance_sheathRightWeapon(wow_m2appearance* appearance, bool sheath);
MW_API bool m2Appearance_isLeftWeaponSheathed(wow_m2appearance* appearance);
MW_API bool m2Appearance_isRightWeaponSheathed(wow_m2appearance* appearance);