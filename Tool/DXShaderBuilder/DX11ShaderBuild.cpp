#include "DX11ShaderBuild.h"
#include "mywow.h"
#include "DXShaderBuilder.h"

//D3D11 Shader Include

HRESULT CD3D11ShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, 
	LPCSTR pFileName,
	LPCVOID pParentData,
	LPCVOID* ppData,
	UINT* pBytes)
{
	string256 absfilename = CurrentDir;
	absfilename.append(pFileName);

	IReadFile* rFile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rFile)
	{
		*ppData = NULL;
		*pBytes = 0;

		_ASSERT(false);
		return S_FALSE;
	}

	char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
	rFile->seek(0);
	uint32_t len = rFile->read(buffer, rFile->getSize());
	delete rFile;

	*ppData = (LPCVOID)buffer;
	*pBytes = len;

	return S_OK;
}

HRESULT CD3D11ShaderInclude::Close(LPCVOID pData)
{
	void* buffer = (void*)pData;
	if (buffer)
	{
		Z_FreeTempMemory(buffer);
	}

	return S_OK;
}

void buildShaders_40( bool vs, bool ps, bool gs )
{
	const char* basedir = g_BaseDir.c_str();

	printf("4_0.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex/vs_4_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11VS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel/ps_4_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11PS, (void*)dirname.c_str());
	}

	if (gs)
	{
		string256 dirname = basedir;
		dirname.append("Geometry/gs_4_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11GS, (void*)dirname.c_str());
	}

	printf("\n\n");
}

void buildShaders_50( bool vs, bool ps, bool gs, bool ds, bool hs )
{
	const char* basedir = g_BaseDir.c_str();

	printf("5_0.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex/vs_5_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11VS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel/ps_5_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11PS, (void*)dirname.c_str());
	}

	if (gs)
	{
		string256 dirname = basedir;
		dirname.append("Geometry/gs_5_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11GS, (void*)dirname.c_str());
	}

	if (ds)
	{
		string256 dirname = basedir;
		dirname.append("Domain/ds_5_0");
		//Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11,(void*)dirname.c_str());
	}

	if (hs)
	{
		string256 dirname = basedir;
		dirname.append("Hull/hs_5_0");
		//Q_iterateFiles(dirname.c_str(), "*.fx", funcShader11, (void*)dirname.c_str());
	}

	printf("\n\n");
}

void funcShader11VS( const char* filename, void* args )
{
	if (!args)
		return;

	const char* dirname = (const char*)args;
	char profile[8];
	getFileNameA(dirname, profile, 8);

	char tmp[2];
	tmp[0] = profile[strlen(profile)-3];
	tmp[1] = 0;
	uint32_t major = (uint32_t)atoi(tmp); 
	tmp[0] = profile[strlen(profile)-1];
	uint32_t minor = (uint32_t)atoi(tmp);
	char sign = profile[0];
	sign = _toupper(sign);

	string256 absfilename = filename;
	printf("compiling: %s\n", absfilename.c_str());

	//read shader file
	IReadFile* rFile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rFile)
	{
		printf("\n");
		printf("open shader file failed!!! %s\n", absfilename.c_str());
		++nFailed;
		return;
	}

	char name[MAX_PATH];
	getFullFileNameNoExtensionA(filename, name, MAX_PATH);
	string256 outfilename = name;
	outfilename.append(".bls");
	IWriteFile* wfile = g_fs->createAndWriteFile(outfilename.c_str(), true);
	if (!wfile)
	{
		delete rFile;
		printf("\n");
		printf("open shader file failed!!! %s\n", outfilename.c_str());
		++nFailed;
		return;
	}

	//header
	BlsHeader header;
	header._magic[0] = 'S';
	header._magic[1] =  sign;
	header._magic[2] = 'X';
	header._magic[3] = 'G';
	header._major = major;
	header._minor = minor;
	header.shaderCount = 1;

	wfile->write(&header, sizeof(header));

	//make macro
	std::vector<D3D_SHADER_MACRO>	shaderMacros;

	D3D_SHADER_MACRO macroEnd;
	macroEnd.Name = NULL;
	macroEnd.Definition = NULL;
	shaderMacros.push_back(macroEnd);

	char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
	rFile->seek(0);
	uint32_t len = rFile->read(buffer, rFile->getSize());

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* pBlobOut = NULL;
	ID3DBlob* pErrorBlob;
	CD3D11ShaderInclude shaderInclude(dirname);
	HRESULT hr =D3DX11CompileFromMemory(buffer, len, NULL, shaderMacros.data(), &shaderInclude, "main", profile,
		dwShaderFlags, 0, NULL, &pBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		printf("\n");
		printf("compile failed!!! %s\n", absfilename.c_str());
		++nFailed;

		if (pErrorBlob && pErrorBlob->GetBufferSize() > 0)
		{
			const char* error = (const char*)pErrorBlob->GetBufferPointer();
			printf("%s\n", error);
		}

		SAFE_RELEASE(pBlobOut);
		SAFE_RELEASE(pErrorBlob);
		return;
	}

	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect( pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), 
		IID_ID3D11ShaderReflection, (void**) &pReflector);

	uint32_t constSize = 0;
	uint32_t samplerSize = 0;
	uint32_t textureSize = 0;
	uint8_t* constBuffer = NULL;
	uint8_t* samplerBuffer = NULL;
	uint8_t* textureBuffer = NULL;

	if (!pReflector)
	{
		_ASSERT(false);
		printf("compile failed!!! %s cannot get reflector\n", absfilename.c_str());
		return;
	}

	D3D11_SHADER_DESC desc;
	pReflector->GetDesc(&desc);

	for (uint32_t i=0; i<desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		switch (resourceDesc.Type)
		{
		case D3D_SIT_CBUFFER:
		case D3D_SIT_TBUFFER:
			constSize += ((sizeof(BlsConst) - 1)+ strlen(resourceDesc.Name) + 1);		//index, size, tbuffer, len+1
			break;
		case D3D_SIT_SAMPLER:
			samplerSize += ((sizeof(BlsSampler) - 1) + strlen(resourceDesc.Name) + 1);
			break;
		case D3D_SIT_TEXTURE:
			textureSize += ((sizeof(BlsTexture) - 1) + strlen(resourceDesc.Name) + 1);		//index, len+1
			break;
		}
	}

	//block header
	BlsBlockHeader blockHeader;
	blockHeader.constSize = constSize;
	blockHeader.samplerSize = samplerSize ;
	blockHeader.textureSize = textureSize;
	blockHeader.codeSize = pBlobOut->GetBufferSize();
	wfile->write(&blockHeader, sizeof(BlsBlockHeader));

	//const buffer
	constBuffer = (uint8_t*)Z_AllocateTempMemory(constSize);
	uint8_t* pointer = constBuffer;
	for (uint32_t i=0; i<desc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pConstant = pReflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pConstant->GetDesc(&bufferDesc);

		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDescByName(bufferDesc.Name, &resourceDesc);

		uint32_t v;
		//index
		v = resourceDesc.BindPoint;
		memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
		pointer += sizeof(uint32_t);
		//size
		v = bufferDesc.Size;
		memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
		pointer += sizeof(uint32_t);
		//tbuffer
		v = bufferDesc.Type == D3D_SIT_TBUFFER ? 1 : 0;
		memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
		pointer += sizeof(uint32_t);
		//name
		uint32_t s = strlen(bufferDesc.Name) + 1;
		memcpy_s(pointer, s, bufferDesc.Name, s);
		pointer += s;
	}
	_ASSERT(pointer == constBuffer + constSize);
	if (constSize)
	{
		_ASSERT(constBuffer);
		wfile->write(constBuffer, constSize);
	}
	Z_FreeTempMemory(constBuffer);

	//sampler buffer
	samplerBuffer = (uint8_t*)Z_AllocateTempMemory(samplerSize);
	pointer = (uint8_t*)samplerBuffer;
	for (uint32_t i=0; i<desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		if (resourceDesc.Type == D3D_SIT_SAMPLER)
		{
			uint32_t v;
			//index
			v = resourceDesc.BindPoint;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);

			//name
			uint32_t s = strlen(resourceDesc.Name) + 1;
			memcpy_s(pointer, s, resourceDesc.Name, s);
			pointer += s;
		}
	}
	_ASSERT(pointer == samplerBuffer + samplerSize);
	if (samplerSize)
	{
		_ASSERT(samplerBuffer);
		wfile->write(samplerBuffer, samplerSize);
	}
	Z_FreeTempMemory(samplerBuffer);

	//texture buffer
	textureBuffer = (uint8_t*)Z_AllocateTempMemory(textureSize);
	pointer = (uint8_t*)textureBuffer;
	for (uint32_t i=0; i<desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		if (resourceDesc.Type == D3D_SIT_TEXTURE)
		{
			uint32_t v;
			//index
			v = resourceDesc.BindPoint;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);

			//name
			uint32_t s = strlen(resourceDesc.Name) + 1;
			memcpy_s(pointer, s, resourceDesc.Name, s);
			pointer += s;
		}
	}
	_ASSERT(pointer == textureBuffer + textureSize);
	if (textureSize)
	{
		_ASSERT(textureBuffer);
		wfile->write(textureBuffer, textureSize);
	}
	Z_FreeTempMemory(textureBuffer);

	//code
	if (pBlobOut->GetBufferSize())
	{
		wfile->write(pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize());
	}

	SAFE_RELEASE(pReflector);
	SAFE_RELEASE(pBlobOut);
	SAFE_RELEASE(pErrorBlob);

	++nSucceed;

	delete rFile;
	delete wfile;
}

