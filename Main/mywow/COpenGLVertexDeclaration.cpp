#include "stdafx.h"
#include "COpenGLVertexDeclaration.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "S3DVertex.h"
#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLShaderServices.h"
#include "COpenGLHelper.h"
#include "CLock.h"

#define NAME_POS		"Pos"
#define NAME_NORMAL		"Normal"
#define NAME_COLOR0		"Col0"
#define NAME_TEX0	"Tex0"
#define NAME_TEX1	"Tex1"
#define NAME_WEIGHT		"BlendWeight"
#define NAME_BLENDINDICES		"BlendIndices"

#define buffer_offset COpenGLHelper::buffer_offset

COpenGLVertexDeclaration::COpenGLVertexDeclaration( E_VERTEX_TYPE vtype )
 : VertexType(vtype)
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	Extension = driver->getGLExtension();
	MaterialRenderServices = static_cast<COpenGLMaterialRenderServices*>(driver->getMaterialRenderServices());
	ShaderServices = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
}

COpenGLVertexDeclaration::~COpenGLVertexDeclaration()
{
	ASSERT(VaoMap.empty());
	for (T_VaoMap::const_iterator itr = VaoMap.begin(); itr != VaoMap.end(); ++itr)
	{
		Extension->extGlDeleteVertexArrays(1, &itr->second);
	}
	VaoMap.clear();
}

void COpenGLVertexDeclaration::apply(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1)
{
	if (Extension->canUseVAO())
	{
		SVAOParam param;
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
			setDecl_P(program, vbuffer0, offset0);
			break;
		case EVT_PC:
			setDecl_PC(program, vbuffer0, offset0);
			break;
		case EVT_PCT:
			setDecl_PCT(program, vbuffer0, offset0);
			break;
		case EVT_PN:
			setDecl_PN(program, vbuffer0, offset0);
			break;
		case EVT_PNC:
			setDecl_PNC(program, vbuffer0, offset0);
			break;
		case EVT_PNT:
			setDecl_PNT(program, vbuffer0, offset0);
			break;
		case EVT_PT:
			setDecl_PT(program, vbuffer0, offset0);
			break;
		case EVT_PNCT:
			setDecl_PNCT(program, vbuffer0, offset0);
			break;
		case EVT_PNCT2:
			setDecl_PNCT2(program, vbuffer0, offset0);
			break;
		case EVT_PNT2W:
			setDecl_PNT2W(program, vbuffer0, offset0, vbuffer1, offset1);
			break;
		default:
			ASSERT(false);
		}
	}
}

