#pragma once

#include "IModelExporter.h"

namespace fbxsdk_2015_1
{
	class FbxManager;
	class FbxScene;
}

using namespace fbxsdk_2015_1;

class wowFbxExporter : public IModelExporter
{
private:
	DISALLOW_COPY_AND_ASSIGN(wowFbxExporter);

public:
	wowFbxExporter();
	~wowFbxExporter();

public:
	virtual bool exportM2SceneNode( IM2SceneNode* node, const char* filename );
	virtual bool exportWMOSceneNode( IWMOSceneNode* node, const char* filename );

private:
	bool createM2Scene(FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node, const char* filename);

	bool createM2Materials(FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node);

	bool createM2Mesh(FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node, const char* filename);

	bool createM2Skeleton(FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node);

};