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

	Driver->registerLostReset(this);

	LastShaderState.reset();
	ShaderState.reset();
	ResetShaders = true;
}

CD3D9ShaderServices::~CD3D9ShaderServices()
{
	Driver->removeLostReset(this);

	for (u32 i=0; i < EVST_COUNT; ++i)
	{
		delete VertexShaders[i];
	}

	for (u32 i=0; i < EPST_COUNT; ++i)
	{
		for (u32 k=PS_Macro_None; k<PS_Macro_Num; ++k)
			delete PixelShaders[i][k];
	}
}

void CD3D9ShaderServices::loadAll()
{
	const c8* profile = "";

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

void CD3D9ShaderServices::onLost()
{
}

void CD3D9ShaderServices::onReset()
{
	ResetShaders = true;
}

#ifdef DIRECTX_USE_COMPILED_SHADER

bool CD3D9ShaderServices::loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	ASSERT(false);
	return false;
}

bool CD3D9ShaderServices::loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	ASSERT(false);
	return false;
}

bool CD3D9ShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
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

	IDirect3DVertexShader9* vertexShader = NULL_PTR;
	u32 len = 0;

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
	u8* constBuffer = (u8*)Z_AllocateTempMemory(blockHeader.constSize);
	u8* codeBuffer = (u8*)Z_AllocateTempMemory(blockHeader.codeSize);

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

		const u8* p = constBuffer;
		while (p < constBuffer + blockHeader.constSize)
		{
			BlsConst* constEntry = (BlsConst*)p;
			SDx9ConstDesc desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;
			desc.size = constEntry->size;
			constList.emplace_back(desc);

			p += (sizeof(BlsConst) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == constBuffer + blockHeader.constSize);
		constList.sort();
	}
	Z_FreeTempMemory(constBuffer);

	ASSERT(VertexShaders[type] == NULL_PTR);
	VertexShaders[type] = shader;
	
	return true;
}

bool CD3D9ShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
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

	IDirect3DPixelShader9* pixelShader = NULL_PTR;
	u32 len = 0;

	//read bls file
	BlsHeader header;
	len = rfile->read(&header, sizeof(header));

	ASSERT(len == sizeof(header));

	if (header.shaderCount <= (u32)macro)
	{
		delete rfile;
		ASSERT(false);
		return false;
	}

	//seek to block that correspond to macro
	for (u32 i = 0; i < (u32)macro; ++i)
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
	u8* constBuffer = (u8*)Z_AllocateTempMemory(blockHeader.constSize);
	u8* codeBuffer = (u8*)Z_AllocateTempMemory(blockHeader.codeSize);

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

		const u8* p = constBuffer;
		while (p < constBuffer + blockHeader.constSize)
		{
			BlsConst* constEntry = (BlsConst*)p;
			SDx9ConstDesc desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;
			desc.size = constEntry->size;
			constList.emplace_back(desc);

			p += (sizeof(BlsConst) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == constBuffer + blockHeader.constSize);
		constList.sort();
	}

	Z_FreeTempMemory(constBuffer);

	ASSERT(PixelShaders[type][macro] == NULL_PTR);
	PixelShaders[type][macro] = shader;

	return true;
}

#else
/*
bool CD3D9ShaderServices::loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	HRESULT hr = CD3D9Helper::d3dxAssembleShaderFromFileW( absFileNameW, NULL_PTR, NULL_PTR, dwShaderFlags, &pCode, NULL_PTR );
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

	ASSERT(VertexShaders[type] == NULL_PTR);
	VertexShaders[type] = new CD3D9VertexShader(type, vertexShader, callback);

	return true;
}

bool CD3D9ShaderServices::loadPShader( const c8* filename, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif


	LPD3DXBUFFER pCode;
	HRESULT hr = CD3D9Helper::d3dxAssembleShaderFromFileW( absFileNameW, NULL_PTR, NULL_PTR, dwShaderFlags, &pCode, NULL_PTR );
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

	ASSERT(PixelShaders[type] == NULL_PTR);
	PixelShaders[type] = new CD3D9PixelShader(type, pixelShader, callback);

	return true;
}

bool CD3D9ShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	LPD3DXCONSTANTTABLE constantsTable;
	HRESULT hr = CD3D9Helper::d3dxCompileShaderFromFileW( absFileNameW, NULL_PTR, NULL_PTR, entry, profile, dwShaderFlags, &pCode, NULL_PTR, &constantsTable );

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
		for (u32 i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = constantsTable->GetConstant(NULL_PTR, i);
			if (handle)
			{
				constantsTable->GetConstantDesc(handle, &desc, NULL_PTR);
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

bool CD3D9ShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	LPD3DXCONSTANTTABLE constantsTable;
	HRESULT hr = CD3D9Helper::d3dxCompileShaderFromFileW( absFileNameW, NULL_PTR, NULL_PTR, entry, profile, dwShaderFlags, &pCode, NULL_PTR, &constantsTable );

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
		for (u32 i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = constantsTable->GetConstant(NULL_PTR, i);
			if (handle)
			{
				constantsTable->GetConstantDesc(handle, &desc, NULL_PTR);
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
	if (ResetShaders || LastShaderState.vshader != ShaderState.vshader)
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
			Device->SetVertexShader(NULL_PTR);
		}
	}

	if (ResetShaders || LastShaderState.pshader != ShaderState.pshader)
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
			Device->SetPixelShader(NULL_PTR);
		}
	}

	LastShaderState = ShaderState;
	ResetShaders = false;
}

void CD3D9ShaderServices::setShaderConstants( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	ASSERT(vs);
	if (vs && vs->ShaderConstCallback)
		vs->ShaderConstCallback(vs, material, pass);
}

void CD3D9ShaderServices::setShaderConstants( IPixelShader* ps, const SMaterial& material, u32 pass)
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
		return NULL_PTR;
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
		return NULL_PTR;
	}
}


#endif