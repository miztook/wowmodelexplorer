#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "core.h"
#include "SColor.h"
#include "CSysUtility.h"

#include <gl/GL.h>
#include "gl/glext.h"

#if 0 //defined(_DEBUG) || defined(DEBUG)

#define CHECK_OPENGL_ERROR(FUNC_NAME) \
{			\
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR)		\
	{				\
	 CSysUtility::outputDebug("OPENGL_ERROR : '%s', errorcode = 0x%x \n", (FUNC_NAME), err);				\
	ASSERT(false);						\
	}					\
}

#define ASSERT_OPENGL_SUCCESS()		ASSERT(glGetError() == GL_NO_ERROR)

#else

#define CHECK_OPENGL_ERROR(FUNC_NAME)

#define ASSERT_OPENGL_SUCCESS()

#endif

class COpenGLHelper
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLHelper);

public:
	static bool init();

public:
	static const GLvoid* buffer_offset(u32 offset) { return (const GLvoid*)((char*)NULL_PTR + offset); }

	static void colorToGLfloat4(const SColor& col, GLfloat v[]);

	static void colorfToGLfloat4(const SColorf& col, GLfloat v[]);

	static GLenum getGLCompare(E_COMPARISON_FUNC comp);

	static GLint getGLTextureAddress(E_TEXTURE_CLAMP clamp);

	static E_TEXTURE_CLAMP getTextureAddressMode(GLint address);

	static GLint getGLTextureEnvMode(E_TEXTURE_OP colorOp, E_TEXTURE_OP alphaOp);

	static GLint getGLTextureOp(E_TEXTURE_OP op, GLfloat& scale);

	static GLint getGLTextureArg( E_TEXTURE_ARG arg );

	static GLint getGLBlend( E_BLEND_FACTOR factor );

	static GLint getGLTextureFormat(ECOLOR_FORMAT format, GLenum& colorFormat, GLenum& type);

	static GLenum getGLDepthFormat(ECOLOR_FORMAT format);

	static bool IsGLError();

	static void getGLMatrixFromD3D(GLfloat gl_matrix[16], const matrix4& m);					//d3d左手坐标系到opengl的右手坐标系

	static void getGLTextureMatrix(GLfloat gl_matrix[16], const matrix4& m);

	static GLenum getGLTopology(E_PRIMITIVE_TYPE type);

	static E_UNIFORM_TYPE getGLUniformType(GLenum type);
};


inline E_TEXTURE_CLAMP COpenGLHelper::getTextureAddressMode( GLint address )
{
	switch(address)
	{
	case GL_REPEAT:
		return ETC_REPEAT;
	case GL_MIRRORED_REPEAT:
		return ETC_MIRROR;
	case GL_CLAMP_TO_EDGE:
		return ETC_CLAMP;
	case GL_CLAMP_TO_BORDER:
		return ETC_CLAMP_TO_BORDER;
	case GL_MIRROR_CLAMP_ATI:
		return ETC_MIRROR_CLAMP;
	case GL_MIRROR_CLAMP_TO_BORDER_EXT:
		return ETC_MIRROR_CLAMP_TO_BORDER;
	default:
		ASSERT(false);
		return ETC_REPEAT;
	}
}

inline GLint COpenGLHelper::getGLTextureOp( E_TEXTURE_OP op, GLfloat& scale )
{
	scale = 1.0f;
	GLint r = GL_MODULATE;

	switch(op)
	{
	case ETO_DISABLE:
		break;
	case ETO_ARG1:
		r = GL_REPLACE;
		break;
	case ETO_MODULATE:
		break;
	case ETO_MODULATE_X2:
		scale = 2.0f;
		break;
	case ETO_MODULATE_X4:
		scale = 4.0f;
		break;
	default:
		ASSERT(false);
	}

	return r;
}

