#pragma once

class ILostResetCallback
{
public:
	virtual void onLost() = 0;
	virtual void onReset() = 0;
};