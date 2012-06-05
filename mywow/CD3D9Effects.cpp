#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"
#include "CD3D9Shader.h"
#include "CD3D9Texture.h"

void BasicHlsl_setShaderConst(IEffect* effect, const SMaterial& material);

void CD3D9ShaderServices::loadAllEffects()
{
	loadEffect("Effects\\BasicHLSL.fx", "t0", EET_BASICHLSL, BasicHlsl_setShaderConst);
}

void BasicHlsl_setShaderConst( IEffect* effect, const SMaterial& material )
{
	IVideoDriver* driver = g_Engine->getDriver();
	ID3DXEffect* d3d9Effect = static_cast<CD3D9Effect*>(effect)->getEffect();
	CD3D9Texture* tex = static_cast<CD3D9Texture*>(driver->getTexture(0));
	if(tex)
	{
		IDirect3DTexture9* texture = tex->getDXTexture();
		d3d9Effect->SetTexture( "g_MeshTexture",  texture );
	}

	const matrix4& w = driver->getTransform(ETS_WORLD);
	const matrix4& v = driver->getTransform(ETS_VIEW);
	const matrix4& p = driver->getTransform(ETS_PROJECTION);

	matrix4 wvp = p * v * w;														//shader的 matrix 乘法 顺序和普通的matrix4相反, shader的矩阵 16字节对齐

	D3DXMATRIXA16		world(*(const D3DXMATRIX*)w.pointer());
	matrix4A16 worldViewProjection = *(const matrix4A16*)wvp.pointer();

	//variable per frame
	d3d9Effect->SetMatrix( "g_mWorldViewProjection", (const D3DXMATRIX*)&worldViewProjection);
	d3d9Effect->SetMatrix( "g_mWorld", &world);
	d3d9Effect->SetFloat( "g_fTime", (float)0.01f );
}