inline void COpenGLHelper::colorToGLfloat4( const SColor& col, GLfloat v[] )
{
	const f32 f = 1.0f / 255.0f;
	v[0] = col.getRed() * f;
	v[1] = col.getGreen() * f;
	v[2] = col.getBlue() * f;
	v[3] = col.getAlpha() * f;
}

inline void COpenGLHelper::colorfToGLfloat4( const SColorf& col, GLfloat v[] )
{
	v[0] = col.getRed();
	v[1] = col.getGreen();
	v[2] = col.getBlue();
	v[3] = col.getAlpha();
}

inline GLenum COpenGLHelper::getGLCompare( E_COMPARISON_FUNC comp )
{
	GLenum r = GL_NEVER;
	switch(comp)
	{
	case ECFN_NEVER:
		r = GL_NEVER; break;
	case	ECFN_LESSEQUAL:
		r = GL_LEQUAL; break;
	case	ECFN_EQUAL:
		r = GL_EQUAL; break;
	case 	ECFN_LESS:
		r = GL_LESS; break;
	case	ECFN_NOTEQUAL:
		r = GL_NOTEQUAL; break;
	case	ECFN_GREATEREQUAL:
		r = GL_GEQUAL; break;
	case	ECFN_GREATER:
		r = GL_GREATER;	break;
	case	ECFN_ALWAYS:
		r = GL_ALWAYS;	break;
	default:
		ASSERT(false);
	}
	return r;
}

inline GLint COpenGLHelper::getGLBlend( E_BLEND_FACTOR factor )
{
	GLint r = 0;
	switch ( factor )
	{
	case EBF_ZERO:			
		r = GL_ZERO; break;
	case EBF_ONE:			
		r = GL_ONE; break;
	case EBF_DST_COLOR:		
		r = GL_DST_COLOR; break;
	case EBF_ONE_MINUS_DST_COLOR:	
		r = GL_ONE_MINUS_DST_COLOR; break;
	case EBF_SRC_COLOR:		
		r = GL_SRC_COLOR; break;
	case EBF_ONE_MINUS_SRC_COLOR:	
		r = GL_ONE_MINUS_SRC_COLOR; break;
	case EBF_SRC_ALPHA:		
		r = GL_SRC_ALPHA; break;
	case EBF_ONE_MINUS_SRC_ALPHA:	
		r = GL_ONE_MINUS_SRC_ALPHA; break;
	case EBF_DST_ALPHA:		
		r = GL_DST_ALPHA; break;
	case EBF_ONE_MINUS_DST_ALPHA:	
		r = GL_ONE_MINUS_DST_ALPHA; break;
	case EBF_SRC_ALPHA_SATURATE:	
		r = GL_SRC_ALPHA_SATURATE; break;
	default:
		ASSERT(false);
	}
	return r;
}

inline void COpenGLHelper::getGLMatrixFromD3D( GLfloat gl_matrix[16], const matrix4& m )
{
	Q_memcpy(gl_matrix, sizeof(GLfloat) * 16, m.pointer(), sizeof(f32) * 16);
	gl_matrix[12] = -gl_matrix[12];
}

inline void COpenGLHelper::getGLTextureMatrix( GLfloat gl_matrix[16], const matrix4& m )
{
	gl_matrix[0] = m[0];
	gl_matrix[1] = m[1];
	gl_matrix[2] = 0.f;
	gl_matrix[3] = 0.f;

	gl_matrix[4] = m[4];
	gl_matrix[5] = m[5];
	gl_matrix[6] = 0.f;
	gl_matrix[7] = 0.f;

	gl_matrix[8] = 0.f;
	gl_matrix[9] = 0.f;
	gl_matrix[10] = 1.f;
	gl_matrix[11] = 0.f;

	gl_matrix[12] = m[8];
	gl_matrix[13] = m[9];
	gl_matrix[14] = 0.f;
	gl_matrix[15] = 1.f;
}

