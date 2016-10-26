#include "stdafx.h"
#include "wow_fbxExporter.h"
#include "mywow.h"
#include "fbxCommon.h"

wowFbxExporter::wowFbxExporter()
{

}

wowFbxExporter::~wowFbxExporter()
{

}

bool wowFbxExporter::exportM2SceneNode( IM2SceneNode* node, const c8* filename )
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	// Create the scene.
	lResult = createM2Scene(lSdkManager, lScene, node, filename);

	if(lResult == false)
	{
		//FBXSDK_printf("\n\nAn error occurred while creating the scene...\n");
		DestroySdkObjects(lSdkManager, lResult);
		return false;
	}

	lResult = SaveScene(lSdkManager, lScene, filename);
	if(lResult == false)
	{
		//FBXSDK_printf("\n\nAn error occurred while saving the scene...\n");
		DestroySdkObjects(lSdkManager, lResult);
		return false;
	}

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

	return lResult == 0;
}

bool wowFbxExporter::exportWMOSceneNode( IWMOSceneNode* node, const c8* filename )
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult = false;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

	return lResult == 0;
}

bool wowFbxExporter::createM2Scene( FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node, const c8* filename )
{
	if (!createM2Materials(pSdkManager, pScene, node))
	{
		_ASSERT(false);
		return false;
	}

	if (!createM2Mesh(pSdkManager, pScene, node, filename))
	{
		_ASSERT(false);
		return false;
	}

	if (!createM2Skeleton(pSdkManager, pScene, node))
	{
		_ASSERT(false);
		return false;
	}

	return true;
}

bool wowFbxExporter::createM2Materials( FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node )
{
	return true;
}

bool wowFbxExporter::createM2Mesh( FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* m2Node, const c8* filename )
{
	FbxNode* root_node = pScene->GetRootNode();

	c8 outfilename[DEFAULT_SIZE];
	getFileNameA(filename, outfilename, DEFAULT_SIZE);
	FbxNode* node = FbxNode::Create(pSdkManager, outfilename);

	root_node->AddChild(node);

	const IFileM2* filem2 = m2Node->getFileM2();
	//mesh node
	FbxMesh* mesh = FbxMesh::Create(pSdkManager, filem2->Name);

	//layer0
	FbxLayer* layer = mesh->GetLayer(0);
	if (!layer)
	{
		int n = mesh->CreateLayer();
		_ASSERT(n == 0);
		layer = mesh->GetLayer(0);
	}

	//normal
	FbxLayerElementNormal* layer_normal = FbxLayerElementNormal::Create(mesh, "Normal");
	layer_normal->SetMappingMode(FbxLayerElement::eByControlPoint);
	layer_normal->SetReferenceMode(FbxLayerElement::eDirect);
	layer->SetNormals(layer_normal);

	//UV

	//fill data
	mesh->InitControlPoints((int)filem2->NumVertices);
	FbxVector4* vertices = mesh->GetControlPoints();

	return true;
}

bool wowFbxExporter::createM2Skeleton( FbxManager* pSdkManager, FbxScene* pScene, IM2SceneNode* node )
{
	return true;
}
