#include "stdafx.h"
#include "CGLES2VertexDeclaration.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "S3DVertex.h"
#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2ShaderServices.h"
#include "CGLES2Helper.h"
#include "CLock.h"

#define NAME_POS		"Pos"
#define NAME_NORMAL		"Normal"
#define NAME_COLOR0		"Col0"
#define NAME_TEX0	"Tex0"
#define NAME_TEX1	"Tex1"
#define NAME_WEIGHT		"BlendWeight"
#define NAME_BLENDINDICES		"BlendIndices"

#define buffer_offset CGLES2Helper::buffer_offset

CGLES2VertexDeclaration::CGLES2VertexDeclaration( E_VERTEX_TYPE vtype )
	: VertexType(vtype)
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	Extension = driver->getGLExtension();
	ShaderServices = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
}

CGLES2VertexDeclaration::~CGLES2VertexDeclaration()
{
	ASSERT(VaoMap.empty());
	for (T_VaoMap::const_iterator itr = VaoMap.begin(); itr != VaoMap.end(); ++itr)
	{
		Extension->extGlDeleteVertexArrays(1, &itr->second);
	}
	VaoMap.clear();
}

void CGLES2VertexDeclaration::apply(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1)
{
	if (Extension->canUseVAO())
	{
		SVParam param;
		param.program = const_cast<SGLProgram*>(program);
		param.vbuffer0 = vbuffer0;
		param.offset0 = offset0;
		param.vbuffer1 = vbuffer1;
		param.offset1 = offset1;

		GLuint vao = getVao(param);
		Extension->extGlBindVertexArray(vao);
	}
	else
	{
		switch(VertexType)
		{
		case EVT_P:
			createDecl_P(program, vbuffer0, offset0);
			break;
		case EVT_PC:
			createDecl_PC(program, vbuffer0, offset0);
			break;
		case EVT_PCT:
			createDecl_PCT(program, vbuffer0, offset0);
			break;
		case EVT_PN:
			createDecl_PN(program, vbuffer0, offset0);
			break;
		case EVT_PNC:
			createDecl_PNC(program, vbuffer0, offset0);
			break;
		case EVT_PNT:
			createDecl_PNT(program, vbuffer0, offset0);
			break;
		case EVT_PT:
			createDecl_PT(program, vbuffer0, offset0);
			break;
		case EVT_PNCT:
			createDecl_PNCT(program, vbuffer0, offset0);
			break;
		case EVT_PNCT2:
			createDecl_PNCT2(program, vbuffer0, offset0);
			break;
		case EVT_PNT2W:
			createDecl_PNT2W(program, vbuffer0, offset0, vbuffer1, offset1);
			break;
		default:
			ASSERT(false);
		}
	}
}

