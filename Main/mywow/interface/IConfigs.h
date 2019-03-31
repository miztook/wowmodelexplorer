#pragma once

#include "base.h"

class IFileSystem;

enum E_CONFIG_TYPE
{
	ECT_SETTING = 0,				//base
	ECT_ENGINESETTING,

	ECT_COUNT,
};

class IConfigs
{
public:
	virtual ~IConfigs() {}

public:
	virtual const char* getSetting(E_CONFIG_TYPE type, const char* key) const = 0;
	virtual void setSetting(E_CONFIG_TYPE type, const char* key, const char* val) = 0;

	virtual bool readBaseSetting(IFileSystem* fs) = 0;
	virtual bool readEngineSetting(IFileSystem* fs) = 0;

	virtual bool writeEngineSetting(IFileSystem* fs) = 0;
};