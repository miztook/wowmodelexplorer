#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "CD3D9Shader.h"

CD3D9ShaderServices::CD3D9ShaderServices()
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	Device = Driver->pID3DDevice;

	LastShaderState.reset();
	ShaderState.reset();
}

CD3D9ShaderServices::~CD3D9ShaderServices()
{
	for (uint32_t i=0; i < EVST_COUNT; ++i)
	{
		delete VertexShaders[i];
	}

	for (uint32_t i=0; i < EPST_COUNT; ++i)
	{
		for (uint32_t k=PS_Macro_None; k<PS_Macro_Num; ++k)
			delete PixelShaders[i][k];
	}
}

void CD3D9ShaderServices::loadAll()
{
	const char* profile = "";

	//vs
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	if (Driver->queryFeature(EVDF_VERTEX_SHADER_3_0))
	{
		profile = "vs_3_0";
		vs30Loader.loadAll(this);
	}
	else if (Driver->queryFeature(EVDF_VERTEX_SHADER_2_0))
	{
		profile = "vs_2_0";
		vs20Loader.loadAll(this);
	}
	else 
	{
		ASSERT(false);
		profile = "";
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);

	//ps
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	if (Driver->queryFeature(EVDF_PIXEL_SHADER_3_0))
	{
		profile = "ps_3_0";
		ps30Loader.loadAll(this);
	}
	else if (Driver->queryFeature(EVDF_PIXEL_SHADER_2_0))
	{
		profile = "ps_2_0";
		ps20Loader.loadAll(this);
	}
	else
	{
		ASSERT(false);
		profile = "";
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);
}

#ifdef DIRECTX_USE_COMPILED_SHADER

bool CD3D9ShaderServices::loadVShader( const char* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	ASSERT(false);
	return false;
}

bool CD3D9ShaderServices::loadPShader( const char* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	ASSERT(false);
	return false;
}

bool CD3D9ShaderServices::loadVShaderHLSL( const char* filename, const char* entry, const char* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), true);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadVShaderHLSL Failed: Bls Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	IDirect3DVertexShader9* vertexShader = nullptr;
	uint32_t len = 0;

	//read bls file
	BlsHeader header;
	len = rfile->read(&header, sizeof(header));

	ASSERT(len == sizeof(header));
	ASSERT(header.shaderCount == 1);

	BlsBlockHeader blockHeader;
	len = rfile->read(&blockHeader, sizeof(blockHeader));

	ASSERT(len == sizeof(blockHeader));
	ASSERT(blockHeader.codeSize);

	//const
	uint8_t* constBuffer = (uint8_t*)Z_AllocateTempMemory(blockHeader.constSize);
	uint8_t* codeBuffer = (uint8_t*)Z_AllocateTempMemory(blockHeader.codeSize);

	len = rfile->read(constBuffer, blockHeader.constSize);
	ASSERT(len == blockHeader.constSize);
	len = rfile->read(codeBuffer, blockHeader.codeSize);
	ASSERT(len == blockHeader.codeSize);

	delete rfile;

	//code
	HRESULT hr = Device->CreateVertexShader((DWORD*)codeBuffer, &vertexShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadVShaderHLSL Failed : CreateVertexShader");
		ASSERT(false);

		Z_FreeTempMemory(codeBuffer);
		Z_FreeTempMemory(constBuffer);
		return false;
	}
	Z_FreeTempMemory(codeBuffer);

	CD3D9VertexShader* shader = new CD3D9VertexShader(type, vertexShader, callback);
	if (blockHeader.constSize)
	{
		CD3D9VertexShader::T_ConstList& constList = shader->getConstList();

		const uint8_t* p = constBuffer;
		while (p < constBuffer + blockHeader.constSize)
		{
			BlsConst* constEntry = (BlsConst*)p;
			SDx9ConstDesc desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;
			desc.size = constEntry->size;
			constList.emplace_back(desc);

			p += (sizeof(BlsConst) -1);
			p+= (strlen((const char*)p)+1);
		}
		ASSERT(p == constBuffer + blockHeader.constSize);
		constList.sort();
	}
	Z_FreeTempMemory(constBuffer);

	ASSERT(VertexShaders[type] == nullptr);
	VertexShaders[type] = shader;
	
	return true;
}