GLuint CGLES2VertexDeclaration::createVao( const SVParam& param )
{
	GLuint vao;
	Extension->extGlGenVertexArrays(1, &vao);

	Extension->extGlBindVertexArray(vao);

	switch(VertexType)
	{
	case EVT_P:
		createDecl_P(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PC:
		createDecl_PC(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PCT:
		createDecl_PCT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PN:
		createDecl_PC(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNC:
		createDecl_PNC(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNT:
		createDecl_PNT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PT:
		createDecl_PT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNCT:
		createDecl_PNCT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNCT2:
		createDecl_PNCT2(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNT2W:
		createDecl_PNT2W(param.program, param.vbuffer0, param.offset0, param.vbuffer1, param.offset1);
		break;
	default:
		ASSERT(false);
		break;
	}

	Extension->extGlBindVertexArray(0);

	return vao;
}

GLuint CGLES2VertexDeclaration::getVao( const SVParam& param )
{
	CLock lock(&g_Globals.vaoCS);

	T_VaoMap::iterator itr = VaoMap.find(param);
	if (itr == VaoMap.end())
	{
		GLuint vao = createVao(param);
		VaoMap[param] = vao;
		return vao;
	}
	return itr->second;
}

void CGLES2VertexDeclaration::createDecl_P( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_P), buffer_offset(sizeof(SVertex_P) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PC( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PC), buffer_offset(sizeof(SVertex_PC) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(colorIndex);
		Extension->extGlVertexAttribPointer(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PC), buffer_offset(12 + sizeof(SVertex_PC) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);

}

void CGLES2VertexDeclaration::createDecl_PCT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PCT), buffer_offset(sizeof(SVertex_PC) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(colorIndex);
		Extension->extGlVertexAttribPointer(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PCT), buffer_offset(12 + sizeof(SVertex_PC) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex0Index);
		Extension->extGlVertexAttribPointer(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PCT), buffer_offset(16 + sizeof(SVertex_PCT) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PN( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PN), buffer_offset(sizeof(SVertex_PN) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(normalIndex);
		Extension->extGlVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PN), buffer_offset(12 + sizeof(SVertex_PN) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PNC( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNC), buffer_offset(sizeof(SVertex_PNC) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(normalIndex);
		Extension->extGlVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNC), buffer_offset(12 + sizeof(SVertex_PNC) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(colorIndex);
		Extension->extGlVertexAttribPointer(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNC), buffer_offset(24 + sizeof(SVertex_PNC) * offset0));
	}		

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PNT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT), buffer_offset(sizeof(SVertex_PNT) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(normalIndex);
		Extension->extGlVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT), buffer_offset(12 + sizeof(SVertex_PNT) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex0Index);
		Extension->extGlVertexAttribPointer(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT), buffer_offset(24 + sizeof(SVertex_PNT) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PT), buffer_offset(sizeof(SVertex_PT) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex0Index);
		Extension->extGlVertexAttribPointer(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PT), buffer_offset(12 + sizeof(SVertex_PT) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PNCT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT), buffer_offset(sizeof(SVertex_PNCT) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(normalIndex);
		Extension->extGlVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT), buffer_offset(12 + sizeof(SVertex_PNCT) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(colorIndex);
		Extension->extGlVertexAttribPointer(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNCT), buffer_offset(24 + sizeof(SVertex_PNCT) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex0Index);
		Extension->extGlVertexAttribPointer(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT), buffer_offset(28 + sizeof(SVertex_PNCT) * offset0));
	}	

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PNCT2( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(sizeof(SVertex_PNCT2) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(normalIndex);
		Extension->extGlVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(12 + sizeof(SVertex_PNCT2) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(colorIndex);
		Extension->extGlVertexAttribPointer(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNCT2), buffer_offset(24 + sizeof(SVertex_PNCT2) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex0Index);
		Extension->extGlVertexAttribPointer(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(28 + sizeof(SVertex_PNCT2) * offset0));
	}

	//tex1
	s32 tex1Index = ShaderServices->getAttribLocation(program, NAME_TEX1);
	if (tex1Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex1Index);
		Extension->extGlVertexAttribPointer(tex1Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(36 + sizeof(SVertex_PNCT2) * offset0));
	}	

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::createDecl_PNT2W( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);
	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer0->HWLink));

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(posIndex);
		Extension->extGlVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(sizeof(SVertex_PNT2W) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(normalIndex);
		Extension->extGlVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(12 + sizeof(SVertex_PNT2W) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex0Index);
		Extension->extGlVertexAttribPointer(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(24 + sizeof(SVertex_PNT2W) * offset0));
	}

	//tex1
	s32 tex1Index = ShaderServices->getAttribLocation(program, NAME_TEX1);
	if (tex1Index >= 0)
	{
		Extension->extGlEnableVertexAttribArray(tex1Index);
		Extension->extGlVertexAttribPointer(tex1Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(32 + sizeof(SVertex_PNT2W) * offset0));
	}

	//weight
	s32 weightIndex = ShaderServices->getAttribLocation(program, NAME_WEIGHT);
	if (weightIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(weightIndex);
		Extension->extGlVertexAttribPointer(weightIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNT2W), buffer_offset(40 + sizeof(SVertex_PNT2W) * offset0));
	}

	ASSERT(vbuffer1 && vbuffer1->HWLink);
	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(vbuffer1->HWLink));

	//blendindices
	s32 blendIndex = ShaderServices->getAttribLocation(program, NAME_BLENDINDICES);
	if (blendIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArray(blendIndex);
		Extension->extGlVertexAttribPointer(blendIndex, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(SVertex_A), buffer_offset(sizeof(SVertex_A) * offset1));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGLES2VertexDeclaration::deleteVao( IVertexBuffer* vbuffer0 )
{
	CLock lock(&g_Globals.vaoCS);

	ASSERT(getVertexType(vbuffer0->Type) == VertexType);
	for (T_VaoMap::iterator itr = VaoMap.begin(); itr != VaoMap.end();)
	{
		if (itr->first.vbuffer0 == vbuffer0)
			VaoMap.erase(itr++);
		else
			++itr;
	}
}



#endif