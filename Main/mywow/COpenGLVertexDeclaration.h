#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "base.h"
#include "gl/GL.h"
#include "function.h"
#include <map>
#include <unordered_map>

class COpenGLMaterialRenderServices;
class COpenGLShaderServices;
class IVertexBuffer;
class COpenGLExtension;
struct SGLProgram;

class COpenGLVertexDeclaration
{
public:
	explicit COpenGLVertexDeclaration(E_VERTEX_TYPE vtype);
	~COpenGLVertexDeclaration();

public:
	void apply(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1);

	void deleteVao(IVertexBuffer* vbuffer0);

private:
	struct SVAOParam
	{
		SGLProgram* program;
		IVertexBuffer* vbuffer0;
		IVertexBuffer* vbuffer1;
		u32 offset0;
		u32 offset1;

		bool operator<(const SVAOParam& other) const
		{
			if(program != other.program)
				return program < other.program;
			else if (vbuffer0 != other.vbuffer0)
				return vbuffer0 < other.vbuffer0;
			else if (vbuffer1 != other.vbuffer1)
				return vbuffer1 < other.vbuffer1;
			else if (offset0 != other.offset0)
				return offset0 < other.offset0;
			else if (offset1 != other.offset1)
				return offset1 < other.offset1;

			return false;
		}

		bool operator==(const SVAOParam& other) const
		{
			return program == other.program &&
				vbuffer0 == other.vbuffer0 &&
				offset0 == other.offset0 &&
				vbuffer1 == other.vbuffer1 &&
				offset1 == other.offset1;
		}

	};

	struct SVAOParam_hash
	{
		size_t operator()(const SVAOParam& _Keyval) const
		{
			return (size_t)CRC32_BlockChecksum(&_Keyval, sizeof(_Keyval));
		}
	};

private:
	GLuint createVao(const SVAOParam& param);
	GLuint	 getVao(const SVAOParam& param);

private:
	void createVao_P(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PN(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PNC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PNT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PNCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PNCT2(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createVao_PNT2W(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1);

private:	
	void setDecl_P(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PN(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PNC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PNT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PNCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PNCT2(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void setDecl_PNT2W(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1);

#ifdef USE_QALLOCATOR
	typedef std::map<SVAOParam, GLuint, std::less<SVAOParam>, qzone_allocator<std::pair<SVAOParam, GLuint>>>	T_VaoMap;
#else
	typedef std::unordered_map<SVAOParam, GLuint, SVAOParam_hash>	T_VaoMap;
#endif

private:
	E_VERTEX_TYPE		VertexType;
	COpenGLMaterialRenderServices*		MaterialRenderServices;
	COpenGLShaderServices*		ShaderServices;
	COpenGLExtension*		Extension;

	T_VaoMap		VaoMap;
};


#endif