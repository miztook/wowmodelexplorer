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

	virtual IIndexBuffer* getLowResIndexBuffer() const { return LowResIndexBuffer; }
	virtual IIndexBuffer* getHighResIndexBuffer() const { return HighResIndexBuffer; }

	virtual u32 getVertexCount() const { return 9*9 + 8*8; }
	virtual u32 getLowResPrimCount() const { return 8*8*2; } //{ return 4; }
	virtual u32 getHighResPrimCount() const { return 8*8*4; }

private:
	void createLowResIndexBuffer();
	void createHighResIndexBuffer();

	int indexMapBuf(int x, int y) { return ((y+1)/2)*9 + (y/2)*8 + x; }

private:
	IIndexBuffer*		LowResIndexBuffer;
	IIndexBuffer*		HighResIndexBuffer;
};