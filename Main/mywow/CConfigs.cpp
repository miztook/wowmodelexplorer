#include "stdafx.h"
#include "CConfigs.h"
#include "mywow.h"
#include "CFileSystem.h"

#define  SETTINGS	"setting.cfg"
#define  ENGINESETTINGS "engine.cfg"

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
	c8 path[QMAX_PATH];
	Q_strcpy(path, QMAX_PATH, fs->getBaseDirectory());
	Q_strcat(path, QMAX_PATH, SETTINGS);
	return readSetting(fs, path, ECT_SETTING);
}

bool CConfigs::readEngineSetting( IFileSystem* fs )
{
	c8 path[QMAX_PATH];
	Q_strcpy(path, QMAX_PATH, fs->getBaseDirectory());
	Q_strcat(path, QMAX_PATH, ENGINESETTINGS);
	return readSetting(fs, path, ECT_ENGINESETTING);
}

bool CConfigs::writeEngineSetting( IFileSystem* fs )
{
	c8 path[QMAX_PATH];
	Q_strcpy(path, QMAX_PATH, fs->getBaseDirectory());
	Q_strcat(path, QMAX_PATH, ENGINESETTINGS);
	return writeSetting(fs, path, ECT_ENGINESETTING);
}

bool CConfigs::readSetting( IFileSystem* fs, const c8* path, E_CONFIG_TYPE type )
{
	T_SettingMap& settingMap = SettingMaps[type];

	settingMap.clear();

	IReadFile* settingFile = fs->createAndOpenFile(path, false);
	if (!settingFile)
		return false;

	c8 buffer[1024] = {0};
	c8 name[32] = {0};
	c8 val[256] = {0};

	while(!settingFile->isEof())
	{
		u32 len = settingFile->readLine(buffer, 1024);
		if (!isComment(buffer))
		{
			u32 count = 0;
			while(count < len)
			{
				if (buffer[count] == '=' && count > 0)
				{
					trim(buffer, count, name, 32);
					trim(&buffer[count+1], val, 256);
					settingMap[name] = val;
					break;
				}
				++count;
			}
			memset(buffer, 0, sizeof(buffer));
			memset(name, 0, sizeof(name));
			memset(val, 0, sizeof(val));
		}
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

	c8 buffer[1024] = {0};

	for (T_SettingMap::const_iterator itr = settingMap.begin(); itr != settingMap.end(); ++itr)
	{
		Q_sprintf(buffer, 1024, "%s = %s\n", itr->first.c_str(), itr->second.c_str());

		settingFile->writeText(buffer, 1024);
	}

	settingFile->flush();

	delete settingFile;

	return true;
}

