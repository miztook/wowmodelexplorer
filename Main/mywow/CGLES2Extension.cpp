#include "stdafx.h"
#include "CGLES2Extension.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2



#ifdef MW_PLATFORM_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif


#include "CGLES2Helper.h"

const char* const GLES2FeatureStrings[] = {
	"GL_OES_compressed_ETC1_RGB8_texture",
	"GL_OES_compressed_paletted_texture",
	"GL_OES_depth24",
	"GL_OES_depth32",
	"GL_OES_depth_texture",
	"GL_OES_EGL_image",
	"GL_OES_EGL_image_external",
	"GL_OES_element_index_uint",
	"GL_OES_fbo_render_mipmap",
	"GL_OES_fragment_precision_high",
	"GL_OES_get_program_binary",
	"GL_OES_mapbuffer",
	"GL_OES_packed_depth_stencil",		
	"GL_OES_required_internalformat",  
	"GL_OES_rgb8_rgba8",
	"GL_OES_standard_derivatives",
	"GL_OES_stencil1",
	"GL_OES_stencil4",
	"GL_OES_texture_3D",
	"GL_OES_texture_float",
	"GL_OES_texture_float_linear",
	"GL_OES_texture_half_float",
	"GL_OES_texture_half_float_linear",
	"GL_OES_texture_npot",
	"GL_OES_vertex_array_object",
	"GL_OES_vertex_half_float",
	"GL_OES_vertex_type_10_10_10_2",

	"GL_AMD_compressed_3DC_texture",
	"GL_AMD_compressed_ATC_texture",
	"GL_AMD_performance_monitor",
	"GL_AMD_program_binary_Z400",

	"GL_ANGLE_depth_texture",
	"GL_ANGLE_framebuffer_blit",
	"GL_ANGLE_framebuffer_multisample",
	"GL_ANGLE_instanced_arrays",
	"GL_ANGLE_pack_reverse_row_order",
	"GL_ANGLE_program_binary",
	"GL_ANGLE_texture_compression_dxt3",
	"GL_ANGLE_texture_compression_dxt5",
	"GL_ANGLE_texture_usage",
	"GL_ANGLE_translated_shader_source",
	"GL_APPLE_copy_texture_levels",
	"GL_APPLE_framebuffer_multisample",
	"GL_APPLE_rgb_422",
	"GL_APPLE_sync",
	"GL_APPLE_texture_format_BGRA8888",
	"GL_APPLE_texture_max_level",

	"GL_ARM_mali_program_binary",
	"GL_ARM_mali_shader_binary",
	"GL_ARM_rgba8",

	"GL_EXT_blend_minmax",
	"GL_EXT_color_buffer_half_float",
	"GL_EXT_debug_label",
	"GL_EXT_debug_marker",
	"GL_EXT_discard_framebuffer",
	"GL_EXT_map_buffer_range",
	"GL_EXT_multisampled_render_to_texture",
	"GL_EXT_multiview_draw_buffers",
	"GL_EXT_multi_draw_arrays",
	"GL_EXT_occlusion_query_boolean",
	"GL_EXT_read_format_bgra",
	"GL_EXT_robustness",
	"GL_EXT_separate_shader_objects",
	"GL_EXT_shader_framebuffer_fetch",
	"GL_EXT_shader_texture_lod",
	"GL_EXT_shadow_samplers",
	"GL_EXT_sRGB",
	
	"GL_EXT_texture_compression_dxt1",
	"GL_EXT_texture_filter_anisotropic",
	"GL_EXT_texture_format_BGRA8888",
	"GL_EXT_texture_rg",
	"GL_EXT_texture_storage",
	"GL_EXT_texture_type_2_10_10_10_REV",
	"GL_EXT_unpack_subimage",

	"GL_DMP_shader_binary",

	"GL_FJ_shader_binary_GCCSO",

	"GL_IMG_program_binary",
	"GL_IMG_read_format",
	"GL_IMG_shader_binary",
	"GL_IMG_texture_compression_pvrtc",
	"GL_IMG_texture_compression_pvrtc2",
	"GL_IMG_multisampled_render_to_texture",

	"GL_NV_coverage_sample",
	"GL_NV_depth_nonlinear",
	"GL_NV_draw_buffers",
	"GL_NV_draw_instanced",
	"GL_NV_fbo_color_attachments",
	"GL_NV_fence",
	"GL_NV_framebuffer_blit",
	"GL_NV_framebuffer_multisample",
	"GL_NV_generate_mipmap_sRGB",
	"GL_NV_instanced_arrays",
	"GL_NV_read_buffer",
	"GL_NV_read_buffer_front",
	"GL_NV_read_depth",
	"GL_NV_read_depth_stencil",
	"GL_NV_read_stencil",
	"GL_NV_shadow_samplers_array",
	"GL_NV_shadow_samplers_cube",
	"GL_NV_sRGB_formats",
	"GL_NV_texture_border_clamp",
	"GL_NV_texture_compression_s3tc_update",
	"GL_NV_texture_npot_2D_mipmap",

	"GL_QCOM_alpha_test",
	"GL_QCOM_binning_control",
	"GL_QCOM_driver_control",
	"GL_QCOM_extended_get",
	"GL_QCOM_extended_get2",
	"GL_QCOM_perfmon_global_mode",
	"GL_QCOM_writeonly_rendering",
	"GL_QCOM_tiled_rendering",

	"GL_VIV_shader_binary",
};

