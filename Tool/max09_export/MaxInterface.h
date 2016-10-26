#pragma once

#include "max.h"
#include "base.h"

class CMaxNode;
class CMaxMesh;

class CMaxInterface
{
public:
	CMaxInterface(ExpInterface* pExpInterface, Interface* pInterface);

	virtual ~CMaxInterface() {}

public:
	ExpInterface* getExpInterface() { return m_expInterface; }
	Interface* getInterface() const { return m_interface; }

	s32 getCurrentFrame();
	s32 getStartFrame();
	s32 getEndFrame();

	f32 getCurrentTime();
	s32 getStartTime();
	s32 getEndTime();

	s32 getFPS();
	HWND getMainWnd();

	CMaxMesh* getMesh(CMaxNode* pNode);
	CMaxNode* getNode(const c8* name);
	s32 getSelectedNodeCount();
	CMaxNode* getSelectedNode(s32 nodeId);
	CMaxNode* getRootNode();

	bool isBone(CMaxNode* pNode);
	bool isDummy(CMaxNode* pNode);
	bool isMesh(CMaxNode* pNode);
	bool isHook(CMaxNode* pNode);

	bool isBoneINode(INode* node);
	bool isDummyINode(INode* node);
	bool isMeshINode(INode* node);
	bool isHookINode(INode* node);

	bool hasSkinModifier(INode* pNode);

	void setProgressInfo(s32 percentage);
	void startProgressInfo(const c8* text);
	void stopProgressInfo();

private:
	ExpInterface*	m_expInterface;
	Interface*	m_interface;
};

extern  CMaxInterface* g_MaxInterface;