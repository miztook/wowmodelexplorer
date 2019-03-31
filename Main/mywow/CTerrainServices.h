#pragma once

#include "ITerrainServices.h"

class CTerrainServices : public ITerrainServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CTerrainServices);

public:
	CTerrainServices();
	~CTerrainServices();

public:

	CIndexBuffer* getLowResIndexBuffer() const { return LowResIndexBuffer; }
	CIndexBuffer* getHighResIndexBuffer() const { return HighResIndexBuffer; }

	uint32_t getVertexCount() const { return 9*9 + 8*8; }
	uint32_t getLowResPrimCount() const { return 8*8*2; } //{ return 4; }
	uint32_t getHighResPrimCount() const { return 8*8*4; }

private:
	void createLowResIndexBuffer();
	void createHighResIndexBuffer();

	int indexMapBuf(int x, int y) { return ((y+1)/2)*9 + (y/2)*8 + x; }

private:
	CIndexBuffer*		LowResIndexBuffer;
	CIndexBuffer*		HighResIndexBuffer;
};