CGLES2Extension::CGLES2Extension()
{
	MaxTextureUnits = 1;
	MaxAnisotropy = 1;
	MaxMultiSample = 0;
	MaxTextureSize = 1;
	Version = 0;
	ShaderLanguageVersion = 0;
	MaxVertexShaderConst = 0;
	MaxPixelShaderConst = 0;

	TextureCompressionS3 = false;
	TextureCompressionPVR = false;
	TextureCompressionPVR2 = false;
	TextureCompressionETC1 = false;
	TextureCompressionATC = false;
	TextureCompression3DC = false;

	TextureCompression = false;
	MultisampleSupported = false;
	DiscardFrameSupported = false;

	memset(FeatureAvailable, 0, sizeof(FeatureAvailable));

	DimAliasedLine[0]=1.f;
	DimAliasedLine[1]=1.f;
	DimAliasedPoint[0]=1.f;
	DimAliasedPoint[1]=1.f;

#ifndef MW_PLATFORM_IOS
	pGlBindVertexArrayOES = NULL;
	pGlDeleteVertexArrayOES = NULL;
	pGlGenVertexArrayOES = NULL;
	pGlIsVertexArrayOES = NULL;
	pGlDiscardFrameBufferEXT = NULL;
    pGlRenderbufferStorageMultisampleEXT = NULL;
	pGlFramebufferTexture2DMultisampleEXT = NULL;
	pGlRenderbufferStorageMultisampleIMG = NULL;
	pGlFramebufferTexture2DMultisampleIMG = NULL;
#endif
    
	
}

CGLES2Extension::~CGLES2Extension()
{

}

