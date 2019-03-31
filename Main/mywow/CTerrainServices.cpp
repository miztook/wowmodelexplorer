#include "stdafx.h"
#include "CTerrainServices.h"
#include "mywow.h"

CTerrainServices::CTerrainServices()
{
	LowResIndexBuffer = new CIndexBuffer(false);
	createLowResIndexBuffer();

	HighResIndexBuffer = new CIndexBuffer(false);
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
	uint32_t isize = 4 * 3;
	uint16_t* indices = (uint16_t*)Hunk_AllocateTempMemory(sizeof(uint16_t) * isize);
	
	uint16_t* out =indices;


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
	uint32_t vcount = 9 * 9 + 8 * 8;
	uint32_t isize = 8 * 8 * 2 * 3;
	uint32_t totalSize = isize * 16 * 16;

	uint16_t* indices = (uint16_t*)Z_AllocateTempMemory(sizeof(uint16_t) * totalSize);
	uint16_t* tmp = (uint16_t*)Z_AllocateTempMemory(sizeof(uint16_t) * isize);
	for (uint16_t i=0; i<isize; ++i)
		tmp[i] = i;

	uint16_t* out = indices;

	for (uint32_t i=0; i<16 * 16; ++i)
	{
		uint32_t add = i * vcount;
		for (int row=0; row<8; ++row) {
			uint16_t* thisrow = &tmp[indexMapBuf(0,row*2)];
			uint16_t* nextrow = &tmp[indexMapBuf(0,(row+1)*2)];

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
	uint32_t vcount = 9 * 9 + 8 * 8;
	uint32_t isize = (8 * 8 * 4) * 3;		
	uint32_t totalSize = isize * 16 * 16;

	uint16_t* indices = (uint16_t*)Z_AllocateTempMemory(sizeof(uint16_t) * totalSize);
	uint16_t* tmp = (uint16_t*)Z_AllocateTempMemory(sizeof(uint16_t) * isize);
	for (uint16_t i=0; i<isize; ++i)
		tmp[i] = i;

	uint16_t* out = indices;

	for (uint32_t i=0; i<16 * 16; ++i)
	{
		uint32_t add = i * vcount;
		for (int row=0; row<8; ++row) { 
			uint16_t* thisrow = &tmp[indexMapBuf(0,row*2)];
			uint16_t* nextrow = &tmp[indexMapBuf(0,row*2+1)];
			uint16_t* overrow = &tmp[indexMapBuf(0,(row+1)*2)];

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

