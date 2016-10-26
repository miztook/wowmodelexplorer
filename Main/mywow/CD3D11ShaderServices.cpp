#include "stdafx.h"
#include "CD3D11ShaderServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"
#include "CD3D11Shader.h"
#include "CD3D11Helper.h"

CD3D11ShaderServices::CD3D11ShaderServices()
{
	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	Device = Driver->pID3DDevice11;
	Driver->registerLostReset(this);

	LastShaderState.reset();
	ShaderState.reset();
	ResetShaders = true;
}

CD3D11ShaderServices::~CD3D11ShaderServices()
{
	Driver->removeLostReset(this);
	
	releaseTextureBuffers();

	releaseConstantBuffers();

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

void CD3D11ShaderServices::loadAll()
{
	const c8* profile = "";

	//vs
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	if (Driver->queryFeature(EVDF_VERTEX_SHADER_5_0))
	{
		profile = "vs_5_0";
		vs50Loader.loadAll(this);
	}
	else if (Driver->queryFeature(EVDF_VERTEX_SHADER_4_0))
	{
		profile = "vs_4_0";
		vs40Loader.loadAll(this);
	}
	else 
	{
		ASSERT(false);
		profile = "";
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);

	//ps
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	if (Driver->queryFeature(EVDF_PIXEL_SHADER_5_0))
	{
		profile = "ps_5_0";
		ps50Loader.loadAll(this);
	}
	else 
	if (Driver->queryFeature(EVDF_PIXEL_SHADER_4_0))
	{
		profile = "ps_4_0";
		ps40Loader.loadAll(this);
	}
	else
	{
		ASSERT(false);
		profile = "";
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);

	buildConstantBuffers();
	buildTextureBuffers();

	assignBuffersToShaders();
}

void CD3D11ShaderServices::onLost()
{
}

void CD3D11ShaderServices::onReset()
{
	ResetShaders = true;
}

bool CD3D11ShaderServices::loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	ASSERT(false);
	return false;
}

bool CD3D11ShaderServices::loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	ASSERT(false);
	return false;
}

#ifdef DIRECTX_USE_COMPILED_SHADER