bool CGLES2Extension::initExtensions()
{
	const c8* sv = "2.0";  //reinterpret_cast<const c8*>(glGetString(GL_VERSION));
	const f32 ogl_ver = fast_atof(sv);
	Version = static_cast<u16>(floor32_(ogl_ver)*100+round32_(fract_(ogl_ver)*10.0f));

	const char* t = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
	if (!t)
	{
		ASSERT(false);
		return false;
	}
	u32 len = (u32)strlen(t);
	c8 * str = (c8*)Z_AllocateTempMemory(len + 1); 

	c8* p = str;

	for (u32 i=0; i<len; ++i)
	{
		str[i] = static_cast<char>(t[i]);

		if (str[i] == ' ')
		{
			str[i] = 0;
			for (u32 j=0; j<IRR_GLES2_Feature_Count; ++j)
			{
				if (!strcmp(GLES2FeatureStrings[j], p))
				{
					FeatureAvailable[j] = true;
					break;
				}
			}

            CSysUtility::outputDebug("%s\n", p);

			p = p + strlen(p) + 1;
		}
	}

	Z_FreeTempMemory(str);

	GLint num;

	TextureCompressionS3 = false;
	TextureCompressionPVR = FeatureAvailable[IRR_IMG_texture_compression_pvrtc];
	TextureCompressionPVR2 = FeatureAvailable[IRR_IMG_texture_compression_pvrtc2];
	TextureCompressionETC1 = FeatureAvailable[IRR_OES_compressed_ETC1_RGB8_texture];
	TextureCompressionATC = FeatureAvailable[IRR_AMD_compressed_ATC_texture];
	TextureCompression3DC = FeatureAvailable[IRR_AMD_compressed_3DC_texture];

	TextureCompression = TextureCompressionS3 ||
		TextureCompressionPVR ||
		TextureCompressionPVR2 ||
		TextureCompressionETC1 ||
		TextureCompressionATC ||
		TextureCompression3DC;

	MultisampleSupported = FeatureAvailable[IRR_APPLE_framebuffer_multisample];
		//FeatureAvailable[IRR_IMG_multisampled_render_to_texture] || 
		//FeatureAvailable[IRR_EXT_multisampled_render_to_texture];

	DiscardFrameSupported = FeatureAvailable[IRR_EXT_discard_framebuffer];

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
	MaxTextureUnits=static_cast<u8>(num);

	if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic])
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &num);
		MaxAnisotropy=static_cast<u8>(num);
	}

#ifndef MW_PLATFORM_IOS
	if (FeatureAvailable[IRR_EXT_multisampled_render_to_texture])
	{
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &num);
		MaxMultiSample = static_cast<u8>(num);
	}
	else if (FeatureAvailable[IRR_IMG_multisampled_render_to_texture])
	{
		glGetIntegerv(GL_MAX_SAMPLES_IMG, &num);
		MaxMultiSample = static_cast<u8>(num);
	}
	else
#else
	{
		glGetIntegerv(GL_MAX_SAMPLES_APPLE, &num);
		MaxMultiSample = static_cast<u8>(num);
	}
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &num);
	MaxTextureSize = max_(static_cast<u32>(num), 2048u);

	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);

	glGetError(); // clean error buffer

	//const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	const f32 sl_ver = fast_atof("3.00");
	ShaderLanguageVersion = static_cast<u16>(floor32_(sl_ver)*100+round32_(fract_(sl_ver)*10.0f));
	
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &num);
	MaxVertexShaderConst = static_cast<u32>(num);

	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &num);
	MaxPixelShaderConst = static_cast<u32>(num);

	MaxTextureUnits = min_(MaxTextureUnits, static_cast<u8>(MATERIAL_MAX_TEXTURES));

	return initExtFunctions();
}

bool CGLES2Extension::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{
	switch (feature)
	{
	case EVDF_RENDER_TO_TARGET:
		return true;
	case EVDF_HARDWARE_TL:
	case EVDF_TEXTURE_ADDRESS:
	case EVDF_SEPARATE_UVWRAP:
	case  EVDF_MIP_MAP:
	case EVDF_STENCIL_BUFFER:
	case EVDF_TEXTURE_NSQUARE:
		return true;
	case EVDF_TEXTURE_NPOT:
		return true;
	case EVDF_COLOR_MASK:
	case EVDF_MULTIPLE_RENDER_TARGETS:
	case EVDF_MRT_BLEND:
		return true;
	case EVDF_STREAM_OFFSET:
		return false;
	case EVDF_TEXTURE_MULTISAMPLING:
		return FeatureAvailable[IRR_APPLE_framebuffer_multisample];
	case EVDF_PIXEL_SHADER_2_0:
	case EVDF_VERTEX_SHADER_2_0:
		return ShaderLanguageVersion>=200;
	case EVDF_PIXEL_SHADER_3_0:
	case EVDF_VERTEX_SHADER_3_0:
		return ShaderLanguageVersion>=300;
	default:
		return false;
	}
}

#ifndef MW_PLATFORM_IOS
GLboolean CGLES2Extension::extGlSwapInterval( EGLDisplay dpy, EGLint interval )
{
	GLboolean ret;
	ret = eglSwapInterval(dpy, interval);
	CHECK_GLES2_ERROR("extGlSwapInterval");

	return ret;
}

