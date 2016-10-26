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
	virtual const c8* getSetting(E_CONFIG_TYPE type, const c8* key) const = 0;
	virtual void setSetting(E_CONFIG_TYPE type, const c8* key, const c8* val) = 0;

	virtual bool readBaseSetting(IFileSystem* fs) = 0;
	virtual bool readEngineSetting(IFileSystem* fs) = 0;

	virtual bool writeEngineSetting(IFileSystem* fs) = 0;
};