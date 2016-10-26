#include "stdafx.h"
#include "wow_m2appearance.h"
#include "mywow.h"
#include "CM2SceneNode.h"
#include "wow_m2instance.h"

wow_m2appearance::wow_m2appearance( IM2SceneNode* node )
	: HideHelmHair(false), HideHelmFacial1(false), HideHelmFacial2(false), HideHelmFacial3(false), HelmNode(NULL_PTR)
{
	M2SceneNode = node;
	M2Instance = node->getM2Instance();

	ASSERT(M2Instance->CharacterInfo);
}

void wow_m2appearance::onAnimationEnd()
{
	if (HelmNode)
	{
 		if (rand_() % 3 == 0)
 			static_cast<CM2SceneNode*>(HelmNode)->AnimateColors = false;
 		else 
			static_cast<CM2SceneNode*>(HelmNode)->AnimateColors = true;
	}
}

bool wow_m2appearance::takeItem( s32 itemid, bool sheath )
{
	s32 slot = M2Instance->getItemSlot(itemid);

	if (slot == -1)
		return false;

	if (M2Instance->isOrdinaryDagger(itemid))		//¿É×óÓÒÌæ»»
	{
		if (M2Instance->getSlotItemId(CS_HAND_RIGHT) != itemid)
			slot = CS_HAND_RIGHT;
		else
			slot = CS_HAND_LEFT;
	}
	
	M2Instance->updateEquipments(slot, itemid);
	takeSlotModelItem(slot, itemid, sheath);

	return true;
}


void wow_m2appearance::takeSlotModelItem( s32 slot, s32 itemid, bool sheath )
{
	if (M2Instance->slotHasModel(slot))
	{
		M2SceneNode->setM2ModelEquipment(slot, itemid, sheath);

		//dagger
		if (M2Instance->isBothHandsDagger(itemid))
		{
			M2Instance->updateEquipments(CS_HAND_LEFT, itemid);
			M2SceneNode->setM2ModelEquipment(CS_HAND_LEFT, itemid, sheath);
		}
	}
}

void wow_m2appearance::loadStartOutfit( s32 startid, bool deathknight, bool sheath )
{
	if (M2SceneNode->isNpc())
		return;

	M2Instance->dressStartOutfit(startid);

	for (u32 slot=0; slot<NUM_CHAR_SLOTS; ++slot)
	{
		s32 id = M2Instance->CharacterInfo->Equipments[slot];
		takeSlotModelItem(slot, id, sheath);
	}

	M2Instance->CharacterInfo->DeathKnight = deathknight;
}

void wow_m2appearance::loadSet( s32 setid, bool sheath )
{
	if (M2SceneNode->isNpc())
		return;

	M2Instance->dressSet(setid);
	for (u32 slot=0; slot<NUM_CHAR_SLOTS; ++slot)
	{
		s32 id = M2Instance->CharacterInfo->Equipments[slot];
		takeSlotModelItem(slot, id, sheath);
	}
}

void wow_m2appearance::sheathLeftWeapon( bool sheath )
{
	if (!M2Instance->CharacterInfo)
		return;

	CM2SceneNode* m2SceneNode = static_cast<CM2SceneNode*>(M2SceneNode);

	for (CM2SceneNode::T_AttachmentList::iterator itr = m2SceneNode->AttachmentList.begin(); itr != m2SceneNode->AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_HAND_LEFT)
		{		
			M2Instance->UseAttachments[entry->attachIndex] -= 1;
		}
	}

	s32 leftItemId = M2Instance->CharacterInfo->Equipments[CS_HAND_LEFT];
	if (leftItemId)
	{
		SAttachmentEntry entry1;
		SAttachmentInfo info1;
		SAttachmentEntry entry2;
		SAttachmentInfo info2;
		M2Instance->setM2Equipment(CS_HAND_LEFT, leftItemId, sheath, &entry1, &info1, &entry2, &info2);

		if (entry1.attachIndex != -1)
		{
			for (CM2SceneNode::T_AttachmentList::iterator itr = m2SceneNode->AttachmentList.begin(); itr != m2SceneNode->AttachmentList.end(); ++itr)
			{
				SAttachmentEntry* entry = &(*itr);

				if (entry->slot == CS_HAND_LEFT)
				{
					entry->id = entry1.id;
					entry->attachIndex = entry1.attachIndex;
					M2Instance->UseAttachments[entry->attachIndex] += 1;
					m2SceneNode->updateAttachmentEntry(entry);

					break;
				}
			}
		}

		m2SceneNode->updateLeftCloseHand(isLeftWeaponSheathed());
	}
}

