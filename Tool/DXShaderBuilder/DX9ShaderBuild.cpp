#include "DX9ShaderBuild.h"
#include "mywow.h"
#include "DXShaderBuilder.h"

//D3D9 Shader Include
HRESULT CD3D9ShaderInclude::Open(D3DXINCLUDE_TYPE Type,
	LPCSTR Name,
	LPCVOID ParentData,
	LPCVOID* Data,
	UINT* Bytes)
{
	string256 absfilename = CurrentDir;
	absfilename.append(Name);

	IReadFile* rFile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rFile)
	{
		*Data = NULL;
		*Bytes = 0;

		_ASSERT(false);
		return S_FALSE;
	}

	char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
	rFile->seek(0);
	uint32_t len = rFile->read(buffer, rFile->getSize());
	delete rFile;

	*Data = (LPCVOID)buffer;
	*Bytes = len;

	return S_OK;
}

HRESULT CD3D9ShaderInclude::Close(LPCVOID Data)
{
	void* buffer = (void*)Data;
	if (buffer)
	{
		Z_FreeTempMemory(buffer);
	}
	return S_OK;
}


void buildShaders_20( bool vs, bool ps )
{
	const char* basedir = g_BaseDir.c_str();

	printf("2_0.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex/vs_2_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader9VS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel/ps_2_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader9PS, (void*)dirname.c_str());
	}

	printf("\n\n");
}

void buildShaders_30( bool vs, bool ps )
{
	const char* basedir = g_BaseDir.c_str();

	printf("3_0.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex/vs_3_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader9VS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel/ps_3_0");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShader9PS, (void*)dirname.c_str());
	}

	printf("\n\n");
}

void funcShader9VS(const char* filename, void* args)
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

	//make macro
	std::vector<D3DXMACRO>	shaderMacros;

	D3DXMACRO macroEnd;
	macroEnd.Name = NULL;
	macroEnd.Definition = NULL;
	shaderMacros.push_back(macroEnd);

	char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
	rFile->seek(0);
	uint32_t len = rFile->read(buffer, rFile->getSize());
	delete rFile;

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pErrorMsg = NULL;
	LPD3DXCONSTANTTABLE constantsTable = NULL;
	CD3D9ShaderInclude shaderInclude(dirname);
	HRESULT hr = D3DXCompileShader( buffer, len, shaderMacros.data(), &shaderInclude, "main", profile, dwShaderFlags, &pCode, &pErrorMsg, &constantsTable );

	Z_FreeTempMemory(buffer);

	if (FAILED(hr))
	{
		printf("\n");
		printf("compile failed!!! %s\n", absfilename.c_str());
		++nFailed;

		if (pErrorMsg && pErrorMsg->GetBufferSize() > 0)
		{
			const char* error = (const char*)pErrorMsg->GetBufferPointer();
			printf("%s\n", error);
		}

		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsg);
		SAFE_RELEASE(constantsTable);

		return;
	}

	uint32_t constSize = 0;
	uint8_t* constBuffer = NULL;
	if (constantsTable)
	{
		D3DXCONSTANTTABLE_DESC table_desc;
		constantsTable->GetDesc(&table_desc);
		for (uint32_t i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = constantsTable->GetConstant(NULL, i);
			if (handle)
			{
				constantsTable->GetConstantDesc(handle, &desc, NULL);
				constSize += ((sizeof(BlsConst) - 1)+ (uint32_t)strlen(desc.Name) + 1);
			}
		}

		constBuffer = (uint8_t*)Z_AllocateTempMemory(constSize);
		uint8_t* pointer = constBuffer;
		for (uint32_t i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = constantsTable->GetConstant(NULL, i);
			if (handle)
			{
				constantsTable->GetConstantDesc(handle, &desc, NULL);
				uint32_t v;
				v = desc.RegisterIndex;
				//index
				memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
				pointer += sizeof(uint32_t);
				//size
				v = desc.Bytes;
				memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
				pointer += sizeof(uint32_t);
				//
				v = 0;
				memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
				pointer += sizeof(uint32_t);
				//name
				uint32_t s = (uint32_t)strlen(desc.Name) + 1;
				memcpy_s(pointer, s, desc.Name, s);
				pointer += s;
			}
		}
		_ASSERT(pointer == constBuffer + constSize);
	}

	BlsHeader header;
	header._magic[0] = 'S';
	header._magic[1] =  sign;
	header._magic[2] = 'X';
	header._magic[3] = 'G';
	header._major = major;
	header._minor = minor;
	header.shaderCount = 1;

	char name[MAX_PATH];
	getFullFileNameNoExtensionA(filename, name, MAX_PATH);
	string256 outfilename = name;
	outfilename.append(".bls");
	IWriteFile* wfile = g_fs->createAndWriteFile(outfilename.c_str(), true);

	wfile->write(&header, sizeof(header));

	//block
	BlsBlockHeader blockHeader;				//dx9中没有sampler和texture变量，都是constant
	blockHeader.constSize = constSize;
	blockHeader.samplerSize = 0 ;
	blockHeader.textureSize = 0;
	blockHeader.codeSize = pCode->GetBufferSize();
	wfile->write(&blockHeader, sizeof(BlsBlockHeader));

	if (constSize)
	{
		_ASSERT(constBuffer);
		wfile->write(constBuffer, constSize);
		Z_FreeTempMemory(constBuffer);
	}

	if (pCode->GetBufferSize())
	{
		wfile->write(pCode->GetBufferPointer(), pCode->GetBufferSize());
	}

	delete wfile;

	SAFE_RELEASE(pCode);
	SAFE_RELEASE(pErrorMsg);
	SAFE_RELEASE(constantsTable);

	++nSucceed;
}