GLuint COpenGLVertexDeclaration::createVao( const SVAOParam& param )
{
	GLuint vao;
	Extension->extGlGenVertexArrays(1, &vao);

	Extension->extGlBindVertexArray(vao);

	switch(VertexType)
	{
	case EVT_P:
		createVao_P(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PC:
		createVao_PC(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PCT:
		createVao_PCT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PN:
		createVao_PN(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNC:
		createVao_PNC(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNT:
		createVao_PNT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PT:
		createVao_PT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNCT:
		createVao_PNCT(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNCT2:
		createVao_PNCT2(param.program, param.vbuffer0, param.offset0);
		break;
	case EVT_PNT2W:
		createVao_PNT2W(param.program, param.vbuffer0, param.offset0, param.vbuffer1, param.offset1);
		break;
	default:
		ASSERT(false);
		break;
	}

	Extension->extGlBindVertexArray(0);

	return vao;
}

GLuint COpenGLVertexDeclaration::getVao( const SVAOParam& param )
{
	//CLock lock(&g_Globals.vaoCS);

	T_VaoMap::iterator itr = VaoMap.find(param);
	if (itr == VaoMap.end())
	{
		GLuint vao = createVao(param);
		VaoMap[param] = vao;
		return vao;
	}
	return itr->second;
}

void COpenGLVertexDeclaration::createVao_P( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_P), buffer_offset(sizeof(SVertex_P) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PC( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PC), buffer_offset(sizeof(SVertex_PC) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(colorIndex);
		Extension->extGlVertexAttribPointerARB(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PC), buffer_offset(12 + sizeof(SVertex_PC) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PCT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PCT), buffer_offset(sizeof(SVertex_PC) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(colorIndex);
		Extension->extGlVertexAttribPointerARB(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PCT), buffer_offset(12 + sizeof(SVertex_PC) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex0Index);
		Extension->extGlVertexAttribPointerARB(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PCT), buffer_offset(16 + sizeof(SVertex_PCT) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PN( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PN), buffer_offset(sizeof(SVertex_PN) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(normalIndex);
		Extension->extGlVertexAttribPointerARB(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PN), buffer_offset(12 + sizeof(SVertex_PN) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PNC( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNC), buffer_offset(sizeof(SVertex_PNC) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(normalIndex);
		Extension->extGlVertexAttribPointerARB(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNC), buffer_offset(12 + sizeof(SVertex_PNC) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(colorIndex);
		Extension->extGlVertexAttribPointerARB(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNC), buffer_offset(24 + sizeof(SVertex_PNC) * offset0));
	}		

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PNT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT), buffer_offset(sizeof(SVertex_PNT) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(normalIndex);
		Extension->extGlVertexAttribPointerARB(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT), buffer_offset(12 + sizeof(SVertex_PNT) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex0Index);
		Extension->extGlVertexAttribPointerARB(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT), buffer_offset(24 + sizeof(SVertex_PNT) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PT), buffer_offset(sizeof(SVertex_PT) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex0Index);
		Extension->extGlVertexAttribPointerARB(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PT), buffer_offset(12 + sizeof(SVertex_PT) * offset0));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PNCT( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT), buffer_offset(sizeof(SVertex_PNCT) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(normalIndex);
		Extension->extGlVertexAttribPointerARB(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT), buffer_offset(12 + sizeof(SVertex_PNCT) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(colorIndex);
		Extension->extGlVertexAttribPointerARB(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNCT), buffer_offset(24 + sizeof(SVertex_PNCT) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex0Index);
		Extension->extGlVertexAttribPointerARB(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT), buffer_offset(28 + sizeof(SVertex_PNCT) * offset0));
	}	

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PNCT2( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(sizeof(SVertex_PNCT2) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(normalIndex);
		Extension->extGlVertexAttribPointerARB(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(12 + sizeof(SVertex_PNCT2) * offset0));
	}

	//color
	s32 colorIndex = ShaderServices->getAttribLocation(program, NAME_COLOR0);
	if (colorIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(colorIndex);
		Extension->extGlVertexAttribPointerARB(colorIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNCT2), buffer_offset(24 + sizeof(SVertex_PNCT2) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex0Index);
		Extension->extGlVertexAttribPointerARB(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(28 + sizeof(SVertex_PNCT2) * offset0));
	}

	//tex1
	s32 tex1Index = ShaderServices->getAttribLocation(program, NAME_TEX1);
	if (tex1Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex1Index);
		Extension->extGlVertexAttribPointerARB(tex1Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNCT2), buffer_offset(36 + sizeof(SVertex_PNCT2) * offset0));
	}	

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::createVao_PNT2W( const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1 )
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);
	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	s32 posIndex = ShaderServices->getAttribLocation(program, NAME_POS);
	if (posIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(posIndex);
		Extension->extGlVertexAttribPointerARB(posIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(sizeof(SVertex_PNT2W) * offset0));
	}

	//normal
	s32 normalIndex = ShaderServices->getAttribLocation(program, NAME_NORMAL);
	if (normalIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(normalIndex);
		Extension->extGlVertexAttribPointerARB(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(12 + sizeof(SVertex_PNT2W) * offset0));
	}

	//tex0
	s32 tex0Index = ShaderServices->getAttribLocation(program, NAME_TEX0);
	if (tex0Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex0Index);
		Extension->extGlVertexAttribPointerARB(tex0Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(24 + sizeof(SVertex_PNT2W) * offset0));
	}

	//tex1
	s32 tex1Index = ShaderServices->getAttribLocation(program, NAME_TEX1);
	if (tex1Index >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(tex1Index);
		Extension->extGlVertexAttribPointerARB(tex1Index, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex_PNT2W), buffer_offset(32 + sizeof(SVertex_PNT2W) * offset0));
	}

	//weight
	s32 weightIndex = ShaderServices->getAttribLocation(program, NAME_WEIGHT);
	if (weightIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(weightIndex);
		Extension->extGlVertexAttribPointerARB(weightIndex, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SVertex_PNT2W), buffer_offset(40 + sizeof(SVertex_PNT2W) * offset0));
	}

	ASSERT(vbuffer1 && vbuffer1->HWLink);
	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer1->HWLink);

	//blendindices
	s32 blendIndex = ShaderServices->getAttribLocation(program, NAME_BLENDINDICES);
	if (blendIndex >= 0)
	{
		Extension->extGlEnableVertexAttribArrayARB(blendIndex);
		Extension->extGlVertexAttribPointerARB(blendIndex, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(SVertex_A), buffer_offset(sizeof(SVertex_A) * offset1));
	}

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

void COpenGLVertexDeclaration::deleteVao( IVertexBuffer* vbuffer0 )
{
	//CLock lock(&g_Globals.vaoCS);

	ASSERT(getVertexType(vbuffer0->Type) == VertexType);
	for (T_VaoMap::iterator itr = VaoMap.begin(); itr != VaoMap.end();)
	{
		if (itr->first.vbuffer0 == vbuffer0)
			VaoMap.erase(itr++);
		else
			++itr;
	}
}

void COpenGLVertexDeclaration::setDecl_P(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{	
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_P), buffer_offset(sizeof(SVertex_P) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PC),  buffer_offset(sizeof(SVertex_PC) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//color
	glEnableClientState(GL_COLOR_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_PC), buffer_offset(12 + sizeof(SVertex_PC) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PCT), buffer_offset(sizeof(SVertex_PCT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//color
	glEnableClientState(GL_COLOR_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_PCT), buffer_offset(12 + sizeof(SVertex_PCT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex0
	MaterialRenderServices->setActiveTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PCT), buffer_offset(16 + sizeof(SVertex_PCT) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PN(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PN), buffer_offset(sizeof(SVertex_PN) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//normal
	glEnableClientState(GL_NORMAL_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glNormalPointer(GL_FLOAT, sizeof(SVertex_PN), buffer_offset(12 + sizeof(SVertex_PN) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PNC(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PNC), buffer_offset(sizeof(SVertex_PNC) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//normal
	glEnableClientState(GL_NORMAL_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glNormalPointer(GL_FLOAT, sizeof(SVertex_PNC), buffer_offset(12 + sizeof(SVertex_PNC) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//color
	glEnableClientState(GL_COLOR_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_PNC), buffer_offset(24 + sizeof(SVertex_PNC) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PNT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PNT), buffer_offset(sizeof(SVertex_PNT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//normal
	glEnableClientState(GL_NORMAL_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glNormalPointer(GL_FLOAT, sizeof(SVertex_PNT), buffer_offset(12 + sizeof(SVertex_PNT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex0
	MaterialRenderServices->setActiveTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PNT), buffer_offset(24 + sizeof(SVertex_PNT) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PT), buffer_offset(sizeof(SVertex_PT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex0
	MaterialRenderServices->setActiveTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PT), buffer_offset(12 + sizeof(SVertex_PT) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PNCT(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PNCT), buffer_offset(sizeof(SVertex_PNCT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//normal
	glEnableClientState(GL_NORMAL_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glNormalPointer(GL_FLOAT, sizeof(SVertex_PNCT), buffer_offset(12 + sizeof(SVertex_PNCT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//color
	glEnableClientState(GL_COLOR_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_PNCT), buffer_offset(24 + sizeof(SVertex_PNCT) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex0
	MaterialRenderServices->setActiveTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PNCT), buffer_offset(28 + sizeof(SVertex_PNCT) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PNCT2(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PNCT2), buffer_offset(sizeof(SVertex_PNCT2) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//normal
	glEnableClientState(GL_NORMAL_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glNormalPointer(GL_FLOAT, sizeof(SVertex_PNCT2), buffer_offset(12 + sizeof(SVertex_PNCT2) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//color
	glEnableClientState(GL_COLOR_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_PNCT2), buffer_offset(24 + sizeof(SVertex_PNCT2) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex0
	MaterialRenderServices->setActiveTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PNCT2), buffer_offset(28 + sizeof(SVertex_PNCT2) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex1
	MaterialRenderServices->setActiveTexture(1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PNCT2), buffer_offset(36 + sizeof(SVertex_PNCT2) * offset0));
	ASSERT_OPENGL_SUCCESS();
}

void COpenGLVertexDeclaration::setDecl_PNT2W(const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1)
{
	ASSERT(vbuffer0 && vbuffer0->HWLink);

	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer0->HWLink);

	//position
	glEnableClientState(GL_VERTEX_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glVertexPointer(3, GL_FLOAT, sizeof(SVertex_PNT2W), buffer_offset(sizeof(SVertex_PNT2W) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//normal
	glEnableClientState(GL_NORMAL_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glNormalPointer(GL_FLOAT, sizeof(SVertex_PNT2W), buffer_offset(12 + sizeof(SVertex_PNT2W) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex0
	MaterialRenderServices->setActiveTexture(0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PNT2W), buffer_offset(24 + sizeof(SVertex_PNT2W) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//tex1
	MaterialRenderServices->setActiveTexture(1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SVertex_PNT2W), buffer_offset(32 + sizeof(SVertex_PNT2W) * offset0));
	ASSERT_OPENGL_SUCCESS();

	//weight
	MaterialRenderServices->setActiveTexture(2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	ASSERT_OPENGL_SUCCESS();
	glTexCoordPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_PNT2W), buffer_offset(40 + sizeof(SVertex_PNT2W) * offset0));
	ASSERT_OPENGL_SUCCESS();

	ASSERT(vbuffer1 && vbuffer1->HWLink);
	Extension->extGlBindBuffer(GL_ARRAY_BUFFER, (GLuint)vbuffer1->HWLink);

	//blendindices
	glEnableClientState(GL_COLOR_ARRAY);		
	ASSERT_OPENGL_SUCCESS();
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SVertex_A), buffer_offset(sizeof(SVertex_A) * offset1));
	ASSERT_OPENGL_SUCCESS();
}

#endif

