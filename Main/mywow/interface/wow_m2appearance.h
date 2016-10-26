#pragma once

#include "core.h"
#include "SColor.h"

class IM2SceneNode;
class wow_m2instance;

//IM2SceneNode的外观部分
class wow_m2appearance
{
public:
	explicit wow_m2appearance(IM2SceneNode* node);

public:
	void onAnimationEnd();

	bool takeItem(s32 itemid, bool sheath = false);
	void loadStartOutfit(s32 startid, bool deathknight, bool sheath = false);
	void loadSet(s32 setid, bool sheath = false);

	void sheathLeftWeapon(bool sheath);
	void sheathRightWeapon(bool sheath);
	bool isLeftWeaponSheathed() const;
	bool isRightWeaponSheathed() const;

	void showHelm(bool show);
	void showCape(bool show);

	bool isShowHelm() const;
	bool isShowCape() const;

private:
	void takeSlotModelItem(s32 slot, s32 itemid, bool sheath);

private:
	IM2SceneNode*			M2SceneNode;
	wow_m2instance*		M2Instance;

public:
	IM2SceneNode*		HelmNode;
	bool HideHelmHair;
	bool HideHelmFacial1;
	bool HideHelmFacial2;
	bool HideHelmFacial3;
};