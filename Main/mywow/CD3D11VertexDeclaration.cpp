#include "stdafx.h"
#include "CD3D11VertexDeclaration.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"
#include "CD3D11Shader.h"

//vertex declaration

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_P[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PC[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //color
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PCT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //color
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PN[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PNC[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //normal
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //color
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PNT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //normal
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PNCT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //normal
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //color
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PNCT2[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //normal
	{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //color
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
};

D3D11_INPUT_ELEMENT_DESC CD3D11VertexDeclaration::Decl_PNT2W_M[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//position
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //tex
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },	   //blend weight
	{ "BLENDINDICES", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },	 //blend indices
};

CD3D11VertexDeclaration::CD3D11VertexDeclaration( E_VERTEX_TYPE vtype )
	: VertexType(vtype)
{
	switch (VertexType)
	{
	case EVT_P:
		IAElements = Decl_P;
		Size = sizeof(Decl_P)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PC:
		IAElements = Decl_PC;
		Size = sizeof(Decl_PC)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PCT:
		IAElements = Decl_PCT;
		Size = sizeof(Decl_PCT)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PN:
		IAElements = Decl_PN;
		Size = sizeof(Decl_PN)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PNC:
		IAElements = Decl_PNC;
		Size = sizeof(Decl_PNC)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PNT:
		IAElements = Decl_PNT;
		Size = sizeof(Decl_PNT)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PT:
		IAElements = Decl_PT;
		Size = sizeof(Decl_PT)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PNCT:
		IAElements = Decl_PNCT;
		Size = sizeof(Decl_PNCT)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PNCT2:
		IAElements = Decl_PNCT2;
		Size = sizeof(Decl_PNCT2)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	case EVT_PNT2W:
		IAElements = Decl_PNT2W_M;
		Size = sizeof(Decl_PNT2W_M)/sizeof(D3D11_INPUT_ELEMENT_DESC);
		break;
	default:
		ASSERT(false);
	}
}

CD3D11VertexDeclaration::~CD3D11VertexDeclaration()
{
	for (T_LayoutMap::iterator itr = LayoutMap.begin(); itr != LayoutMap.end(); ++itr)
	{
		SAFE_RELEASE_STRICT(itr->second);
	}
}

ID3D11InputLayout* CD3D11VertexDeclaration::getInputLayout( IVertexShader* shader )
{
	ID3D11Device* pDevice = static_cast<CD3D11Driver*>(g_Engine->getDriver())->pID3DDevice11;
	CD3D11VertexShader* vertexShader = static_cast<CD3D11VertexShader*>(shader);
	const void* signature = vertexShader->getCodeBuffer();
	if (!signature)
	{
		ASSERT(false);
		return NULL_PTR;
	}

	u32 shaderSize = vertexShader->getCodeSize();

	ID3D11InputLayout* layout = NULL_PTR;
	T_LayoutMap::const_iterator itr = LayoutMap.find(signature);
	if (itr != LayoutMap.end())
	{
		layout = itr->second;
	}
	else
	{
		if(FAILED(pDevice->CreateInputLayout(IAElements, Size, signature, shaderSize, &layout)))
		{
			ASSERT(false);
			return NULL_PTR;
		}
		LayoutMap[signature] = layout;
	}
	return layout;
}

#endif