bool CD3D9ShaderServices::loadPShaderHLSL( const char* filename, const char* entry, const char* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), true);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadPShaderHLSL Failed: Bls Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	IDirect3DPixelShader9* pixelShader = nullptr;
	uint32_t len = 0;

	//read bls file
	BlsHeader header;
	len = rfile->read(&header, sizeof(header));

	ASSERT(len == sizeof(header));

	if (header.shaderCount <= (uint32_t)macro)
	{
		delete rfile;
		ASSERT(false);
		return false;
	}

	//seek to block that correspond to macro
	for (uint32_t i = 0; i < (uint32_t)macro; ++i)
	{
		BlsBlockHeader blockHeader;
		len = rfile->read(&blockHeader, sizeof(blockHeader));

		ASSERT(len == sizeof(blockHeader));
		ASSERT(blockHeader.codeSize);

		bool ret = rfile->seek(blockHeader.constSize + blockHeader.samplerSize + blockHeader.textureSize + blockHeader.codeSize, true);
		if (!ret)
		{
			delete rfile;
			ASSERT(false);
			return false;
		}
	}

	BlsBlockHeader blockHeader;
	len = rfile->read(&blockHeader, sizeof(blockHeader));

	ASSERT(len == sizeof(blockHeader));
	ASSERT(blockHeader.codeSize);

	//const
	uint8_t* constBuffer = (uint8_t*)Z_AllocateTempMemory(blockHeader.constSize);
	uint8_t* codeBuffer = (uint8_t*)Z_AllocateTempMemory(blockHeader.codeSize);

	len = rfile->read(constBuffer, blockHeader.constSize);
	ASSERT(len == blockHeader.constSize);
	len = rfile->read(codeBuffer, blockHeader.codeSize);
	ASSERT(len == blockHeader.codeSize);

	delete rfile;

	//code
	HRESULT hr = Device->CreatePixelShader((DWORD*)codeBuffer, &pixelShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadPShaderHLSL Failed : CreateVertexShader");
		ASSERT(false);

		Z_FreeTempMemory(codeBuffer);
		Z_FreeTempMemory(constBuffer);
		return false;
	}
	Z_FreeTempMemory(codeBuffer);

	CD3D9PixelShader* shader = new CD3D9PixelShader(type, macro, pixelShader, callback);
	if (blockHeader.constSize)
	{
		CD3D9VertexShader::T_ConstList& constList = shader->getConstList();

		const uint8_t* p = constBuffer;
		while (p < constBuffer + blockHeader.constSize)
		{
			BlsConst* constEntry = (BlsConst*)p;
			SDx9ConstDesc desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;
			desc.size = constEntry->size;
			constList.emplace_back(desc);

			p += (sizeof(BlsConst) -1);
			p+= (strlen((const char*)p)+1);
		}
		ASSERT(p == constBuffer + blockHeader.constSize);
		constList.sort();
	}

	Z_FreeTempMemory(constBuffer);

	ASSERT(PixelShaders[type][macro] == nullptr);
	PixelShaders[type][macro] = shader;

	return true;
}

#else
/*
bool CD3D9ShaderServices::loadVShader( const char* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	char16_t	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	HRESULT hr = CD3D9Helper::d3dxAssembleShaderFromFileW( absFileNameW, nullptr, nullptr, dwShaderFlags, &pCode, nullptr );
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadVShader Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	IDirect3DVertexShader9* vertexShader;
	hr = Device->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &vertexShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadVShader Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	ASSERT(VertexShaders[type] == nullptr);
	VertexShaders[type] = new CD3D9VertexShader(type, vertexShader, callback);

	return true;
}

bool CD3D9ShaderServices::loadPShader( const char* filename, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	char16_t	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif


	LPD3DXBUFFER pCode;
	HRESULT hr = CD3D9Helper::d3dxAssembleShaderFromFileW( absFileNameW, nullptr, nullptr, dwShaderFlags, &pCode, nullptr );
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadPShader Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	IDirect3DPixelShader9* pixelShader;
	hr = Device->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &pixelShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadPShader Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	ASSERT(PixelShaders[type] == nullptr);
	PixelShaders[type] = new CD3D9PixelShader(type, pixelShader, callback);

	return true;
}

bool CD3D9ShaderServices::loadVShaderHLSL( const char* filename, const char* entry, const char* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	char16_t	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	LPD3DXCONSTANTTABLE constantsTable;
	HRESULT hr = CD3D9Helper::d3dxCompileShaderFromFileW( absFileNameW, nullptr, nullptr, entry, profile, dwShaderFlags, &pCode, nullptr, &constantsTable );

	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadVShaderHLSL Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	IDirect3DVertexShader9* vertexShader;
	hr = Device->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &vertexShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadVShaderHLSL Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}


	CD3D9VertexShader* shader = new CD3D9VertexShader(type, vertexShader, callback);
	if (constantsTable)
	{
		CD3D9VertexShader::T_ConstMap& constMap = shader->getConstMap();
		constMap.clear();

		D3DXCONSTANTTABLE_DESC table_desc;
		constantsTable->GetDesc(&table_desc);
		for (uint32_t i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = constantsTable->GetConstant(nullptr, i);
			if (handle)
			{
				constantsTable->GetConstantDesc(handle, &desc, nullptr);
				SDx9ConstDesc d;
				d.index = desc.RegisterIndex;
				d.size = desc.Bytes;
				constMap[desc.Name] = d;
			}
		}
	}

	SAFE_RELEASE(pCode);
	SAFE_RELEASE(constantsTable);

	VertexShaderMap[type] = shader;

	return true;
}

bool CD3D9ShaderServices::loadPShaderHLSL( const char* filename, const char* entry, const char* profile, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	char16_t	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	LPD3DXCONSTANTTABLE constantsTable;
	HRESULT hr = CD3D9Helper::d3dxCompileShaderFromFileW( absFileNameW, nullptr, nullptr, entry, profile, dwShaderFlags, &pCode, nullptr, &constantsTable );

	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	IDirect3DPixelShader9* pixelShader;
	hr = Device->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &pixelShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9ShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	CD3D9PixelShader* shader = new CD3D9PixelShader(type, pixelShader, callback);
	if (constantsTable)
	{
		CD3D9PixelShader::T_ConstMap& constMap = shader->getConstMap();
		constMap.clear();

		D3DXCONSTANTTABLE_DESC table_desc;
		constantsTable->GetDesc(&table_desc);
		for (uint32_t i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = constantsTable->GetConstant(nullptr, i);
			if (handle)
			{
				constantsTable->GetConstantDesc(handle, &desc, nullptr);
				SDx9ConstDesc d;
				d.index = desc.RegisterIndex;
				d.size = desc.Bytes;
				constMap[desc.Name] = d;
			}
		}
	}
	SAFE_RELEASE(pCode);
	SAFE_RELEASE(constantsTable);

	PixelShaderMap[type] = shader;

	return true;
}
*/
#endif

