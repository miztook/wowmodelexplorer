#pragma once

#include "base.h"
#include "wow_def.h"
#include "wow_dbc70.h"
#include <map>

class wowEnvironment;

namespace WowLegion
{
	class CFieldStructure
	{
	public:
		string256 name;
		string256 type;
		bool isKey;
		bool needIndex;
		u32 arraySize;
		s32 id;

		s32 pos;
		bool isCommonData;
		bool isRelationshipData;
	};

	class CTableStructure
	{
	public:
		string256 name;
		string256 file;
		std::vector<CFieldStructure> fields;
	};

	class wowDatabase70
	{
	public:
		explicit wowDatabase70(wowEnvironment* env);

		bool init();

		~wowDatabase70();

	private:
		bool initFromXml();

	private:
		wowEnvironment*		Environment;

		std::map<string256, CTableStructure>  DbStructureMap;
	};
};