void wow_m2appearance::sheathRightWeapon( bool sheath )
{
	if (!M2Instance->CharacterInfo)
		return;

	CM2SceneNode* m2SceneNode = static_cast<CM2SceneNode*>(M2SceneNode);

	for (CM2SceneNode::T_AttachmentList::iterator itr = m2SceneNode->AttachmentList.begin(); itr != m2SceneNode->AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_HAND_RIGHT)
		{		
			M2Instance->UseAttachments[entry->attachIndex] -= 1;
		}
	}

	s32 rightItemId = M2Instance->CharacterInfo->Equipments[CS_HAND_RIGHT];
	if (rightItemId)
	{
		SAttachmentEntry entry1;
		SAttachmentInfo info1;
		SAttachmentEntry entry2;
		SAttachmentInfo info2;
		M2Instance->setM2Equipment(CS_HAND_RIGHT, rightItemId, sheath, &entry1, &info1, &entry2, &info2);

		if (entry1.attachIndex != -1)
		{
			for (CM2SceneNode::T_AttachmentList::iterator itr = m2SceneNode->AttachmentList.begin(); itr != m2SceneNode->AttachmentList.end(); ++itr)
			{
				SAttachmentEntry* entry = &(*itr);

				if (entry->slot == CS_HAND_RIGHT)
				{
					entry->id = entry1.id;
					entry->attachIndex = entry1.attachIndex;
					M2Instance->UseAttachments[entry->attachIndex] += 1;
					m2SceneNode->updateAttachmentEntry(entry);

					break;
				}
			}
		}

		m2SceneNode->updateRightCloseHand(isRightWeaponSheathed());
	}

}

bool wow_m2appearance::isLeftWeaponSheathed() const
{
	if (!M2Instance->CharacterInfo)
		return false;

	CM2SceneNode* m2SceneNode = static_cast<CM2SceneNode*>(M2SceneNode);

	for (CM2SceneNode::T_AttachmentList::iterator itr = m2SceneNode->AttachmentList.begin(); itr != m2SceneNode->AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_HAND_LEFT)
		{		
			return entry->id ==ATT_MIDDLE_BACK_SHEATH || entry->id == ATT_LEFT_BACK_SHEATH || entry->id == ATT_RIGHT_BACK || entry->id == ATT_RIGHT_HIP_SHEATH; 
		}
	}

	return false;
}

bool wow_m2appearance::isRightWeaponSheathed() const
{
	if (!M2Instance->CharacterInfo)
		return false;

	CM2SceneNode* m2SceneNode = static_cast<CM2SceneNode*>(M2SceneNode);

	for (CM2SceneNode::T_AttachmentList::iterator itr = m2SceneNode->AttachmentList.begin(); itr != m2SceneNode->AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_HAND_RIGHT)
		{
			return entry->id ==ATT_MIDDLE_BACK_SHEATH || entry->id == ATT_RIGHT_BACK_SHEATH || entry->id == ATT_LEFT_BACK || entry->id == ATT_LEFT_HIP_SHEATH; 
		}
	}

	return false;
}

void wow_m2appearance::showHelm( bool show )
{
	if (M2SceneNode->isNpc())
		return;

	if (HelmNode)
	{
		HelmNode->Visible = show;
		if (!show)
		{
			M2Instance->CharacterInfo->HelmHideHair = false;
			M2Instance->CharacterInfo->HelmHideFacial1 = false;
			M2Instance->CharacterInfo->HelmHideFacial2 = false;
			M2Instance->CharacterInfo->HelmHideFacial3 = false;
		}
		else 
		{
			M2Instance->CharacterInfo->HelmHideHair = HideHelmHair;
			M2Instance->CharacterInfo->HelmHideFacial1 = HideHelmFacial1;
			M2Instance->CharacterInfo->HelmHideFacial2 = HideHelmFacial2;
			M2Instance->CharacterInfo->HelmHideFacial3 = HideHelmFacial3;
		}
	}
}

void wow_m2appearance::showCape( bool show )
{
	if (M2SceneNode->isNpc())
		return;
	M2Instance->CharacterInfo->ShowCape = show;
}

bool wow_m2appearance::isShowHelm() const
{
	if (M2SceneNode->isNpc())
		return true;

	if (HelmNode)
		return HelmNode->Visible;
	else
		return false;
}

bool wow_m2appearance::isShowCape() const
{
	if (M2SceneNode->isNpc())
		return true;

	return M2Instance->CharacterInfo->ShowCape;
}

