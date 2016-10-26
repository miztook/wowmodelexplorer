#include "stdafx.h"
#include "uiLoader.h"
#include "mywowui.h"

uiLoader::uiLoader()
{
}

uiLoader::~uiLoader()
{
}

bool uiLoader::loadXml( const c8* filename, uiUi& ui )
{
	IMemFile* memFile = g_Engine->getWowEnvironment()->openFile(filename);
	if (!memFile)
		return false;

	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_buffer(memFile->getBuffer(), memFile->getSize(), pugi::parse_default, pugi::encoding_auto);
	if (!parseResult)
	{
		CSysUtility::outputDebug("%s : %s", filename, parseResult.description());
		_ASSERT(false);

		delete memFile;
		return false;
	}

	pugi::xml_node node = doc.first_child();
	_ASSERT(strcmp(node.name(), "Ui") == 0);
	if (!loadXml(node, ui))
	{
		_ASSERT(false);

		delete memFile;
		return false;
	}

	delete memFile;
	return true;
}

bool uiLoader::loadXml( const pugi::xml_node& node, uiUi& ui )
{
	return uiParser_Ui::getInstance().loadXml(node, ui);
}



