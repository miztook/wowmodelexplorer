#include "stdafx.h"
#include "wow_database70.h"
#include "mywow.h"

#if WOW_VER == 80
#define DATABASEFILE	"database80.xml"
#elif WOW_VER == 70
#define DATABASEFILE	"database70.xml"
#else
#define DATABASEFILE	"database80.xml"
#endif

namespace WowLegion
{
	wowDatabase::wowDatabase(const wowEnvironment* env)
		: Environment(env)
	{
	}

	bool wowDatabase::init()
	{
		if (!initFromXml())
			return false;

		AnimDB = std::make_unique<animDB>(Environment, this);
		CharClassesDB = std::make_unique< charClassesDB>(Environment, this);

		return true;
	}

	wowDatabase::~wowDatabase()
	{
	}

	bool wowDatabase::initFromXml()
	{
		string_path path = Environment->getFileSystem()->getDataDirectory();
		path.normalizeDir();
		path.append(DATABASEFILE);
		path.normalize();

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(path.c_str());
		if (result.status != pugi::status_ok)
			return false;

		auto root = doc.first_child();
		for (auto dbtable : root.children("table"))
		{
			CTableStructure tblStruct;

			tblStruct.name = dbtable.attribute("name").as_string();

			auto attr_dbfile = dbtable.attribute("dbfile");
			if (!attr_dbfile.empty())
				tblStruct.file = attr_dbfile.as_string();
			else
				tblStruct.file = tblStruct.name;

			int fieldId = 0;
			for (auto dbfield : dbtable.children("field"))
			{
				CFieldStructure fieldStruct;

				auto attr_name = dbfield.attribute("name");
				auto attr_type = dbfield.attribute("type");
				auto attr_key = dbfield.attribute("primary");
				auto attr_arraySize = dbfield.attribute("arraySize");
				auto attr_index = dbfield.attribute("createIndex");
				auto attr_pos = dbfield.attribute("pos");
				auto attr_commonData = dbfield.attribute("commonData");
				auto attr_relationshipData = dbfield.attribute("relationshipData");

				if (!attr_name.empty() && !attr_type.empty())
				{
					fieldStruct.name = attr_name.as_string();
					fieldStruct.type = attr_type.as_string();

					fieldStruct.isKey = !attr_key.empty();
					fieldStruct.needIndex = !attr_index.empty();
					fieldStruct.arraySize = attr_arraySize.as_uint();

					fieldStruct.pos = attr_pos.as_int();
					fieldStruct.isCommonData = attr_commonData.as_bool();
					fieldStruct.isRelationshipData = attr_relationshipData.as_bool();

					//add
					tblStruct.fields.emplace_back(fieldStruct);
				}

				++fieldId;
			}

			DbStructureMap.insert(std::pair<string256, CTableStructure>(tblStruct.name, tblStruct));
		}
		return true;
	}

	const WowLegion::CTableStructure* wowDatabase::getTableStructure(const char* name) const
	{
		auto itr = DbStructureMap.find(name);
		if (itr == DbStructureMap.end())
			return nullptr;
		return &itr->second;
	}

};