#include "stdafx.h"
#include "CConfigs.h"
#include "CFileSystem.h"
#include "IReadFile.h"
#include "IWriteFile.h"

#define  SETTINGS	"\\setting.cfg"
#define  ENGINESETTINGS "\\engine.cfg"

CConfigs::CConfigs()
{

}

const c8* CConfigs::getSetting( E_CONFIG_TYPE type, const c8* key ) const
{
	if (type < 0 || type >= ECT_COUNT)
		return "";

	T_SettingMap::const_iterator itr = SettingMaps[type].find(key);
	if (itr != SettingMaps[type].end())
	{
		return itr->second.c_str();
	}
	return "";
}

void CConfigs::setSetting( E_CONFIG_TYPE type, const c8* key, const c8* val )
{
	if (type < 0 || type >= ECT_COUNT)
		return;

	SettingMaps[type][key] = val;
}

bool CConfigs::readBaseSetting(IFileSystem* fs)
{
	string256  path = fs->getInitialWorkingDirectory();
	path.append(SETTINGS);
	return readSetting(fs, path.c_str(), ECT_SETTING);
}

bool CConfigs::readEngineSetting( IFileSystem* fs )
{
	string256  path = fs->getInitialWorkingDirectory();
	path.append(ENGINESETTINGS);
	return readSetting(fs, path.c_str(), ECT_ENGINESETTING);
}

bool CConfigs::writeEngineSetting( IFileSystem* fs )
{
	string256  path = fs->getInitialWorkingDirectory();
	path.append(ENGINESETTINGS);
	return writeSetting(fs, path.c_str(), ECT_ENGINESETTING);
}

bool CConfigs::readSetting( IFileSystem* fs, const c8* path, E_CONFIG_TYPE type )
{
	T_SettingMap& settingMap = SettingMaps[type];

	settingMap.clear();

	IReadFile* settingFile = fs->createAndOpenFile(path, false);
	if (!settingFile)
		return false;

	c8 buffer[256] = {0};
	c8 name[32] = {0};
	c8 val[64] = {0};

	u32 len = settingFile->readLine(buffer, 256);
	while(len)
	{
		u32 count = 0;
		while(count < len)
		{
			if (buffer[count] == '=' && count > 0)
			{
				trim(buffer, count, name, 32);
				trim(&buffer[count+1], val, 64);
				settingMap[name] = val;
				break;
			}
			++count;
		}
		memset(buffer, 0, sizeof(buffer));
		memset(name, 0, sizeof(name));
		memset(val, 0, sizeof(val));
		len = settingFile->readLine(buffer);
	}

	delete settingFile;

	return true;
}

bool CConfigs::writeSetting( IFileSystem* fs, const c8* path, E_CONFIG_TYPE type )
{
	T_SettingMap& settingMap = SettingMaps[type];

	IWriteFile* settingFile = fs->createAndWriteFile(path, false);
	if (!settingFile)
		return false;

	c8 buffer[256] = {0};

	for (T_SettingMap::const_iterator itr = settingMap.begin(); itr != settingMap.end(); ++itr)
	{
		sprintf_s(buffer, 256, "%s = %s\n", itr->first.c_str(), itr->second.c_str());

		settingFile->writeText(buffer, 256);
	}

	settingFile->flush();

	delete settingFile;

	return true;
}

