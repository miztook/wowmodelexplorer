#pragma once

class IFileM2;

class IM2LoadCallback
{
public:
	virtual void onM2Loaded(IFileM2* filem2) = 0;
};