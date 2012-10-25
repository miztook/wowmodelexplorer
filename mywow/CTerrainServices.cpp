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
	u32 vcount = 9 * 9 + 8 * 8;

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

	_ASSERT(out == indices + isize);
	*/

	u32 isize = 8 * 8 * 2 * 3;

	u16* indices = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * isize);
	u16* tmp = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * isize);
	for (u16 i=0; i<isize; ++i)
		tmp[i] = i;

	u16* out = indices;

	for (int row=0; row<8; ++row) {
		u16* thisrow = &tmp[indexMapBuf(0,row*2)];
		u16* nextrow = &tmp[indexMapBuf(0,(row+1)*2)];

		for (int col=0; col<8; ++col)
		{
			*out++ = thisrow[col];
			*out++ = thisrow[col + 1];
			*out++ = nextrow[col + 1];

			*out++ = thisrow[col];
			*out++ = nextrow[col + 1];
			*out++ = nextrow[col];
		}
	}

	_ASSERT(out == indices + isize);
	LowResIndexBuffer->set(indices, EIT_16BIT, isize, EMM_STATIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(LowResIndexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(LowResIndexBuffer, 0, isize);

	Hunk_FreeTempMemory(tmp);
	Hunk_FreeTempMemory(indices);
}

void CTerrainServices::createHighResIndexBuffer()
{
	u32 vcount = 9 * 9 + 8 * 8;
	u32 isize = (8 * 8 * 4) * 3;	

	u16* indices = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * isize);
	u16* tmp = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * isize);
	for (u16 i=0; i<isize; ++i)
		tmp[i] = i;

	u16* out = indices;

	for (int row=0; row<8; ++row) { 
		u16* thisrow = &tmp[indexMapBuf(0,row*2)];
		u16* nextrow = &tmp[indexMapBuf(0,row*2+1)];
		u16* overrow = &tmp[indexMapBuf(0,(row+1)*2)];

		for (int col=0; col<8; ++col)
		{
			*out++ = thisrow[col];
			*out++ = thisrow[col + 1];
			*out++ = nextrow[col];

			*out++ = thisrow[col + 1];
			*out++ = overrow[col + 1];
			*out++ = nextrow[col];
			
			*out++ = overrow[col + 1];
			*out++ = overrow[col];
			*out++ = nextrow[col];
			
			*out++ = overrow[col];
			*out++ = thisrow[col];
			*out++ = nextrow[col];
		}
	}

	_ASSERT(out == indices + isize);

	HighResIndexBuffer->set(indices, EIT_16BIT, isize, EMM_STATIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(HighResIndexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(HighResIndexBuffer, 0, isize);

	Hunk_FreeTempMemory(tmp);
	Hunk_FreeTempMemory(indices);
}

