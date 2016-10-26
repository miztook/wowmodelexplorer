#pragma once

#include "base.h"

#define STYLE_NORMAL	0x00000000
#define STYLE_BOLD		0x00000001
#define STYLE_ITALIC	0x00000010

#define DEFAULT_DPI 96

struct My_FaceID_Rec
{
	char file_path[256];
	int face_index;
	int font_style;
};

typedef My_FaceID_Rec*	My_FaceID;

typedef struct
{
	union
	{
		struct
		{
			unsigned short fract;
			short value;
		};
		signed long fixed;
	};
} FTD_FIXED, *FTD_LPFIXED;

inline int FixedToInt(int fixed)
{
	FTD_FIXED *pFixed = (FTD_FIXED*)&fixed;
	if (pFixed->fract >= 0x8000)
		return pFixed->value + 1;
	else
		return pFixed->value;
}