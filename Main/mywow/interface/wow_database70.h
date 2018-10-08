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

	class wowDatabase
	{
	public:
		explicit wowDatabase(const wowEnvironment* env);

		bool init();

		~wowDatabase();

	const CTableStructure* getTableStructure(const char* name) const;

	private:
		bool initFromXml();

	private:
		const wowEnvironment*		Environment;

		std::map<string256, CTableStructure>  DbStructureMap;

		std::unique_ptr<animDB>		AnimDB;
	};
};