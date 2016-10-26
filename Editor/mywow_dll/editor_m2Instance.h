#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void m2Instance_setCharFeature(wow_m2instance* inst, const SCharFeature& feature);
MW_API bool m2Instance_getCharFeature(wow_m2instance* inst, SCharFeature* feature);

MW_API s32 m2Instance_getItemSlot(wow_m2instance* inst, s32 itemid);
MW_API s32 m2Instance_getSlotItemId(wow_m2instance* inst, s32 slot);
MW_API bool m2Instance_slotHasModel(wow_m2instance* inst, s32 slot);
