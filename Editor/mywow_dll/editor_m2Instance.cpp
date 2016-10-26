#include "stdafx.h"
#include "editor_m2Instance.h"

void m2Instance_setCharFeature( wow_m2instance* instance, const SCharFeature& feature )
{
	if (!instance->CharacterInfo)
		return;

	if (instance->CharacterInfo->SkinColor == feature.skinColor &&
		instance->CharacterInfo->FaceType == feature.faceType &&
		instance->CharacterInfo->HairColor == feature.hairColor &&
		instance->CharacterInfo->HairStyle == feature.hairStyle &&
		instance->CharacterInfo->FacialHair == feature.facialHair)
		return;

	instance->CharacterInfo->SkinColor = feature.skinColor;
	instance->CharacterInfo->FaceType = feature.faceType;
	instance->CharacterInfo->HairColor = feature.hairColor;
	instance->CharacterInfo->HairStyle = feature.hairStyle;
	instance->CharacterInfo->FacialHair = feature.facialHair;
}

bool m2Instance_getCharFeature( wow_m2instance* instance, SCharFeature* feature )
{
	if (!instance->CharacterInfo)
		return false;

	feature->skinColor = instance->CharacterInfo->SkinColor;
	feature->faceType = instance->CharacterInfo->FaceType;
	feature->hairColor = instance->CharacterInfo->HairColor;
	feature->hairStyle = instance->CharacterInfo->HairStyle;
	feature->facialHair = instance->CharacterInfo->FacialHair;

	return true;
}

s32 m2Instance_getItemSlot( wow_m2instance* instance, s32 itemid )
{
	return instance->getItemSlot(itemid);
}

s32 m2Instance_getSlotItemId( wow_m2instance* instance, s32 slot )
{
	if (!instance->CharacterInfo)
		return -1;
	return instance->CharacterInfo->Equipments[slot];
}

bool m2Instance_slotHasModel( wow_m2instance* instance, s32 slot )
{
	return instance->slotHasModel(slot);
}
