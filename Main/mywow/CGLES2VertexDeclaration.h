#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "base.h"
#include <map>

#ifdef MW_PLATFORM_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif

class CGLES2ShaderServices;
class IVertexBuffer;
class CGLES2Extension;
struct SGLProgram;

class CGLES2VertexDeclaration
{
public:
	explicit CGLES2VertexDeclaration(E_VERTEX_TYPE vtype);
	~CGLES2VertexDeclaration();

public:
	void apply(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1);

	void deleteVao(IVertexBuffer* vbuffer0);

private:
	struct SVParam
	{
		SGLProgram* program;
		IVertexBuffer* vbuffer0;
		IVertexBuffer* vbuffer1;
		u32 offset0;
		u32 offset1;

		bool operator<(const SVParam& other) const
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

	};

private:
	GLuint createVao(const SVParam& param);
	GLuint	 getVao(const SVParam& param);

private:
	void createDecl_P(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PN(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PNC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PNT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PNCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PNCT2(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0);
	void createDecl_PNT2W(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1);

private:
	E_VERTEX_TYPE		VertexType;
	CGLES2ShaderServices*		ShaderServices;
	CGLES2Extension*		Extension;

	typedef std::map<SVParam, GLuint, std::less<SVParam>, qzone_allocator<std::pair<SVParam, GLuint> > >	T_VaoMap;
	T_VaoMap		VaoMap;

};

#endif