void funcShader11PS( const char* filename, void* args )
{
	if (!args)
		return;

	const char* dirname = (const char*)args;
	char profile[8];
	getFileNameA(dirname, profile, 8);

	char tmp[2];
	tmp[0] = profile[strlen(profile)-3];
	tmp[1] = 0;
	uint32_t major = (uint32_t)atoi(tmp); 
	tmp[0] = profile[strlen(profile)-1];
	uint32_t minor = (uint32_t)atoi(tmp);
	char sign = profile[0];
	sign = _toupper(sign);

	string256 absfilename = filename;

	//read shader file
	IReadFile* rFile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rFile)
	{
		printf("\n");
		printf("open shader file failed!!! %s\n", absfilename.c_str());
		++nFailed;
		return;
	}

	char name[MAX_PATH];
	getFullFileNameNoExtensionA(filename, name, MAX_PATH);
	string256 outfilename = name;
	outfilename.append(".bls");
	IWriteFile* wfile = g_fs->createAndWriteFile(outfilename.c_str(), true);
	if (!wfile)
	{
		delete rFile;
		printf("\n");
		printf("open shader file failed!!! %s\n", outfilename.c_str());
		++nFailed;
		return;
	}

	//header
	BlsHeader header;
	header._magic[0] = 'S';
	header._magic[1] =  sign;
	header._magic[2] = 'X';
	header._magic[3] = 'G';
	header._major = major;
	header._minor = minor;
	header.shaderCount = PS_Macro_Num;

	wfile->write(&header, sizeof(header));

	for (uint32_t i=PS_Macro_None; i<PS_Macro_Num; ++i)
	{
		//make macro
		std::vector<D3D_SHADER_MACRO>	shaderMacros;
		std::vector<string128> macroStrings;
		const char* strMacro = getPSMacroString((E_PS_MACRO)i);
		makeMacroStringList(macroStrings, strMacro);

		printf("compiling: %s, macro: %s\n", absfilename.c_str(), strMacro);

		for (uint32_t i=0; i<(uint32_t)macroStrings.size(); ++i)
		{
			D3D_SHADER_MACRO macro;
			macro.Name = macroStrings[i].c_str();
			macro.Definition = "1";
			shaderMacros.push_back(macro);
		}

		D3D_SHADER_MACRO macroEnd;
		macroEnd.Name = NULL;
		macroEnd.Definition = NULL;
		shaderMacros.push_back(macroEnd);

		char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
		rFile->seek(0);
		uint32_t len = rFile->read(buffer, rFile->getSize());

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
		dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		ID3DBlob* pBlobOut = NULL;
		ID3DBlob* pErrorBlob;
		CD3D11ShaderInclude shaderInclude(dirname);
		HRESULT hr =D3DX11CompileFromMemory(buffer, len, NULL, shaderMacros.data(), &shaderInclude, "main", profile,
			dwShaderFlags, 0, NULL, &pBlobOut, &pErrorBlob, NULL);
		if (FAILED(hr))
		{
			printf("\n");
			printf("compile failed!!! %s\n", absfilename.c_str());
			++nFailed;

			if (pErrorBlob && pErrorBlob->GetBufferSize() > 0)
			{
				const char* error = (const char*)pErrorBlob->GetBufferPointer();
				printf("%s\n", error);
			}

			SAFE_RELEASE(pBlobOut);
			SAFE_RELEASE(pErrorBlob);
			return;
		}

		ID3D11ShaderReflection* pReflector = NULL; 
		D3DReflect( pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), 
			IID_ID3D11ShaderReflection, (void**) &pReflector);

		uint32_t constSize = 0;
		uint32_t samplerSize = 0;
		uint32_t textureSize = 0;
		uint8_t* constBuffer = NULL;
		uint8_t* samplerBuffer = NULL;
		uint8_t* textureBuffer = NULL;

		if (!pReflector)
		{
			_ASSERT(false);
			printf("compile failed!!! %s cannot get reflector\n", absfilename.c_str());
			return;
		}

		D3D11_SHADER_DESC desc;
		pReflector->GetDesc(&desc);

		for (uint32_t i=0; i<desc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
			pReflector->GetResourceBindingDesc(i, &resourceDesc);
			switch (resourceDesc.Type)
			{
			case D3D_SIT_CBUFFER:
			case D3D_SIT_TBUFFER:
				constSize += ((sizeof(BlsConst) - 1)+ strlen(resourceDesc.Name) + 1);		//index, size, tbuffer, len+1
				break;
			case D3D_SIT_SAMPLER:
				samplerSize += ((sizeof(BlsSampler) - 1) + strlen(resourceDesc.Name) + 1);
				break;
			case D3D_SIT_TEXTURE:
				textureSize += ((sizeof(BlsTexture) - 1) + strlen(resourceDesc.Name) + 1);		//index, len+1
				break;
			}
		}

		//block header
		BlsBlockHeader blockHeader;
		blockHeader.constSize = constSize;
		blockHeader.samplerSize = samplerSize ;
		blockHeader.textureSize = textureSize;
		blockHeader.codeSize = pBlobOut->GetBufferSize();
		wfile->write(&blockHeader, sizeof(BlsBlockHeader));

		//const buffer
		constBuffer = (uint8_t*)Z_AllocateTempMemory(constSize);
		uint8_t* pointer = constBuffer;
		for (uint32_t i=0; i<desc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstant = pReflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC bufferDesc;
			pConstant->GetDesc(&bufferDesc);

			D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
			pReflector->GetResourceBindingDescByName(bufferDesc.Name, &resourceDesc);

			uint32_t v;
			//index
			v = resourceDesc.BindPoint;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);
			//size
			v = bufferDesc.Size;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);
			//tbuffer
			v = bufferDesc.Type == D3D_SIT_TBUFFER ? 1 : 0;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);
			//name
			uint32_t s = strlen(bufferDesc.Name) + 1;
			memcpy_s(pointer, s, bufferDesc.Name, s);
			pointer += s;
		}
		_ASSERT(pointer == constBuffer + constSize);
		if (constSize)
		{
			_ASSERT(constBuffer);
			wfile->write(constBuffer, constSize);
		}
		Z_FreeTempMemory(constBuffer);

		//sampler buffer
		samplerBuffer = (uint8_t*)Z_AllocateTempMemory(samplerSize);
		pointer = (uint8_t*)samplerBuffer;
		for (uint32_t i=0; i<desc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
			pReflector->GetResourceBindingDesc(i, &resourceDesc);
			if (resourceDesc.Type == D3D_SIT_SAMPLER)
			{
				uint32_t v;
				//index
				v = resourceDesc.BindPoint;
				memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
				pointer += sizeof(uint32_t);

				//name
				uint32_t s = strlen(resourceDesc.Name) + 1;
				memcpy_s(pointer, s, resourceDesc.Name, s);
				pointer += s;
			}
		}
		_ASSERT(pointer == samplerBuffer + samplerSize);
		if (samplerSize)
		{
			_ASSERT(samplerBuffer);
			wfile->write(samplerBuffer, samplerSize);
		}
		Z_FreeTempMemory(samplerBuffer);

		//texture buffer
		textureBuffer = (uint8_t*)Z_AllocateTempMemory(textureSize);
		pointer = (uint8_t*)textureBuffer;
		for (uint32_t i=0; i<desc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
			pReflector->GetResourceBindingDesc(i, &resourceDesc);
			if (resourceDesc.Type == D3D_SIT_TEXTURE)
			{
				uint32_t v;
				//index
				v = resourceDesc.BindPoint;
				memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
				pointer += sizeof(uint32_t);

				//name
				uint32_t s = strlen(resourceDesc.Name) + 1;
				memcpy_s(pointer, s, resourceDesc.Name, s);
				pointer += s;
			}
		}
		_ASSERT(pointer == textureBuffer + textureSize);
		if (textureSize)
		{
			_ASSERT(textureBuffer);
			wfile->write(textureBuffer, textureSize);
		}
		Z_FreeTempMemory(textureBuffer);

		//code
		if (pBlobOut->GetBufferSize())
		{
			wfile->write(pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize());
		}

		SAFE_RELEASE(pReflector);
		SAFE_RELEASE(pBlobOut);
		SAFE_RELEASE(pErrorBlob);

		++nSucceed;
	}

	delete rFile;
	delete wfile;
}

