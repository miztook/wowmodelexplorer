#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"
#include "CD3D9Shader.h"
#include "CD3D9Texture.h"

void CD3D9ShaderServices::loadAllPixelShaders(const c8* profile)
{
	c8 basePath[128];
	sprintf_s(basePath, 128, "Pixel\\%s\\", profile);

}
