#include "stdafx.h"
#include "editor_m2Appearance.h"

bool m2Appearance_takeItem( wow_m2appearance* appearance, s32 itemId )
{
	return appearance->takeItem(itemId);
}

void m2Appearance_loadStartOutfit( wow_m2appearance* appearance, s32 startId, bool deathknight )
{
	appearance->loadStartOutfit(startId, deathknight);
}

void m2Appearance_loadSet( wow_m2appearance* appearance, s32 setid )
{
	appearance->loadSet(setid);
}

void m2Appearance_sheathLeftWeapon( wow_m2appearance* appearance, bool sheath )
{
	appearance->sheathLeftWeapon(sheath);
}

void m2Appearance_sheathRightWeapon( wow_m2appearance* appearance, bool sheath )
{
	appearance->sheathRightWeapon(sheath);
}

bool m2Appearance_isLeftWeaponSheathed( wow_m2appearance* appearance )
{
	return appearance->isLeftWeaponSheathed();
}

bool m2Appearance_isRightWeaponSheathed( wow_m2appearance* appearance )
{
	return appearance->isRightWeaponSheathed();
}
