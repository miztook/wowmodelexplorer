#include "stdafx.h"
#include "CTerrainServices.h"
#include "mywow.h"

CTerrainServices::CTerrainServices()
{
	LowResIndexBuffer = new IIndexBuffer(false);
	createLowResIndexBuffer();

	HighResIndexBuffer = new IIndexBuffer(false);
	createHighResIndexBuffer();
}

CTerrainServices::~CTerrainServices()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(HighResIndexBuffer);
	delete HighResIndexBuffer;

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(LowResIndexBuffer);
	delete LowResIndexBuffer;
}

void CTerrainServices::createLowResIndexBuffer()
{
	//低精度的存在lod缝隙问题
	/*
	u32 isize = 4 * 3;
	u16* indices = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * isize);
	
	u16* out =indices;


	*out++ = 0;
	*out++ = 8;
	*out++ = 9 * 4 + 8 * 4 + 4;

	*out++ = 8;
	*out++ = 9 * 8 + 8 * 8 + 8;
	*out++ = 9 * 4 + 8 * 4 + 4;

	*out++ = 9 * 8 + 8 * 8 + 8;
	*out++ = 9 * 8 + 8 * 8;
	*out++ = 9 * 4 + 8 * 4 + 4;

	*out++ = 9 * 8 + 8 * 8;
	*out++ = 0;
	*out++ = 9 * 4 + 8 * 4 + 4;

	ASSERT(out == indices + isize);
	*/
	u32 vcount = 9 * 9 + 8 * 8;
	u32 isize = 8 * 8 * 2 * 3;
	u32 totalSize = isize * 16 * 16;

	u16* indices = (u16*)Z_AllocateTempMemory(sizeof(u16) * totalSize);
	u16* tmp = (u16*)Z_AllocateTempMemory(sizeof(u16) * isize);
	for (u16 i=0; i<isize; ++i)
		tmp[i] = i;

	u16* out = indices;

	for (u32 i=0; i<16 * 16; ++i)
	{
		u32 add = i * vcount;
		for (int row=0; row<8; ++row) {
			u16* thisrow = &tmp[indexMapBuf(0,row*2)];
			u16* nextrow = &tmp[indexMapBuf(0,(row+1)*2)];

			for (int col=0; col<8; ++col)
			{
				*out++ = thisrow[col] + add;
				*out++ = thisrow[col + 1] + add;
				*out++ = nextrow[col + 1] + add;

				*out++ = thisrow[col] + add;
				*out++ = nextrow[col + 1] + add;
				*out++ = nextrow[col] + add;
			}
		}
	}

	ASSERT(out == indices + totalSize);
	
	LowResIndexBuffer->set(indices, EIT_16BIT, totalSize, EMM_STATIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(LowResIndexBuffer);

	Z_FreeTempMemory(tmp);
	Z_FreeTempMemory(indices);
}

void CTerrainServices::createHighResIndexBuffer()
{
	u32 vcount = 9 * 9 + 8 * 8;
	u32 isize = (8 * 8 * 4) * 3;		
	u32 totalSize = isize * 16 * 16;

	u16* indices = (u16*)Z_AllocateTempMemory(sizeof(u16) * totalSize);
	u16* tmp = (u16*)Z_AllocateTempMemory(sizeof(u16) * isize);
	for (u16 i=0; i<isize; ++i)
		tmp[i] = i;

	u16* out = indices;

	for (u32 i=0; i<16 * 16; ++i)
	{
		u32 add = i * vcount;
		for (int row=0; row<8; ++row) { 
			u16* thisrow = &tmp[indexMapBuf(0,row*2)];
			u16* nextrow = &tmp[indexMapBuf(0,row*2+1)];
			u16* overrow = &tmp[indexMapBuf(0,(row+1)*2)];

			for (int col=0; col<8; ++col)
			{
				*out++ = thisrow[col] + add;
				*out++ = thisrow[col + 1] + add;
				*out++ = nextrow[col] + add;

				*out++ = thisrow[col + 1] + add;
				*out++ = overrow[col + 1] + add;
				*out++ = nextrow[col] + add;

				*out++ = overrow[col + 1] + add;
				*out++ = overrow[col] + add;
				*out++ = nextrow[col] + add;

				*out++ = overrow[col] + add;
				*out++ = thisrow[col] + add;
				*out++ = nextrow[col] + add;
			}
		}
	}
	ASSERT(out == indices + totalSize);

	HighResIndexBuffer->set(indices, EIT_16BIT, totalSize, EMM_STATIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(HighResIndexBuffer);

	Z_FreeTempMemory(tmp);
	Z_FreeTempMemory(indices);
}

