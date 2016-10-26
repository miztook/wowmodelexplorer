#pragma once

#include "IConfigs.h"
#include "core.h"
#include <unordered_map>

class CConfigs : public IConfigs
{
private:
	DISALLOW_COPY_AND_ASSIGN(CConfigs);

public: 
	CConfigs();

public:
	virtual const c8* getSetting(E_CONFIG_TYPE type, const c8* key) const;
	virtual void setSetting(E_CONFIG_TYPE type, const c8* key, const c8* val);

	virtual bool readBaseSetting(IFileSystem* fs);
	virtual bool readEngineSetting(IFileSystem* fs);
	virtual bool writeEngineSetting(IFileSystem* fs);

private:
	bool readSetting(IFileSystem* fs, const c8* path, E_CONFIG_TYPE type);

	bool writeSetting(IFileSystem* fs, const c8* path, E_CONFIG_TYPE type);

private:
	typedef std::unordered_map<string32, string256, string32::string_hash>	T_SettingMap;

	T_SettingMap SettingMaps[ECT_COUNT];
};