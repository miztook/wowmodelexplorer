#include "stdafx.h"
#include "editor_mwTool.h"
#include "CSysUtility.h"
#include "wow_exportUtility.h"
#include "wow_objExporter.h"
#include "wow_fbxExporter.h"

void mwTool_create()
{
	createTool();
}

void mwTool_destroy()
{
	destroyTool();
}

bool mwTool_openUrlToJsonFile( const c8* url, const c8* filename )
{
	return CSysUtility::openURLtoJsonFile(url, filename);
}

bool mwTool_parseCharacterArmoryInfo( const c8* filename, editor::SCharacterArmoryInfo* charInfo )
{
	SCharArmoryInfo cInfo;
	if (!g_mwTool->getWowArmory()->parseCharacterArmoryInfo(filename, &cInfo))
	{
		return false;
	}

	charInfo->Race = cInfo.Race;
	charInfo->Gender = cInfo.Gender;
	utf8to16(cInfo.Name, charInfo->Name, DEFAULT_SIZE);
	utf8to16(g_Engine->getWowDatabase()->getClassShortName(cInfo.ClassId), charInfo->ClassShortName, DEFAULT_SIZE);

	charInfo->SkinColor = cInfo.SkinColor;
	charInfo->FaceType = cInfo.FaceType;
	charInfo->HairColor = cInfo.HairColor;
	charInfo->HairStyle = cInfo.HairStyle;
	charInfo->FacialHair = cInfo.FacialHair;

	charInfo->Head = cInfo.Head;
	charInfo->Shoulder = cInfo.Shoulder;
	charInfo->Boots = cInfo.Boots;
	charInfo->Belt = cInfo.Belt;
	charInfo->Shirt = cInfo.Shirt;
	charInfo->Pants = cInfo.Pants;
	charInfo->Chest = cInfo.Chest;
	charInfo->Bracers = cInfo.Bracers;
	charInfo->Gloves = cInfo.Gloves;
	charInfo->HandRight = cInfo.HandRight;
	charInfo->HandLeft = cInfo.HandLeft;
	charInfo->Cape = cInfo.Cape;
	charInfo->Tabard = cInfo.Tabard;

	return true;
}

bool mwTool_exportBlpAsTga(const c8* blpfilename, const c8* tgafilename, bool bAlpha)
{
	return AUX_ExportBlpAsTga(blpfilename, tgafilename, bAlpha);
}

bool mwTool_exportBlpAsTgaDir(const char* blpfilename, const char* dirname, bool bAlpha)
{
	return AUX_ExportBlpAsTgaDir(blpfilename, dirname, bAlpha);
}

bool mwTool_exportM2SceneNodeToOBJ(IM2SceneNode* node, const c8* dirname)
{
	if(!node || !node->getFileM2())
		return false;

	c8 filename[MAX_PATH];
	getFileNameNoExtensionA(node->getFileM2()->getFileName(), filename, MAX_PATH);
	string512 path = dirname;
	path.normalizeDir();
	path.append(filename);
	path.normalizeDir();
	path.append(filename);
	path.append(".obj");

	wowObjExporter exporter;
	return exporter.exportM2SceneNode(node, path.c_str());
}

bool mwTool_exportWMOSceneNodeToOBJ(IWMOSceneNode* node, const c8* dirname)
{
	if(!node || !node->getFileWMO())
		return false;

	c8 filename[MAX_PATH];
	getFileNameNoExtensionA(node->getFileWMO()->getFileName(), filename, MAX_PATH);
	string512 path = dirname;
	path.normalizeDir();
	path.append(filename);
	path.normalizeDir();
	path.append(filename);
	path.append(".obj");

	wowObjExporter exporter;
	return exporter.exportWMOSceneNodeGroups(node, path.c_str());
}