GLboolean CGLES2Extension::extGlSwapBuffers( EGLDisplay dpy, EGLSurface surface )
{
	GLboolean ret;
	ret = eglSwapBuffers(dpy, surface);
	CHECK_GLES2_ERROR("extGlSwapBuffers");

	return ret;
}
#endif

void CGLES2Extension::extGlActiveTexture( GLenum texture )
{
	if (MaxTextureUnits > 1)
	{
		glActiveTexture(texture);
		CHECK_GLES2_ERROR("extGlActiveTexture");
	}
}

bool CGLES2Extension::canUseVAO() const
{
	return queryOpenGLFeature(IRR_OES_vertex_array_object);
}

void CGLES2Extension::extGlCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data )
{
	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	CHECK_GLES2_ERROR("extGlCompressedTexImage2D");
}

void CGLES2Extension::extGlCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data )
{
	glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
	CHECK_GLES2_ERROR("extGlCompressedTexSubImage2D");
}

GLuint CGLES2Extension::extGlCreateShader( GLenum type )
{
	GLuint ret = glCreateShader(type);
	CHECK_GLES2_ERROR("extGlCreateShader");
	return ret;
}

void CGLES2Extension::extGlShaderSource( GLuint shader, GLsizei count, const GLchar** string, const GLint* length )
{
	glShaderSource(shader, count, string, length);
	CHECK_GLES2_ERROR("extGlShaderSource");
}

void CGLES2Extension::extGlCompileShader( GLuint shader )
{
	glCompileShader(shader);
	CHECK_GLES2_ERROR("extGlCompileShader");
}

void CGLES2Extension::extGlGetShaderiv( GLuint shader, GLenum pname, GLint* params )
{
	glGetShaderiv(shader, pname, params);
	CHECK_GLES2_ERROR("extGlGetShaderiv");
}

void CGLES2Extension::extGlGetShaderInfoLog( GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog )
{
	glGetShaderInfoLog(shader, bufsize, length, infolog);
	CHECK_GLES2_ERROR("extGlGetShaderInfoLog");
}

void CGLES2Extension::extGlDeleteShader( GLuint shader )
{
	glDeleteShader(shader);
	CHECK_GLES2_ERROR("extGlDeleteShader");
}

GLuint CGLES2Extension::extGlCreateProgram()
{
	GLuint ret = glCreateProgram();
	CHECK_GLES2_ERROR("extGlCreateProgram");
	return ret;
}

void CGLES2Extension::extGlAttachShader( GLuint program, GLuint shader )
{
	glAttachShader(program, shader);
	CHECK_GLES2_ERROR("extGlAttachShader");
}

void CGLES2Extension::extGlBindAttribLocation( GLuint program, GLuint index, const char *name )
{
	glBindAttribLocation(program, index, name);
	CHECK_GLES2_ERROR("extGlBindAttribLocation");
}

int CGLES2Extension::extGlGetAttribLocation( GLuint program, const GLchar *name )
{
	int ret = glGetAttribLocation(program, name);
	CHECK_GLES2_ERROR("extGlGetAttribLocation");
	return ret;
}

void CGLES2Extension::extGlLinkProgram( GLuint program )
{
	glLinkProgram(program);
	CHECK_GLES2_ERROR("extGlLinkProgram");
}

void CGLES2Extension::extGlUseProgram( GLuint program )
{
	glUseProgram(program);
	CHECK_GLES2_ERROR("extGlUseProgram");
}

void CGLES2Extension::extGlDeleteProgram( GLuint program )
{
	glDeleteProgram(program);
	CHECK_GLES2_ERROR("extGlDeleteProgram");
}

void CGLES2Extension::extGlGetAttachedShaders( GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders )
{
	glGetAttachedShaders(program, maxCount, count, shaders);
	CHECK_GLES2_ERROR("extGlGetAttachedShaders");
}

void CGLES2Extension::extGlGetProgramiv( GLuint program, GLenum pname, GLint* params )
{
	glGetProgramiv(program, pname, params);
	CHECK_GLES2_ERROR("extGlGetProgramiv");
}

void CGLES2Extension::extGlGetProgramInfoLog( GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog )
{
	glGetProgramInfoLog(program, bufsize, length, infolog);
	CHECK_GLES2_ERROR("extGlGetProgramInfoLog");
}

