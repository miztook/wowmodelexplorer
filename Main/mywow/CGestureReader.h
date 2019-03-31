#pragma once

#include "IGestureReader.h"
#include "CSysSync.h"
#include "linklist.h"
#include "IResourcePool.h"
#include "IGestureRecognizer.h"
#include <vector>
#include <map>

class CGestureRecognizerBase;

class CGestureReader : public IGestureReader
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGestureReader);

public: 
	CGestureReader();
	~CGestureReader();

public:
	virtual void tick();

	virtual void readTouchInfo(const SGesTouchInfo* arrTouches, uint32_t count);

	virtual bool addGestureToQueue(const SGestureInfo& gesInfo);

	virtual bool removeGestureFromQueue(SGestureInfo& gesInfo);

	virtual bool isQueueEmpty() const;

	IGestureRecognizer::E_RECOGNIZER_STATE getGestureState(E_GESTURE_TYPE type);

#if defined(A_PLATFORM_ANDROID)

	virtual void processTouchesBegin(int touchID, float x, float y);
	virtual void processTouchesEnd(int touchID, float x, float y);
	virtual void processTouchesMove(int touchNumber, int* touchIDs, float* xs, float* ys);
	virtual void processTouchesCancel(int touchNumber, int* touchIDs, float* xs, float* ys);

#endif

private:
	lock_type		cs;
	CGestureRecognizerBase*		GestureRecognizers[GestureType_Num];

	std::vector<SGesTouchInfo>	BeginTouchInfos;
	std::vector<SGesTouchInfo>	MoveTouchInfos;
	std::vector<SGesTouchInfo>	EndTouchInfos;
	std::vector<SGesTouchInfo>	CancelTouchInfos;

	IResourcePool<SGestureInfo>		GesturePool;
	LENTRY		GestureInfoList;

public:
	struct STouchCache
	{
		float time;
		float posX;
		float posY;
	};

typedef std::map<uintptr_t, STouchCache, std::less<uintptr_t>, qzone_allocator<std::pair<uintptr_t, STouchCache> > > T_TouchCacheMap;

//platform
#ifdef MW_PLATFORM_WINDOWS

public:
	void filterWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	enum
	{
		WIN_TOUCH_NONE = 0,
		WIN_TOUCH_LBUTTON,
		WIN_TOUCH_RBUTTON,
		WIN_TOUCH_NUM,
	};

private:
	int32_t FingerId[WIN_TOUCH_NUM];
	STouchCache CacheButtonDown[WIN_TOUCH_NUM];

#elif defined(A_PLATFORM_ANDROID)

private:
	T_TouchCacheMap		m_TouchCacheMap;

#endif
};