void funcShader11GS( const char* filename, void* args )
{
	if (!args)
		return;

	const char* dirname = (const char*)args;
	char profile[8];
	getFileNameA(dirname, profile, 8);

	char tmp[2];
	tmp[0] = profile[strlen(profile)-3];
	tmp[1] = 0;
	uint32_t major = (uint32_t)atoi(tmp); 
	tmp[0] = profile[strlen(profile)-1];
	uint32_t minor = (uint32_t)atoi(tmp);
	char sign = profile[0];
	sign = _toupper(sign);

	string256 absfilename = filename;
	printf("compiling: %s\n", absfilename.c_str());

	//read shader file
	IReadFile* rFile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rFile)
	{
		printf("\n");
		printf("open shader file failed!!! %s\n", absfilename.c_str());
		++nFailed;
		return;
	}

	char name[MAX_PATH];
	getFullFileNameNoExtensionA(filename, name, MAX_PATH);
	string256 outfilename = name;
	outfilename.append(".bls");
	IWriteFile* wfile = g_fs->createAndWriteFile(outfilename.c_str(), true);
	if (!wfile)
	{
		delete rFile;
		printf("\n");
		printf("open shader file failed!!! %s\n", outfilename.c_str());
		++nFailed;
		return;
	}

	//header
	BlsHeader header;
	header._magic[0] = 'S';
	header._magic[1] =  sign;
	header._magic[2] = 'X';
	header._magic[3] = 'G';
	header._major = major;
	header._minor = minor;
	header.shaderCount = 1;

	wfile->write(&header, sizeof(header));

	//make macro
	std::vector<D3D_SHADER_MACRO>	shaderMacros;

	D3D_SHADER_MACRO macroEnd;
	macroEnd.Name = NULL;
	macroEnd.Definition = NULL;
	shaderMacros.push_back(macroEnd);

	char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
	rFile->seek(0);
	uint32_t len = rFile->read(buffer, rFile->getSize());

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* pBlobOut = NULL;
	ID3DBlob* pErrorBlob;
	CD3D11ShaderInclude shaderInclude(dirname);
	HRESULT hr =D3DX11CompileFromMemory(buffer, len, NULL, shaderMacros.data(), &shaderInclude, "main", profile,
		dwShaderFlags, 0, NULL, &pBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		printf("\n");
		printf("compile failed!!! %s\n", absfilename.c_str());
		++nFailed;

		if (pErrorBlob && pErrorBlob->GetBufferSize() > 0)
		{
			const char* error = (const char*)pErrorBlob->GetBufferPointer();
			printf("%s\n", error);
		}

		SAFE_RELEASE(pBlobOut);
		SAFE_RELEASE(pErrorBlob);
		return;
	}

	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect( pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize(), 
		IID_ID3D11ShaderReflection, (void**) &pReflector);

	uint32_t constSize = 0;
	uint32_t samplerSize = 0;
	uint32_t textureSize = 0;
	uint8_t* constBuffer = NULL;
	uint8_t* samplerBuffer = NULL;
	uint8_t* textureBuffer = NULL;

	if (!pReflector)
	{
		_ASSERT(false);
		printf("compile failed!!! %s cannot get reflector\n", absfilename.c_str());
		return;
	}

	D3D11_SHADER_DESC desc;
	pReflector->GetDesc(&desc);

	for (uint32_t i=0; i<desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		switch (resourceDesc.Type)
		{
		case D3D_SIT_CBUFFER:
		case D3D_SIT_TBUFFER:
			constSize += ((sizeof(BlsConst) - 1)+ strlen(resourceDesc.Name) + 1);		//index, size, tbuffer, len+1
			break;
		case D3D_SIT_SAMPLER:
			samplerSize += ((sizeof(BlsSampler) - 1) + strlen(resourceDesc.Name) + 1);
			break;
		case D3D_SIT_TEXTURE:
			textureSize += ((sizeof(BlsTexture) - 1) + strlen(resourceDesc.Name) + 1);		//index, len+1
			break;
		}
	}

	//block header
	BlsBlockHeader blockHeader;
	blockHeader.constSize = constSize;
	blockHeader.samplerSize = samplerSize ;
	blockHeader.textureSize = textureSize;
	blockHeader.codeSize = pBlobOut->GetBufferSize();
	wfile->write(&blockHeader, sizeof(BlsBlockHeader));

	//const buffer
	constBuffer = (uint8_t*)Z_AllocateTempMemory(constSize);
	uint8_t* pointer = constBuffer;
	for (uint32_t i=0; i<desc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pConstant = pReflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pConstant->GetDesc(&bufferDesc);

		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDescByName(bufferDesc.Name, &resourceDesc);

		uint32_t v;
		//index
		v = resourceDesc.BindPoint;
		memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
		pointer += sizeof(uint32_t);
		//size
		v = bufferDesc.Size;
		memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
		pointer += sizeof(uint32_t);
		//tbuffer
		v = bufferDesc.Type == D3D_SIT_TBUFFER ? 1 : 0;
		memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
		pointer += sizeof(uint32_t);
		//name
		uint32_t s = strlen(bufferDesc.Name) + 1;
		memcpy_s(pointer, s, bufferDesc.Name, s);
		pointer += s;
	}
	_ASSERT(pointer == constBuffer + constSize);
	if (constSize)
	{
		_ASSERT(constBuffer);
		wfile->write(constBuffer, constSize);
	}
	Z_FreeTempMemory(constBuffer);

	//sampler buffer
	samplerBuffer = (uint8_t*)Z_AllocateTempMemory(samplerSize);
	pointer = (uint8_t*)samplerBuffer;
	for (uint32_t i=0; i<desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		if (resourceDesc.Type == D3D_SIT_SAMPLER)
		{
			uint32_t v;
			//index
			v = resourceDesc.BindPoint;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);

			//name
			uint32_t s = strlen(resourceDesc.Name) + 1;
			memcpy_s(pointer, s, resourceDesc.Name, s);
			pointer += s;
		}
	}
	_ASSERT(pointer == samplerBuffer + samplerSize);
	if (samplerSize)
	{
		_ASSERT(samplerBuffer);
		wfile->write(samplerBuffer, samplerSize);
	}
	Z_FreeTempMemory(samplerBuffer);

	//texture buffer
	textureBuffer = (uint8_t*)Z_AllocateTempMemory(textureSize);
	pointer = (uint8_t*)textureBuffer;
	for (uint32_t i=0; i<desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceDesc;
		pReflector->GetResourceBindingDesc(i, &resourceDesc);
		if (resourceDesc.Type == D3D_SIT_TEXTURE)
		{
			uint32_t v;
			//index
			v = resourceDesc.BindPoint;
			memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
			pointer += sizeof(uint32_t);

			//name
			uint32_t s = strlen(resourceDesc.Name) + 1;
			memcpy_s(pointer, s, resourceDesc.Name, s);
			pointer += s;
		}
	}
	_ASSERT(pointer == textureBuffer + textureSize);
	if (textureSize)
	{
		_ASSERT(textureBuffer);
		wfile->write(textureBuffer, textureSize);
	}
	Z_FreeTempMemory(textureBuffer);

	//code
	if (pBlobOut->GetBufferSize())
	{
		wfile->write(pBlobOut->GetBufferPointer(), pBlobOut->GetBufferSize());
	}

	SAFE_RELEASE(pReflector);
	SAFE_RELEASE(pBlobOut);
	SAFE_RELEASE(pErrorBlob);

	++nSucceed;

	delete rFile;
	delete wfile;
}