void funcShader9PS(const char* filename, void* args)
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
		std::vector<D3DXMACRO>	shaderMacros;
		std::vector<string128> macroStrings;
		const char* strMacro = getPSMacroString((E_PS_MACRO)i);
		makeMacroStringList(macroStrings, strMacro);

		printf("compiling: %s, macro: %s\n", absfilename.c_str(), strMacro);

		for (uint32_t i=0; i<(uint32_t)macroStrings.size(); ++i)
		{
			D3DXMACRO macro;
			macro.Name = macroStrings[i].c_str();
			macro.Definition = "1";
			shaderMacros.push_back(macro);
		}
			
		D3DXMACRO macroEnd;
		macroEnd.Name = NULL;
		macroEnd.Definition = NULL;
		shaderMacros.push_back(macroEnd);

		char* buffer = (char*)Z_AllocateTempMemory(rFile->getSize());
		rFile->seek(0);
		uint32_t len = rFile->read(buffer, rFile->getSize());

		DWORD dwShaderFlags;
	#ifdef _DEBUG
		dwShaderFlags = D3DXSHADER_DEBUG;
	#else
		dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
	#endif

		LPD3DXBUFFER pCode = NULL;
		LPD3DXBUFFER pErrorMsg = NULL;
		LPD3DXCONSTANTTABLE constantsTable = NULL;
		CD3D9ShaderInclude shaderInclude(dirname);
		HRESULT hr = D3DXCompileShader( buffer, len, shaderMacros.data(), &shaderInclude, "main", profile, dwShaderFlags, &pCode, &pErrorMsg, &constantsTable );

		Z_FreeTempMemory(buffer);

		if (FAILED(hr))
		{
			printf("\n");
			printf("compile failed!!! %s\n", absfilename.c_str());
			++nFailed;

			if (pErrorMsg && pErrorMsg->GetBufferSize() > 0)
			{
				const char* error = (const char*)pErrorMsg->GetBufferPointer();
				printf("%s\n", error);
			}

			SAFE_RELEASE(pCode);
			SAFE_RELEASE(pErrorMsg);
			SAFE_RELEASE(constantsTable);

			return;
		}

		uint32_t constSize = 0;
		uint8_t* constBuffer = NULL;
		if (constantsTable)
		{
			D3DXCONSTANTTABLE_DESC table_desc;
			constantsTable->GetDesc(&table_desc);
			for (uint32_t i=0; i<table_desc.Constants; ++i)
			{
				D3DXCONSTANT_DESC desc;
				D3DXHANDLE handle = constantsTable->GetConstant(NULL, i);
				if (handle)
				{
					constantsTable->GetConstantDesc(handle, &desc, NULL);
					constSize += ((sizeof(BlsConst) - 1)+ (uint32_t)strlen(desc.Name) + 1);
				}
			}

			constBuffer = (uint8_t*)Z_AllocateTempMemory(constSize);
			uint8_t* pointer = constBuffer;
			for (uint32_t i=0; i<table_desc.Constants; ++i)
			{
				D3DXCONSTANT_DESC desc;
				D3DXHANDLE handle = constantsTable->GetConstant(NULL, i);
				if (handle)
				{
					constantsTable->GetConstantDesc(handle, &desc, NULL);
					uint32_t v;
					v = desc.RegisterIndex;
					//index
					memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
					pointer += sizeof(uint32_t);
					//size
					v = desc.Bytes;
					memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
					pointer += sizeof(uint32_t);
					//
					v = 0;
					memcpy_s(pointer, sizeof(uint32_t), &v, sizeof(uint32_t));
					pointer += sizeof(uint32_t);
					//name
					uint32_t s = (uint32_t)strlen(desc.Name) + 1;
					memcpy_s(pointer, s, desc.Name, s);
					pointer += s;
				}
			}
			_ASSERT(pointer == constBuffer + constSize);
		}

		//block
		BlsBlockHeader blockHeader;				//dx9中没有sampler和texture变量，都是constant
		blockHeader.constSize = constSize;
		blockHeader.samplerSize = 0 ;
		blockHeader.textureSize = 0;
		blockHeader.codeSize = pCode->GetBufferSize();
		wfile->write(&blockHeader, sizeof(BlsBlockHeader));

		if (constSize)
		{
			_ASSERT(constBuffer);
			wfile->write(constBuffer, constSize);
			Z_FreeTempMemory(constBuffer);
		}

		if (pCode->GetBufferSize())
		{
			wfile->write(pCode->GetBufferPointer(), pCode->GetBufferSize());
		}

		SAFE_RELEASE(pCode);
		SAFE_RELEASE(pErrorMsg);
		SAFE_RELEASE(constantsTable);

		++nSucceed;
	}		
	
	delete rFile;
	delete wfile;
}