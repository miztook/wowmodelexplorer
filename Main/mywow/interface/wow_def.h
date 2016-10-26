//
//  wow_base.h
//  mywow
//
//  Created by miao yu on 6/9/14.
//  Copyright (c) 2014 MW. All rights reserved.
//

#pragma once

#include "base.h"
#include "wow_enums.h"

struct SItemInfo
{
	s32 id;
	s32 itemType;
	s32 sheathtype;
};

struct SAttachmentInfo
{
	SAttachmentInfo()
	{
		::memset(modelpath, 0, QMAX_PATH);
		::memset(texpath, 0, QMAX_PATH);
	}
	c8		modelpath[QMAX_PATH];
	c8		texpath[QMAX_PATH];
};

struct SAttachmentEntry
{
	SAttachmentEntry() :  scale(1.0f), node(NULL_PTR), id(-1), slot(-1), attachIndex(-1) { }
    
	f32		scale;
	void*		node;
    
	s16		id;
	s16		slot;
	s16		attachIndex;
};

struct SSpellVKInfo
{
	SAttachmentEntry	attachmentEntries[SVK_COUNT];
	SAttachmentInfo		attachmentInfos[SVK_COUNT];
};