bool CD3D11ShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), true);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadVShaderHLSL Failed: Bls Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	ID3D11VertexShader* vertexShader;
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

	//read
	u8* constBuffer = (u8*)Z_AllocateTempMemory(blockHeader.constSize);
	u8* samplerBuffer = (u8*)Z_AllocateTempMemory(blockHeader.samplerSize);
	u8* textureBuffer = (u8*)Z_AllocateTempMemory(blockHeader.textureSize);
	u8* codeBuffer = (u8*)Z_AllocateTempMemory(blockHeader.codeSize);

	len = rfile->read(constBuffer, blockHeader.constSize);
	ASSERT(len == blockHeader.constSize);
	len = rfile->read(samplerBuffer, blockHeader.samplerSize);
	ASSERT(len == blockHeader.samplerSize);
	len = rfile->read(textureBuffer, blockHeader.textureSize);
	ASSERT(len == blockHeader.textureSize);
	len = rfile->read(codeBuffer, blockHeader.codeSize);
	ASSERT(len == blockHeader.codeSize);

	delete rfile;

	//create
	HRESULT hr = Device->CreateVertexShader((const void*)codeBuffer, blockHeader.codeSize, NULL_PTR, &vertexShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadVShaderHLSL Failed : CreateVertexShader");
		ASSERT(false);

		Z_FreeTempMemory(codeBuffer);
		Z_FreeTempMemory(textureBuffer);
		Z_FreeTempMemory(samplerBuffer);
		Z_FreeTempMemory(constBuffer);
		return false;
	}

	CD3D11VertexShader* shader = new CD3D11VertexShader(type, vertexShader, callback, (const void*)codeBuffer, blockHeader.codeSize);

	//const buffer
	if (blockHeader.constSize)
	{
		CD3D11VertexShader::T_BufferList& cbufferList = shader->getCBufferList();
		CD3D11VertexShader::T_BufferList& tbufferList = shader->getTBufferList();

		const u8* p = constBuffer;
		while (p < constBuffer + blockHeader.constSize)
		{
			BlsConst* constEntry = (BlsConst*)p;
			SDx11ConstDesc desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;
			desc.size = constEntry->size;
			desc.tbuffer = constEntry->tbuffer != 0;

			if (desc.tbuffer)
				tbufferList.emplace_back(desc);
			else
				cbufferList.emplace_back(desc);

			p += (sizeof(BlsConst) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == constBuffer + blockHeader.constSize);
		cbufferList.sort();
		tbufferList.sort();
	}

	//sampler buffer
	if (blockHeader.samplerSize)
	{
		CD3D11VertexShader::T_SamplerList& samplerList = shader->getSamplerList();

		const u8* p = samplerBuffer;
		while (p < samplerBuffer + blockHeader.samplerSize)
		{
			BlsSampler* constEntry = (BlsSampler*)p;
			SSamplerDesc11 desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;

			samplerList.emplace_back(desc);

			p += (sizeof(BlsSampler) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == samplerBuffer + blockHeader.samplerSize);
		samplerList.sort();
	}

	//texture buffer
	if (blockHeader.textureSize)
	{
		CD3D11VertexShader::T_TextureList& textureList = shader->getTextureList();

		const u8* p = textureBuffer;
		while (p < textureBuffer + blockHeader.textureSize)
		{
			BlsTexture* constEntry = (BlsTexture*)p;
			STextureDesc11 desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;

			textureList.emplace_back(desc);

			p += (sizeof(BlsTexture) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == textureBuffer + blockHeader.textureSize);
		textureList.sort();
	}

	ASSERT(VertexShaders[type] == NULL_PTR);
	VertexShaders[type] = shader;

	Z_FreeTempMemory(codeBuffer);
	Z_FreeTempMemory(textureBuffer);
	Z_FreeTempMemory(samplerBuffer);
	Z_FreeTempMemory(constBuffer);

	return true;
}

bool CD3D11ShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), true);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadPShaderHLSL Failed: Bls Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	ID3D11PixelShader* pixelShader;
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

	//read
	u8* constBuffer = (u8*)Z_AllocateTempMemory(blockHeader.constSize);
	u8* samplerBuffer = (u8*)Z_AllocateTempMemory(blockHeader.samplerSize);
	u8* textureBuffer = (u8*)Z_AllocateTempMemory(blockHeader.textureSize);
	u8* codeBuffer = (u8*)Z_AllocateTempMemory(blockHeader.codeSize);

	len = rfile->read(constBuffer, blockHeader.constSize);
	ASSERT(len == blockHeader.constSize);
	len = rfile->read(samplerBuffer, blockHeader.samplerSize);
	ASSERT(len == blockHeader.samplerSize);
	len = rfile->read(textureBuffer, blockHeader.textureSize);
	ASSERT(len == blockHeader.textureSize);
	len = rfile->read(codeBuffer, blockHeader.codeSize);
	ASSERT(len == blockHeader.codeSize);

	delete rfile;

	//create
	HRESULT hr = Device->CreatePixelShader((const void*)codeBuffer, blockHeader.codeSize, NULL_PTR, &pixelShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);

		Z_FreeTempMemory(codeBuffer);
		Z_FreeTempMemory(textureBuffer);
		Z_FreeTempMemory(samplerBuffer);
		Z_FreeTempMemory(constBuffer);
		return false;
	}

	CD3D11PixelShader* shader = new CD3D11PixelShader(type, macro, pixelShader, callback);

	//const buffer
	if (blockHeader.constSize)
	{
		CD3D11PixelShader::T_BufferList& cbufferList = shader->getCBufferList();
		CD3D11PixelShader::T_BufferList& tbufferList = shader->getTBufferList();

		const u8* p = constBuffer;
		while (p < constBuffer + blockHeader.constSize)
		{
			BlsConst* constEntry = (BlsConst*)p;
			SDx11ConstDesc desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;
			desc.size = constEntry->size;
			desc.tbuffer = constEntry->tbuffer != 0;

			if (desc.tbuffer)
				tbufferList.emplace_back(desc);
			else
				cbufferList.emplace_back(desc);

			p += (sizeof(BlsConst) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == constBuffer + blockHeader.constSize);
		cbufferList.sort();
		tbufferList.sort();
	}

	//sampler buffer
	if (blockHeader.samplerSize)
	{
		CD3D11PixelShader::T_SamplerList& samplerList = shader->getSamplerList();

		const u8* p = samplerBuffer;
		while (p < samplerBuffer + blockHeader.samplerSize)
		{
			BlsSampler* constEntry = (BlsSampler*)p;
			SSamplerDesc11 desc;
			Q_strcpy(desc.name, 32, constEntry->name);
			desc.index = constEntry->index;

			samplerList.emplace_back(desc);

			p += (sizeof(BlsSampler) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == samplerBuffer + blockHeader.samplerSize);
		samplerList.sort();
	}

	//texture buffer
	if (blockHeader.textureSize)
	{
		CD3D11PixelShader::T_TextureList& textureList = shader->getTextureList();

		const u8* p = textureBuffer;
		while (p < textureBuffer + blockHeader.textureSize)
		{
			BlsTexture* texEntry = (BlsTexture*)p;
			STextureDesc11 desc;
			Q_strcpy(desc.name, 32, texEntry->name);
			desc.index = texEntry->index;

			textureList.emplace_back(desc);

			p += (sizeof(BlsTexture) -1);
			p+= (strlen((const c8*)p)+1);
		}
		ASSERT(p == textureBuffer + blockHeader.textureSize);
		textureList.sort();
	}

	ASSERT(PixelShaders[type][macro] == NULL_PTR);
	PixelShaders[type][macro] = shader;

	Z_FreeTempMemory(codeBuffer);
	Z_FreeTempMemory(textureBuffer);
	Z_FreeTempMemory(samplerBuffer);
	Z_FreeTempMemory(constBuffer);

	return true;
}

#else

/*
bool CD3D11ShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* pBlobOut = NULL_PTR;
	ID3DBlob* pErrorBlob;
	HRESULT hr = CD3D11Helper::d3dx11CompileFromFileW(absFileNameW, NULL_PTR, NULL_PTR, entry, profile,
		dwShaderFlags, 0, NULL_PTR, &pBlobOut, &pErrorBlob, NULL_PTR);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			pErrorBlob->Release();
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadVShaderHLSL Failed");
		ASSERT(false);
		return false;
	}

	if (pErrorBlob)
		pErrorBlob->Release();

	ID3D11VertexShader* vertexShader;
	hr = Device->CreateVertexShader(pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), NULL_PTR, &vertexShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadVShaderHLSL Failed");
		ASSERT(false);
		SAFE_RELEASE_STRICT(pBlobOut);
		return false;
	}

	CD3D11VertexShader* shader = new CD3D11VertexShader(type, vertexShader, callback, pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize());

	//const buffer
	ID3D11ShaderReflection* pReflector = NULL_PTR; 
	CD3D11Helper::d3dReflect( pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), 
		IID_ID3D11ShaderReflection, (void**) &pReflector);

	if (pReflector)
	{
		CD3D11VertexShader::T_ConstMap& constMap = shader->getConstMap();
		constMap.clear();

		D3D11_SHADER_DESC desc;
		pReflector->GetDesc(&desc);
		for (u32 i=0; i<desc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstant = pReflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC bufferDesc;
			pConstant->GetDesc(&bufferDesc);

			SDx11ConstDesc desc;
			desc.index = i;
			desc.size = bufferDesc.Size;
			constMap[bufferDesc.Name] = desc;
		}
		pReflector->Release();
	}

	VertexShaderMap[type] = shader;

	SAFE_RELEASE_STRICT(pBlobOut);
	return true;
}

bool CD3D11ShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* pBlobOut = NULL_PTR;
	ID3DBlob* pErrorBlob;
	HRESULT hr = CD3D11Helper::d3dx11CompileFromFileW(absFileNameW, NULL_PTR, NULL_PTR, entry, profile,
		dwShaderFlags, 0, NULL_PTR, &pBlobOut, &pErrorBlob, NULL_PTR);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			pErrorBlob->Release();
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);
		return false;
	}

	if (pErrorBlob)
		pErrorBlob->Release();

	ID3D11PixelShader* pixelShader;
	hr = Device->CreatePixelShader(pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), NULL_PTR, &pixelShader);
	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11ShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);
		SAFE_RELEASE_STRICT(pBlobOut);
		return false;
	}

	CD3D11PixelShader* shader = new CD3D11PixelShader(type, pixelShader, callback);

	//const buffer
	ID3D11ShaderReflection* pReflector = NULL_PTR; 
	CD3D11Helper::d3dReflect( pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), 
		IID_ID3D11ShaderReflection, (void**) &pReflector);

	if (pReflector)
	{
		CD3D11PixelShader::T_ConstMap& constMap = shader->getConstMap();
		constMap.clear();

		D3D11_SHADER_DESC desc;
		pReflector->GetDesc(&desc);
		for (u32 i=0; i<desc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstant = pReflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC bufferDesc;
			pConstant->GetDesc(&bufferDesc);
			SDx11ConstDesc desc;
			desc.index = i;
			desc.size = bufferDesc.Size;
			constMap[bufferDesc.Name] = desc;
		}
		pReflector->Release();
	}

	PixelShaderMap[type] = shader;

	SAFE_RELEASE_STRICT(pBlobOut);

	return true;
}
*/

#endif


void CD3D11ShaderServices::applyShaders()
{
	if (ResetShaders || LastShaderState.vshader != ShaderState.vshader)
	{
		CD3D11VertexShader* vs = static_cast<CD3D11VertexShader*>(ShaderState.vshader);
		if (vs)
		{
			vs->onShaderUsed();
			//ImmediateContext
			Driver->ImmediateContext->VSSetShader(vs->getDXVShader(), NULL_PTR, 0);
		}
		else
		{
			ASSERT(false);
			Driver->ImmediateContext->VSSetShader(NULL_PTR, NULL_PTR, 0);
		}
	}

	if (ResetShaders || LastShaderState.pshader != ShaderState.pshader)
	{
		CD3D11PixelShader* ps = static_cast<CD3D11PixelShader*>(ShaderState.pshader);
		if (ps)
		{
			ps->onShaderUsed();

			//ImmediateContext
			Driver->ImmediateContext->PSSetShader(ps->getDXPShader(), NULL_PTR, 0);
		}
		else
		{		
			ASSERT(false);
			Driver->ImmediateContext->PSSetShader(NULL_PTR, NULL_PTR, 0);
		}
	}

	LastShaderState = ShaderState;
	ResetShaders = false;
}

void CD3D11ShaderServices::setShaderConstants( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	ASSERT(vs);
	if (vs && vs->ShaderConstCallback)
		vs->ShaderConstCallback(vs, material, pass);
}

void CD3D11ShaderServices::setShaderConstants( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	ASSERT(ps);
	if (ps && ps->ShaderConstCallback)
		ps->ShaderConstCallback(ps, material, pass);
}

IVertexShader* CD3D11ShaderServices::getDefaultVertexShader( E_VERTEX_TYPE vType ) const 
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

IPixelShader* CD3D11ShaderServices::getDefaultPixelShader( E_VERTEX_TYPE vType, E_PS_MACRO macro)
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

CD3D11ShaderServices::SConstantBuffer* CD3D11ShaderServices::findAvailableConstantBuffer( const SDx11ConstDesc* desc )
{
	if (!desc->tbuffer)
	{
		for (T_ConstanBufferList::iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
		{
			if (itr->size == desc->size && !itr->used)
				return &(*itr);
		}
	}
	else
	{
		for (T_ConstanBufferList::iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
		{
			if (itr->size == desc->size && !itr->used)
				return &(*itr);
		}
	}
	return NULL_PTR;
}

void CD3D11ShaderServices::buildConstantBuffers()
{
	//统计每个shader中每种size的cb个数，建立这些cb，在不同的shader之间要尽可能公用

	T_SizeMap	totalSizeMap;

	for (u32 i=0; i<EVST_COUNT; ++i)
	{	
		T_SizeMap  shaderSizeMap;

		CD3D11VertexShader* vs = static_cast<CD3D11VertexShader*>(VertexShaders[i]);
		for (u32 idx=0; idx<vs->getConstantCount(); ++idx)
		{
			const SDx11ConstDesc* desc = vs->getConstantDesc(idx);
			if (desc->tbuffer)
				continue;

			T_SizeMap::iterator it = shaderSizeMap.find(desc->size);
			if (it == shaderSizeMap.end())
				shaderSizeMap[desc->size] = 1;
			else
				it->second += 1;
		}
		
		//添加到总的sizeMap中
		for (T_SizeMap::const_iterator itr=shaderSizeMap.begin(); itr != shaderSizeMap.end(); ++itr)
		{
			u32 size = itr->first;
			u32 count = itr->second;

			T_SizeMap::iterator it = totalSizeMap.find(size);
			if (it == totalSizeMap.end() || it->second < count)		//更新总的map计数
				totalSizeMap[size] = count;
		}
	}

	for (u32 i=0; i<EPST_COUNT; ++i)
	{	
		T_SizeMap  shaderSizeMap;
		for (u32 k=PS_Macro_None; k<PS_Macro_Num; ++k)
		{
			CD3D11PixelShader* ps = static_cast<CD3D11PixelShader*>(PixelShaders[i][k]);
			if (!ps)
				continue;
			for (u32 idx=0; idx<ps->getConstantCount(); ++idx)
			{
				const SDx11ConstDesc* desc = ps->getConstantDesc(idx);
				if (desc->tbuffer)
					continue;

				T_SizeMap::iterator it = shaderSizeMap.find(desc->size);
				if (it == shaderSizeMap.end())
					shaderSizeMap[desc->size] = 1;
				else
					it->second += 1;
			}
		}
		
		//添加到总的sizeMap中
		for (T_SizeMap::const_iterator itr=shaderSizeMap.begin(); itr != shaderSizeMap.end(); ++itr)
		{
			u32 size = itr->first;
			u32 count = itr->second;

			T_SizeMap::iterator it = totalSizeMap.find(size);
			if (it == totalSizeMap.end() || it->second < count)		//更新总的map计数
				totalSizeMap[size] = count;
		}
	}

	//创建constant buffers
	for (T_SizeMap::const_iterator itr = totalSizeMap.begin(); itr != totalSizeMap.end(); ++itr)
	{
		u32 size = itr->first;
		u32 count = itr->second;

		for (u32 i=0; i<count; ++i)
		{
			SConstantBuffer buffer;
			buffer.used = false;
			buffer.size = size;
			D3D11_BUFFER_DESC bufferDesc = {0};
			bufferDesc.ByteWidth = buffer.size;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			if(FAILED(Device->CreateBuffer(&bufferDesc, NULL_PTR, &buffer.constBuffer)))
			{
				ASSERT(false);
				buffer.constBuffer = NULL_PTR;
				buffer.shaderResourceView = NULL_PTR;
			}
			buffer.shaderResourceView = NULL_PTR;

			CBufferList.emplace_back(buffer);

			//log
			//g_Engine->getFileSystem()->writeLog(ELOG_GX, "cbuffer created, size = %d", buffer.size);
		}
	}
}

void CD3D11ShaderServices::releaseConstantBuffers()
{
	for (T_ConstanBufferList::const_iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
	{
		if((*itr).shaderResourceView)
			(*itr).shaderResourceView->Release();

		if((*itr).constBuffer)
			(*itr).constBuffer->Release();
	}
}

void CD3D11ShaderServices::buildTextureBuffers()
{
	//统计每个shader中每种size的cb个数，建立这些cb，在不同的shader之间要尽可能公用

	T_SizeMap	totalSizeMap;

	for (u32 i=0; i<EVST_COUNT; ++i)
	{	
		T_SizeMap  shaderSizeMap;
		
		CD3D11VertexShader* vs = static_cast<CD3D11VertexShader*>(VertexShaders[i]);
		for (u32 idx=0; idx<vs->getConstantCount(); ++idx)
		{
			const SDx11ConstDesc* desc = vs->getConstantDesc(idx);
			if (!desc->tbuffer)
				continue;

			T_SizeMap::iterator it = shaderSizeMap.find(desc->size);
			if (it == shaderSizeMap.end())
				shaderSizeMap[desc->size] = 1;
			else
				it->second += 1;
		}
		
		//添加到总的sizeMap中
		for (T_SizeMap::const_iterator itr=shaderSizeMap.begin(); itr != shaderSizeMap.end(); ++itr)
		{
			u32 size = itr->first;
			u32 count = itr->second;

			T_SizeMap::iterator it = totalSizeMap.find(size);
			if (it == totalSizeMap.end() || it->second < count)		//更新总的map计数
				totalSizeMap[size] = count;
		}
	}

	for (u32 i=0; i<EPST_COUNT; ++i)
	{		
		T_SizeMap  shaderSizeMap;
		
		for (u32 k=PS_Macro_None; k<PS_Macro_Num; ++k)
		{
			CD3D11PixelShader* ps = static_cast<CD3D11PixelShader*>(PixelShaders[i][k]);
			if(!ps)
				continue;
			for (u32 idx=0; idx<ps->getConstantCount(); ++idx)
			{
				const SDx11ConstDesc* desc = ps->getConstantDesc(idx);
				if (!desc->tbuffer)
					continue;

				T_SizeMap::iterator it = shaderSizeMap.find(desc->size);
				if (it == shaderSizeMap.end())
					shaderSizeMap[desc->size] = 1;
				else
					it->second += 1;
			}
		}

		//添加到总的sizeMap中
		for (T_SizeMap::const_iterator itr=shaderSizeMap.begin(); itr != shaderSizeMap.end(); ++itr)
		{
			u32 size = itr->first;
			u32 count = itr->second;

			T_SizeMap::iterator it = totalSizeMap.find(size);
			if (it == totalSizeMap.end() || it->second < count)		//更新总的map计数
				totalSizeMap[size] = count;
		}
	}

	//创建constant buffers
	for (T_SizeMap::const_iterator itr = totalSizeMap.begin(); itr != totalSizeMap.end(); ++itr)
	{
		u32 size = itr->first;
		u32 count = itr->second;

		for (u32 i=0; i<count; ++i)
		{
			SConstantBuffer buffer;
			buffer.size = size;
			buffer.used = false;
			D3D11_BUFFER_DESC bufferDesc = {0};
			bufferDesc.ByteWidth = buffer.size;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			if(FAILED(Device->CreateBuffer(&bufferDesc, NULL_PTR, &buffer.constBuffer)))
			{
				ASSERT(false);
				buffer.constBuffer = NULL_PTR;
				buffer.shaderResourceView = NULL_PTR;
			}
			buffer.shaderResourceView = NULL_PTR;

			if(buffer.constBuffer)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
				viewDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				viewDesc.Buffer.ElementOffset = 0;
				viewDesc.Buffer.ElementWidth = buffer.size / 16;

				if (FAILED(Driver->pID3DDevice11->CreateShaderResourceView(buffer.constBuffer, &viewDesc, &buffer.shaderResourceView)))
				{
					ASSERT(false);
					buffer.shaderResourceView = NULL_PTR;
				}
			}

			TBufferList.emplace_back(buffer);

			//log
			g_Engine->getFileSystem()->writeLog(ELOG_GX, "tbuffer created, size = %d", buffer.size);
		}
	}
}

void CD3D11ShaderServices::releaseTextureBuffers()
{
	for (T_ConstanBufferList::const_iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
	{
		if((*itr).shaderResourceView)
			(*itr).shaderResourceView->Release();

		if((*itr).constBuffer)
			(*itr).constBuffer->Release();
	}
}

void CD3D11ShaderServices::assignBuffersToShaders()
{
	for (u32 i=0; i<EVST_COUNT; ++i)
	{
		CD3D11VertexShader* vs = static_cast<CD3D11VertexShader*>(VertexShaders[i]);

		markAllConstantBufferUnused();

		for (u32 k=0; k<vs->getConstantCount(); ++k)
		{
			SDx11ConstDesc* desc = vs->getConstantDesc(k);

			SConstantBuffer* buffer = findAvailableConstantBuffer(desc);
			desc->constBuffer = buffer->constBuffer;
			desc->shaderResourceView = buffer->shaderResourceView;

			markConstantBufferUsed(buffer);
		}
	}

	for (u32 i=0; i<EPST_COUNT; ++i)
	{
		for (u32 macro=PS_Macro_None; macro<PS_Macro_Num; ++macro)
		{
			CD3D11PixelShader* ps = static_cast<CD3D11PixelShader*>(PixelShaders[i][macro]);
			if (!ps)
				continue;

			markAllConstantBufferUnused();

			for (u32 k=0; k<ps->getConstantCount(); ++k)
			{
				SDx11ConstDesc* desc = ps->getConstantDesc(k);

				SConstantBuffer* buffer = findAvailableConstantBuffer(desc);
				desc->constBuffer = buffer->constBuffer;
				desc->shaderResourceView = buffer->shaderResourceView;

				markConstantBufferUsed(buffer);
			}
		}
	}
}

void CD3D11ShaderServices::markConstantBufferUsed( SConstantBuffer* buffer )
{
	buffer->used = true;
}

void CD3D11ShaderServices::markAllConstantBufferUnused()
{
	for (T_ConstanBufferList::iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
	{
		itr->used = false;
	}

	for (T_ConstanBufferList::iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
	{
		itr->used = false;
	}
}

#endif