void CD3D9ShaderServices::applyShaders()
{
	if (LastShaderState.vshader != ShaderState.vshader)
	{
		CD3D9VertexShader* vs = static_cast<CD3D9VertexShader*>(ShaderState.vshader);
		if (vs)	
		{
			Device->SetVertexShader(vs->getDXVShader());
		}
		else
		{
#ifndef FIXPIPELINE
			ASSERT(false);
#endif
			Device->SetVertexShader(nullptr);
		}
	}

	if (LastShaderState.pshader != ShaderState.pshader)
	{
		CD3D9PixelShader* ps = static_cast<CD3D9PixelShader*>(ShaderState.pshader);
		if (ps)	
		{
			Device->SetPixelShader(ps->getDXPShader());
		}
		else
		{
#ifndef FIXPIPELINE
			ASSERT(false);
#endif
			Device->SetPixelShader(nullptr);
		}
	}

	LastShaderState = ShaderState;
}

void CD3D9ShaderServices::setShaderConstants( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	ASSERT(vs);
	if (vs && vs->ShaderConstCallback)
		vs->ShaderConstCallback(vs, material, pass);
}

void CD3D9ShaderServices::setShaderConstants( IPixelShader* ps, const SMaterial& material, uint32_t pass)
{
	ASSERT(ps);
	if (ps && ps->ShaderConstCallback)
		ps->ShaderConstCallback(ps, material, pass);
}

IVertexShader* CD3D9ShaderServices::getDefaultVertexShader( E_VERTEX_TYPE vType ) const
{
	switch (vType)
	{
	case EVT_P:
		return getVertexShader(EVST_DEFAULT_P);
	case EVT_PC:
		return getVertexShader(EVST_DEFAULT_PC);
	case EVT_PCT:
		return getVertexShader(EVST_DEFAULT_PCT);
	case EVT_PN:
		return getVertexShader(EVST_DEFAULT_PN);
	case EVT_PNC:
		return getVertexShader(EVST_DEFAULT_PNC);
	case EVT_PNCT:
		return getVertexShader(EVST_DEFAULT_PNCT);
	case EVT_PNCT2:
		return getVertexShader(EVST_DEFAULT_PNCT2);
	case EVT_PNT:
		return getVertexShader(EVST_DEFAULT_PNT);
	case EVT_PNT2W:
		return getVertexShader(EVST_DIFFUSE_T1);
	case EVT_PT:
		return getVertexShader(EVST_DEFAULT_PT);
	default:
		return nullptr;
	}
}

IPixelShader* CD3D9ShaderServices::getDefaultPixelShader( E_VERTEX_TYPE vType, E_PS_MACRO macro )
{
	switch (vType)
	{
	case EVT_P:
		return getPixelShader(EPST_DEFAULT_P, macro);
	case EVT_PC:
		return getPixelShader(EPST_DEFAULT_PC, macro);
	case EVT_PCT:
		return getPixelShader(EPST_DEFAULT_PCT, macro);
	case EVT_PN:
		return getPixelShader(EPST_DEFAULT_PN, macro);
	case EVT_PNC:
		return getPixelShader(EPST_DEFAULT_PNC, macro);
	case EVT_PNCT:
		return getPixelShader(EPST_DEFAULT_PNCT, macro);
	case EVT_PNCT2:
		return getPixelShader(EPST_DEFAULT_PNCT2, macro);
	case EVT_PNT:
	case EVT_PNT2W:
		return getPixelShader(EPST_DEFAULT_PNT, macro);
	case EVT_PT:
		return getPixelShader(EPST_DEFAULT_PT, macro);
	default:
		return nullptr;
	}
}


#endif