void CGLES2Extension::extGlGetActiveUniform( GLuint program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLchar *name )
{
	glGetActiveUniform(program, index, maxlength, length, size, type, name);
	CHECK_GLES2_ERROR("extGlGetActiveUniform");
}

GLint CGLES2Extension::extGlGetUniformLocation( GLuint program, const char *name )
{
	GLint ret = glGetUniformLocation(program, name);
	CHECK_GLES2_ERROR("extGlGetUniformLocation");
	return ret;
}

void CGLES2Extension::extGlUniform4fv( GLint location, GLsizei count, const GLfloat *v )
{
	glUniform4fv(location, count, v);
	CHECK_GLES2_ERROR("extGlUniform4fv");
}

void CGLES2Extension::extGlUniform1iv( GLint loc, GLsizei count, const GLint *v )
{
	glUniform1iv(loc, count, v);
	CHECK_GLES2_ERROR("extGlUniform1iv");
}

void CGLES2Extension::extGlUniform1fv( GLint loc, GLsizei count, const GLfloat *v )
{
	glUniform1fv(loc, count, v);
	CHECK_GLES2_ERROR("extGlUniform1fv");
}

void CGLES2Extension::extGlUniform2fv( GLint loc, GLsizei count, const GLfloat *v )
{
	glUniform2fv(loc, count, v);
	CHECK_GLES2_ERROR("extGlUniform2fv");
}

void CGLES2Extension::extGlUniform3fv( GLint loc, GLsizei count, const GLfloat *v )
{
	glUniform3fv(loc, count, v);
	CHECK_GLES2_ERROR("extGlUniform3fv");
}

void CGLES2Extension::extGlUniformMatrix2fv( GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v )
{
	glUniformMatrix2fv(loc, count, transpose, v);
	CHECK_GLES2_ERROR("extGlUniformMatrix2fv");
}

void CGLES2Extension::extGlUniformMatrix3fv( GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v )
{
	glUniformMatrix3fv(loc, count, transpose, v);
	CHECK_GLES2_ERROR("extGlUniformMatrix3fv");
}

void CGLES2Extension::extGlUniformMatrix4fv( GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v )
{
	glUniformMatrix4fv(loc, count, transpose, v);
	CHECK_GLES2_ERROR("extGlUniformMatrix4fv");
}

void CGLES2Extension::extGlBindFramebuffer( GLenum target, GLuint framebuffer )
{
	glBindFramebuffer(target, framebuffer);
	CHECK_GLES2_ERROR("extGlBindFramebuffer");
}

void CGLES2Extension::extGlDeleteFramebuffers( GLsizei n, const GLuint *framebuffers )
{
	glDeleteFramebuffers(n, framebuffers);
	CHECK_GLES2_ERROR("extGlDeleteFrameBuffers");
}

void CGLES2Extension::extGlGenFramebuffers( GLsizei n, GLuint *framebuffers )
{
	glGenFramebuffers(n, framebuffers);
	CHECK_GLES2_ERROR("extGlGenFramebuffers");
}

GLenum CGLES2Extension::extGlCheckFramebufferStatus( GLenum target )
{
	GLenum ret = glCheckFramebufferStatus(target);
	CHECK_GLES2_ERROR("extGlCheckFramebufferStatus");
	return ret;
}

void CGLES2Extension::extGlFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
	CHECK_GLES2_ERROR("extGlFramebufferTexture2D");
}

void CGLES2Extension::extGlBindRenderbuffer( GLenum target, GLuint renderbuffer )
{
	glBindRenderbuffer(target, renderbuffer);
	CHECK_GLES2_ERROR("extGlBindRenderbuffer");
}

void CGLES2Extension::extGlDeleteRenderbuffers( GLsizei n, const GLuint *renderbuffers )
{
	glDeleteRenderbuffers(n, renderbuffers);
	CHECK_GLES2_ERROR("extGlDeleteRenderbuffers");
}

void CGLES2Extension::extGlGenRenderbuffers( GLsizei n, GLuint *renderbuffers )
{
	glGenRenderbuffers(n, renderbuffers);
	CHECK_GLES2_ERROR("extGlGenRenderbuffers");
}

