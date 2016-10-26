#include "stdafx.h"
#include "wow_m2spell.h"
#include "mywow.h"
#include "CM2SceneNode.h"
#include "wow_m2instance.h"

wow_m2spell::wow_m2spell( IM2SceneNode* node )
{
	M2SceneNode = node;

	CurrentId = 0;

	memset(&DynSpell, 0, sizeof(DynSpell));
}

bool wow_m2spell::takeSpellVisualKit( u32 id, u32 time )
{
	wowDatabase* database = g_Engine->getWowDatabase();
	const spellVisualKitDB* spellvisualkitdb = database->getSpellVisualKitDB();
	dbc::record r = spellvisualkitdb->getByID(id);
	if (!r.isValid())
		return false;

	CurrentId = id;
	DynSpell.time = time;
	DynSpell.currentTime = 0;

	const c8* animName = database->getAnimationName(r.getUInt(spellVisualKitDB::CastAnimation));
	E_M2_STATES state = getM2State(animName);
	if (state != EMS_INVALID)
	{
		M2SceneNode->getM2FSM()->changeState(state);
		DynSpell.spellAnimation = true;
	}

	M2SceneNode->setSpellVisualKit(id);

	//play animation
	for (u32 i=0; i<(u32)SVK_COUNT; ++i)
	{
		if (DynSpell.nodes[i])
			DynSpell.nodes[i]->playAnimationByName("Stand", 0, true);
	}

	return true;
}

void wow_m2spell::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	if (CurrentId != 0)
	{
		DynSpell.currentTime += timeSinceLastFrame;
		if (DynSpell.currentTime > DynSpell.time)
		{
			if(DynSpell.spellAnimation)
				M2SceneNode->getM2FSM()->changeState(EMS_STAND);

			M2SceneNode->setSpellVisualKit(0);
			CurrentId = 0;
		}
	}
}