inline GLenum COpenGLHelper::getGLTopology( E_PRIMITIVE_TYPE type )
{
	switch (type)
	{
	case EPT_POINTS:
		return GL_POINTS;
	case EPT_LINE_STRIP:
		return GL_LINE_STRIP;
	case EPT_LINES:
		return GL_LINES;
	case EPT_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	case EPT_TRIANGLES:
		return GL_TRIANGLES;
	default:
		ASSERT(false);
		return GL_TRIANGLES;
	}
}

inline E_UNIFORM_TYPE COpenGLHelper::getGLUniformType( GLenum type )
{
	switch(type)
	{
	case GL_FLOAT:
		return EUT_FLOAT;
	case GL_FLOAT_VEC2:
		return EUT_VEC2;
	case GL_FLOAT_VEC3:
		return EUT_VEC3;
	case GL_FLOAT_MAT2:
		return EUT_MAT2;
	case GL_FLOAT_MAT3:
		return EUT_MAT3;
	case GL_FLOAT_MAT4:
		return EUT_MAT4;
	case GL_SAMPLER_1D:
		return EUT_SAMPLER1D;
	case GL_SAMPLER_2D:
		return EUT_SAMPLER2D;
	case GL_SAMPLER_3D:
		return EUT_SAMPLER3D;
	default:
		ASSERT(false);
		return EUT_VEC4;
	}
}

inline GLint COpenGLHelper::getGLTextureAddress( E_TEXTURE_CLAMP clamp )
{
	GLint mode=GL_REPEAT;
	switch (clamp)
	{
	case ETC_REPEAT:
		mode=GL_REPEAT;
		break;
	case ETC_CLAMP:
		mode = GL_CLAMP_TO_EDGE;
		break;
	case ETC_CLAMP_TO_BORDER:
		mode=GL_CLAMP_TO_EDGE;
		break;
	case ETC_MIRROR:
	case ETC_MIRROR_CLAMP:
	case ETC_MIRROR_CLAMP_TO_BORDER:
		mode = GL_MIRRORED_REPEAT;
		break;
	default:
		break;
	}
	return mode;
}

inline GLint COpenGLHelper::getGLTextureFormat( ECOLOR_FORMAT format, GLenum& colorFormat, GLenum& type )
{
	colorFormat = GL_RGBA;
	type = GL_UNSIGNED_BYTE;

	switch(format)
	{
	case ECF_A8L8:
		colorFormat = GL_LUMINANCE_ALPHA;
		type = GL_UNSIGNED_BYTE;
		return GL_LUMINANCE_ALPHA;
	case ECF_A1R5G5B5:
		colorFormat = GL_RGBA;
		type = GL_UNSIGNED_SHORT_5_5_5_1;
		return GL_RGB5_A1;
	case ECF_R5G6B5:
		colorFormat = GL_RGB;
		type = GL_UNSIGNED_SHORT_5_6_5;
		return GL_RGB565;
	case ECF_R8G8B8:
		colorFormat = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		return GL_RGB8;
	case ECF_A8R8G8B8:
		colorFormat = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		return GL_RGBA8;
	case ECF_ARGB32F:
		colorFormat = GL_RGBA;
		type = GL_FLOAT;
		return GL_RGBA32F;

	case ECF_DXT1:
		colorFormat = GL_NONE;
		type = GL_NONE;
		return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case ECF_DXT3:
		colorFormat = GL_NONE;
		type = GL_NONE;
		return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case ECF_DXT5:
		colorFormat = GL_NONE;
		type = GL_NONE;
		return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	}
	ASSERT(false);
	return GL_RGBA8;
}

inline GLenum COpenGLHelper::getGLDepthFormat(ECOLOR_FORMAT format)
{
	switch(format)
	{
	case ECF_D16:
		return GL_DEPTH_COMPONENT16;

	case ECF_D24:
		return GL_DEPTH_COMPONENT24;

	case ECF_D24S8:
		return GL_DEPTH24_STENCIL8;

	case ECF_D32:
		return GL_DEPTH_COMPONENT32;

	default:
		ASSERT(false);
		return GL_DEPTH_COMPONENT;
	}
}

#endif