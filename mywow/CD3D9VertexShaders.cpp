#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"
#include "CD3D9Shader.h"

struct HWSkinning_CB
{
	matrix4 		mWorldViewProjection;    // World * View * Projection matrix
	SColorf 		MaterialAmbient;
	SColorf			AmbientColor;
	SColorf			EmissiveColor;
	float				Params[4];		//0: numBones, 1: animTexture
	matrix4			mTexture;
};

void HwSkinning_setShaderConst(IVertexShader* vs, const SMaterial& material);

void CD3D9ShaderServices::loadAllVertexShaders(const c8* profile)
{
	c8 basePath[128];
	sprintf_s(basePath, 128, "Vertex\\%s\\", profile);

	string128 path(basePath);
	path.append("HWSkinning.fx");
	loadVShaderHLSL(path.c_str(), "main", profile, EVST_HWSKINNING, HwSkinning_setShaderConst);
}

void HwSkinning_setShaderConst( IVertexShader* vs, const SMaterial& material )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	IVideoDriver* driver = g_Engine->getDriver();
	IDirect3DDevice9*	pID3DDevice = (IDirect3DDevice9*)driver->getD3DDevice();

	SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
	_ASSERT(boneMats);
	_ASSERT(boneMats->count <= 58);

	// para
	HWSkinning_CB cbuffer;

	driver->getShaderServices()->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.Params[0] = unit->u.useBoneMatrix ? (float)boneMats->maxWeights : 0.0f;

	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] =  useAnimTex ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
	}

	cbuffer.AmbientColor =  SColorf(driver->getSceneStateServices()->getAmbientLight());
	cbuffer.MaterialAmbient = material.Lighting ? SColorf(material.AmbientColor) : SColorf(0);
	cbuffer.EmissiveColor = material.Lighting ? SColor(0) : SColorf(material.EmissiveColor);

	// set update buffer
	u32 size = useAnimTex ? sizeof(HWSkinning_CB) / 16 : (sizeof(HWSkinning_CB) - sizeof(matrix4)) / 16;
	pID3DDevice->SetVertexShaderConstantF(0, (const float*)&cbuffer, size);


	u32 regIndex = vs->getRegIndex("g_mBoneMatrixArray");
	if(unit->u.useBoneMatrix && regIndex)
	{
		pID3DDevice->SetVertexShaderConstantF(regIndex, (const float*)boneMats->matrices, boneMats->count * 4);
	}

}