void CGLES2Extension::extGlRenderbufferStorage( GLenum target, GLenum internalformat, GLsizei width, GLsizei height )
{
	glRenderbufferStorage(target, internalformat, width, height);
	CHECK_GLES2_ERROR("extGlRenderbufferStorage");
}

void CGLES2Extension::extGlFramebufferRenderbuffer( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )
{
	glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	CHECK_GLES2_ERROR("extGlFramebufferRenderbuffer");
}

void CGLES2Extension::extGlGenBuffers( GLsizei n, GLuint *buffers )
{
	glGenBuffers(n, buffers);
	CHECK_GLES2_ERROR("extGlGenBuffers");
}

void CGLES2Extension::extGlBindBuffer( GLenum target, GLuint buffer )
{
	glBindBuffer(target, buffer);
	CHECK_GLES2_ERROR("extGlBindBuffer");
}

void CGLES2Extension::extGlBufferData( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage )
{
	glBufferData(target, size, data, usage);
	CHECK_GLES2_ERROR("extGlBufferData");
}

void CGLES2Extension::extGlDeleteBuffers( GLsizei n, const GLuint *buffers )
{
	glDeleteBuffers(n, buffers);
	CHECK_GLES2_ERROR("extGlDeleteBuffers");
}

void CGLES2Extension::extGlBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data )
{
	glBufferSubData(target, offset, size, data);
	CHECK_GLES2_ERROR("extGlBufferSubData");
}

GLboolean CGLES2Extension::extGlIsBuffer( GLuint buffer )
{
	GLboolean ret = glIsBuffer(buffer);
	CHECK_GLES2_ERROR("extGlIsBuffer");
	return ret;
}

void CGLES2Extension::extGlDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	glDrawElements(mode, count, type, indices);
	CHECK_GLES2_ERROR("extGlDrawElements");
}

void CGLES2Extension::extGlGenVertexArrays( GLsizei n, GLuint *arrays )
{
#ifndef MW_PLATFORM_IOS
	pGlGenVertexArrayOES(n, arrays);
#else
    ::glGenVertexArraysOES(n, arrays);
#endif
	CHECK_GLES2_ERROR("extGlGenVertexArrays");
}

void CGLES2Extension::extGlDeleteVertexArrays( GLsizei n, const GLuint *arrays )
{
#ifndef MW_PLATFORM_IOS
	pGlDeleteVertexArrayOES(n, arrays);
#else
    ::glDeleteVertexArraysOES(n, arrays);
#endif
	CHECK_GLES2_ERROR("extGlDeleteVertexArrays");
}

void CGLES2Extension::extGlBindVertexArray( GLuint array )
{
#ifndef MW_PLATFORM_IOS
	pGlBindVertexArrayOES(array);
#else
    ::glBindVertexArrayOES(array);
#endif
	CHECK_GLES2_ERROR("extGlBindVertexArray");
}

void CGLES2Extension::extGlEnableVertexAttribArray( GLuint index )
{
	glEnableVertexAttribArray(index);
	CHECK_GLES2_ERROR("extGlEnableVertexAttribArray");
}

void CGLES2Extension::extGlDisableVertexAttribArray( GLuint index )
{
	glDisableVertexAttribArray(index);
	CHECK_GLES2_ERROR("extGlDisableVertexAttribArray");
}

void CGLES2Extension::extGlVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	CHECK_GLES2_ERROR("extGlVertexAttribPointer");
}

void CGLES2Extension::extGlDiscardFramebufferEXT( GLenum target, GLsizei numAttachments, const GLenum *attachments )
{
#ifndef MW_PLATFORM_IOS
	pGlDiscardFrameBufferEXT(target, numAttachments, attachments);
#else
    ::glDiscardFramebufferEXT(target, numAttachments, attachments);
#endif
	CHECK_GLES2_ERROR("extGlDiscardFramebufferEXT");
}

