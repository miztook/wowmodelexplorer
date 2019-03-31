#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IShaderServices.h"
#include <unordered_map>
#include <map>
#include <list>
#include "CD3D11_VS40.h"
#include "CD3D11_VS50.h"
#include "CD3D11_PS40.h"
#include "CD3D11_PS50.h"

class CD3D11VertexShader;
class CD3D11PixelShader;
class CD3D11Driver;
struct SDx11ConstDesc;

class CD3D11ShaderServices : public IShaderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11ShaderServices);

public:
	CD3D11ShaderServices();
	~CD3D11ShaderServices();

public:
	virtual void loadAll();

	virtual IPixelShader* getPixelShader(E_PS_TYPE type, E_PS_MACRO macro = PS_Macro_None) { return PixelShaders[type][macro]; }

public:
	bool loadVShader( const char* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShader( const char* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	bool loadVShaderHLSL( const char* filename, const char* entry, const char* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShaderHLSL( const char* filename, const char* entry, const char* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	void useVertexShader(IVertexShader* vshader) { ShaderState.vshader = vshader; }
	void usePixelShader(IPixelShader* pshader) { ShaderState.pshader = pshader; }
	void applyShaders();
	void setShaderConstants(IVertexShader* vs, const SMaterial& material, uint32_t pass);
	void setShaderConstants(IPixelShader* ps, const SMaterial& material, uint32_t pass);

public:
	IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const;
	IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, E_PS_MACRO macro = PS_Macro_None);

private:
	struct SConstantBuffer
	{
		uint32_t size;
		ID3D11Buffer*	constBuffer;
		ID3D11ShaderResourceView*	shaderResourceView;		//for tbuffer
		bool used;		//for shader assignment
	};

	SConstantBuffer* findAvailableConstantBuffer(const SDx11ConstDesc* desc); 

	void buildConstantBuffers();
	void releaseConstantBuffers();
	void buildTextureBuffers();
	void releaseTextureBuffers();

	void assignBuffersToShaders();

	void markConstantBufferUsed(SConstantBuffer* buffer);
	void markAllConstantBufferUnused();

	typedef std::list<SConstantBuffer, qzone_allocator<SConstantBuffer> >	T_ConstanBufferList;

	struct SShaderState
	{
		void reset()
		{
			vshader = nullptr;
			pshader = nullptr;
		}

		IVertexShader*		vshader;
		IPixelShader*		pshader;
	};

	SShaderState ShaderState;
	SShaderState LastShaderState;

	CD3D11_VS40 vs40Loader;
	CD3D11_VS50 vs50Loader;
	CD3D11_PS40 ps40Loader;
	CD3D11_PS50 ps50Loader;

	bool ResetShaders;

private:
	ID3D11Device*		Device;
	CD3D11Driver*		Driver;

#ifdef USE_QALLOCATOR
	typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>>		T_SizeMap;
#else
	typedef std::unordered_map<uint32_t, uint32_t>		T_SizeMap;
#endif


	T_ConstanBufferList	CBufferList;
	T_ConstanBufferList	TBufferList;
};

#endif