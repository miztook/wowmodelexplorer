#include "stdafx.h"
#include "MaxInterface.h"

CMaxInterface::CMaxInterface( ExpInterface* pExpInterface, Interface* pInterface )
	: m_expInterface(pExpInterface), m_interface(pInterface)
{

}

s32 CMaxInterface::getCurrentFrame()
{
	return m_interface->GetTime() / GetTicksPerFrame();
}

s32 CMaxInterface::getStartFrame()
{
	return m_interface->GetAnimRange().Start() / GetTicksPerFrame();
}

s32 CMaxInterface::getEndFrame()
{
	return m_interface->GetAnimRange().End() / GetTicksPerFrame();
}

f32 CMaxInterface::getCurrentTime()
{
	return TicksToSec(m_interface->GetTime());
}

s32 CMaxInterface::getStartTime()
{
	return m_interface->GetAnimRange().Start();
}

s32 CMaxInterface::getEndTime()
{
	return m_interface->GetAnimRange().End();
}

s32 CMaxInterface::getFPS()
{
	return GetFrameRate();
}

HWND CMaxInterface::getMainWnd()
{
	return m_interface->GetMAXHWnd();
}