void CGLES2Extension::extGlRenderbufferStorageMultisample( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
{
#ifndef MW_PLATFORM_IOS
	if (FeatureAvailable[IRR_EXT_multisampled_render_to_texture])
	{
		pGlRenderbufferStorageMultisampleEXT(target, samples, internalformat, width, height);
        CHECK_GLES2_ERROR("extGlRenderbufferStorageMultisample");
	}
	else if (FeatureAvailable[IRR_IMG_multisampled_render_to_texture])
	{
		pGlRenderbufferStorageMultisampleIMG(target, samples, internalformat, width, height);
        CHECK_GLES2_ERROR("extGlRenderbufferStorageMultisample");
	}
#else
    ::glRenderbufferStorageMultisampleAPPLE(target, samples, internalformat, width, height);
	CHECK_GLES2_ERROR("extGlRenderbufferStorageMultisample");
#endif
}

void CGLES2Extension::extGlFramebufferTexture2DMultisample( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples )
{
#ifndef MW_PLATFORM_IOS
	if (FeatureAvailable[IRR_EXT_multisampled_render_to_texture])
	{
		pGlFramebufferTexture2DMultisampleEXT(target, attachment, textarget, texture, level, samples);
        CHECK_GLES2_ERROR("extGlFramebufferTexture2DMultisample");
	}
	else if (FeatureAvailable[IRR_IMG_multisampled_render_to_texture])
	{
		pGlFramebufferTexture2DMultisampleIMG(target, attachment, textarget, texture, level, samples);
        CHECK_GLES2_ERROR("extGlFramebufferTexture2DMultisample");
	}
#endif
}

void CGLES2Extension::extGlResolveMultisampleFramebuffer()
{
#ifdef MW_PLATFORM_IOS
	::glResolveMultisampleFramebufferAPPLE();
#else
	ASSERT(false);
#endif

	CHECK_GLES2_ERROR("extGlResolveMultisampleFramebuffer");
}

bool CGLES2Extension::initExtFunctions()
{
    
#ifndef MW_PLATFORM_IOS
	if (FeatureAvailable[IRR_OES_vertex_array_object])
	{
		pGlBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC) getProcAddress("glBindVertexArrayOES");
		pGlDeleteVertexArrayOES = (PFNGLDELETEVERTEXARRAYSOESPROC) getProcAddress("glDeleteVertexArraysOES");
		pGlGenVertexArrayOES = (PFNGLGENVERTEXARRAYSOESPROC) getProcAddress("glGenVertexArraysOES");
		pGlIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC) getProcAddress("glIsVertexArrayOES");
	}

	if (FeatureAvailable[IRR_EXT_discard_framebuffer])
	{
		pGlDiscardFrameBufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC) getProcAddress("glDiscardFramebufferEXT");
	}

	if (FeatureAvailable[IRR_EXT_multisampled_render_to_texture])
	{
		pGlRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) getProcAddress("glRenderbufferStorageMultisampleEXT");
		pGlFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) getProcAddress("glFramebufferTexture2DMultisampleEXT");
	}

	if (FeatureAvailable[IRR_IMG_multisampled_render_to_texture])
	{
		pGlRenderbufferStorageMultisampleIMG = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC) getProcAddress("glRenderbufferStorageMultisampleIMG");
		pGlFramebufferTexture2DMultisampleIMG = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC) getProcAddress("glFramebufferTexture2DMultisampleIMG");
	}
#endif

	return true;
}

#ifndef MW_PLATFORM_IOS
__eglMustCastToProperFunctionPointerType CGLES2Extension::getProcAddress( const c8* funcname )
{
	__eglMustCastToProperFunctionPointerType p = eglGetProcAddress(funcname);
	if (!p)
	{
		CSysUtility::outputDebug("cannot find GLES2 entry point: %s\n", funcname);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "cannot find GLES2 entry point: %s", funcname);
	}
	return p;
}
#endif

bool CGLES2Extension::checkFBOStatus()
{
	GLenum status = extGlCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
		//Our FBO is perfect, return true
	case GL_FRAMEBUFFER_COMPLETE:
		return true;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		ASSERT(false);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		ASSERT(false);
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		ASSERT(false);
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		ASSERT(false);
		break;

	default:
		ASSERT(false);
		break;
	}

	return false;
}


#endif


