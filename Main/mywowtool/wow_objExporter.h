#pragma once

#include "IModelExporter.h"

class CFileM2;
class CFileWMO;

class wowObjExporter : public IModelExporter
{
private:
	DISALLOW_COPY_AND_ASSIGN(wowObjExporter);

public:
	wowObjExporter();
	~wowObjExporter();

public:
	virtual bool exportM2SceneNode( IM2SceneNode* node, const c8* filename );
	virtual bool exportWMOSceneNode( IWMOSceneNode* node, const c8* filename );
	virtual bool exportWMOSceneNodeGroups( IWMOSceneNode* node, const c8* filename);

private:
	bool exportFileM2(IWriteFile* pFileObj, IWriteFile* pFileMtl, const CFileM2* pFileM2, wow_m2instance* pM2Instance);
	bool exportFileM2Vertices(IWriteFile* pFile, const CFileM2* pFileM2);
	bool exportFileM2Materials(IWriteFile* pFile, const CFileM2* pFileM2, wow_m2instance* pM2Instance);

private:
	bool exportFileWMO(IWriteFile* pFileObj, IWriteFile* pFileMtl, const CFileWMO* Wmo);

	bool exportWMOVertices(IWriteFile* pFile, const CFileWMO* Wmo);
	bool exportWMOMaterials(IWriteFile* pFile, const CFileWMO* Wmo);

private:
	bool exportFileWMOGroup(const c8* filename, const CFileWMO* Wmo, u32 iGroup);
	bool exportWMOGroupVertices(IWriteFile* pFile, const CFileWMO* Wmo, u32 iGroup);
	bool exportWMOGroupMaterials(IWriteFile* pFile, const CFileWMO* Wmo, u32 iGroup);

};