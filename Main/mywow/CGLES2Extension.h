#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "base.h"
#include "EGL/egl.h"

enum EGLES2Features
{
	IRR_OES_compressed_ETC1_RGB8_texture = 0,
	IRR_OES_compressed_paletted_texture,
	IRR_OES_depth24,
	IRR_OES_depth32,
	IRR_OES_depth_texture,
	IRR_OES_EIRR_image,
	IRR_OES_EIRR_image_external,
	IRR_OES_element_index_uint,
	IRR_OES_fbo_render_mipmap,
	IRR_OES_fragment_precision_high,
	IRR_OES_get_program_binary,
	IRR_OES_mapbuffer,
	IRR_OES_packed_depth_stencil,		
	IRR_OES_required_internalformat,  
	IRR_OES_rgb8_rgba8,
	IRR_OES_standard_derivatives,
	IRR_OES_stencil1,
	IRR_OES_stencil4,
	IRR_OES_texture_3D,
	IRR_OES_texture_float,
	IRR_OES_texture_float_linear,
	IRR_OES_texture_half_float,
	IRR_OES_texture_half_float_linear,
	IRR_OES_texture_npot,
	IRR_OES_vertex_array_object,
	IRR_OES_vertex_half_float,
	IRR_OES_vertex_type_10_10_10_2,

	IRR_AMD_compressed_3DC_texture,
	IRR_AMD_compressed_ATC_texture,
	IRR_AMD_performance_monitor,
	IRR_AMD_program_binary_Z400,

	IRR_ANGLE_depth_texture,
	IRR_ANGLE_framebuffer_blit,
	IRR_ANGLE_framebuffer_multisample,
	IRR_ANGLE_instanced_arrays,
	IRR_ANGLE_pack_reverse_row_order,
	IRR_ANGLE_program_binary,
	IRR_ANGLE_texture_compression_dxt3,
	IRR_ANGLE_texture_compression_dxt5,
	IRR_ANGLE_texture_usage,
	IRR_ANGLE_translated_shader_source,
	IRR_APPLE_copy_texture_levels,
	IRR_APPLE_framebuffer_multisample,
	IRR_APPLE_rgb_422,
	IRR_APPLE_sync,
	IRR_APPLE_texture_format_BGRA8888,
	IRR_APPLE_texture_max_level,

	IRR_ARM_mali_program_binary,
	IRR_ARM_mali_shader_binary,
	IRR_ARM_rgba8,

	IRR_EXT_blend_minmax,
	IRR_EXT_color_buffer_half_float,
	IRR_EXT_debug_label,
	IRR_EXT_debug_marker,
	IRR_EXT_discard_framebuffer,
	IRR_EXT_map_buffer_range,
	IRR_EXT_multisampled_render_to_texture,
	IRR_EXT_multiview_draw_buffers,
	IRR_EXT_multi_draw_arrays,
	IRR_EXT_occlusion_query_boolean,
	IRR_EXT_read_format_bgra,
	IRR_EXT_robustness,
	IRR_EXT_separate_shader_objects,
	IRR_EXT_shader_framebuffer_fetch,
	IRR_EXT_shader_texture_lod,
	IRR_EXT_shadow_samplers,
	IRR_EXT_sRGB,

	IRR_EXT_texture_compression_dxt1,
	IRR_EXT_texture_filter_anisotropic,
	IRR_EXT_texture_format_BGRA8888,
	IRR_EXT_texture_rg,
	IRR_EXT_texture_storage,
	IRR_EXT_texture_type_2_10_10_10_REV,
	IRR_EXT_unpack_subimage,

	IRR_DMP_shader_binary,

	IRR_FJ_shader_binary_GCCSO,

	IRR_IMG_program_binary,
	IRR_IMG_read_format,
	IRR_IMG_shader_binary,
	IRR_IMG_texture_compression_pvrtc,
	IRR_IMG_texture_compression_pvrtc2,
	IRR_IMG_multisampled_render_to_texture,

	IRR_NV_coverage_sample,
	IRR_NV_depth_nonlinear,
	IRR_NV_draw_buffers,
	IRR_NV_draw_instanced,
	IRR_NV_fbo_color_attachments,
	IRR_NV_fence,
	IRR_NV_framebuffer_blit,
	IRR_NV_framebuffer_multisample,
	IRR_NV_generate_mipmap_sRGB,
	IRR_NV_instanced_arrays,
	IRR_NV_read_buffer,
	IRR_NV_read_buffer_front,
	IRR_NV_read_depth,
	IRR_NV_read_depth_stencil,
	IRR_NV_read_stencil,
	IRR_NV_shadow_samplers_array,
	IRR_NV_shadow_samplers_cube,
	IRR_NV_sRGB_formats,
	IRR_NV_texture_border_clamp,
	IRR_NV_texture_compression_s3tc_update,
	IRR_NV_texture_npot_2D_mipmap,

	IRR_QCOM_alpha_test,
	IRR_QCOM_binning_control,
	IRR_QCOM_driver_control,
	IRR_QCOM_extended_get,
	IRR_QCOM_extended_get2,
	IRR_QCOM_perfmon_global_mode,
	IRR_QCOM_writeonly_rendering,
	IRR_QCOM_tiled_rendering,

	IRR_VIV_shader_binary,

	IRR_GLES2_Feature_Count
};

class CGLES2Extension
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2Extension);

public:
	CGLES2Extension();
	~CGLES2Extension();

public:
	bool initExtensions();

	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	bool queryOpenGLFeature(EGLES2Features feature) const
	{
		return FeatureAvailable[feature];
	}

	bool canUseVAO() const;

	bool checkFBOStatus();

public:

#ifndef MW_PLATFORM_IOS
	static GLboolean extGlSwapInterval(EGLDisplay dpy, EGLint interval);
	static GLboolean extGlSwapBuffers(EGLDisplay dpy, EGLSurface surface);
#endif
    
	//general
	void extGlActiveTexture(GLenum texture);

	void extGlCompressedTexImage2D(GLenum target, GLint level,
		GLenum internalformat, GLsizei width, GLsizei height,
		GLint border, GLsizei imageSize, const void* data);
	void extGlCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);

	//shader 
	GLuint extGlCreateShader(GLenum type);
	void extGlShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
	void	extGlCompileShader(GLuint shader);
	void extGlGetShaderiv(GLuint shader, GLenum pname, GLint* params);
	void	extGlGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
	void extGlDeleteShader(GLuint shader);

	GLuint extGlCreateProgram();
	void extGlAttachShader(GLuint program, GLuint shader);
	void extGlBindAttribLocation(GLuint program, GLuint index, const char *name);
	int extGlGetAttribLocation(GLuint program, const GLchar *name);
	void extGlLinkProgram(GLuint program);
	void extGlUseProgram(GLuint program);
	void extGlDeleteProgram(GLuint program);
	void extGlGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);

	void extGlGetProgramiv(GLuint program, GLenum pname, GLint* params);
	void extGlGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);

	void extGlGetActiveUniform(GLuint program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	GLint extGlGetUniformLocation(GLuint program, const char *name);
	void extGlUniform4fv(GLint location, GLsizei count, const GLfloat *v);
	void extGlUniform1iv(GLint loc, GLsizei count, const GLint *v);
	void extGlUniform1fv(GLint loc, GLsizei count, const GLfloat *v);
	void extGlUniform2fv(GLint loc, GLsizei count, const GLfloat *v);
	void extGlUniform3fv(GLint loc, GLsizei count, const GLfloat *v);
	void extGlUniformMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void extGlUniformMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void extGlUniformMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);

	// framebuffer objects
	void extGlBindFramebuffer(GLenum target, GLuint framebuffer);
	void extGlDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
	void extGlGenFramebuffers(GLsizei n, GLuint *framebuffers);
	GLenum extGlCheckFramebufferStatus(GLenum target);
	void extGlFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void extGlBindRenderbuffer(GLenum target, GLuint renderbuffer);
	void extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
	void extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
	void extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

	// vertex buffer object
	void extGlGenBuffers(GLsizei n, GLuint *buffers);
	void extGlBindBuffer(GLenum target, GLuint buffer);
	void extGlBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
	void extGlDeleteBuffers(GLsizei n, const GLuint *buffers);
	void extGlBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
	GLboolean extGlIsBuffer (GLuint buffer);

	void extGlDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

	//
	void extGlGenVertexArrays(GLsizei n, GLuint *arrays);
	void extGlDeleteVertexArrays(GLsizei n, const GLuint *arrays);
	void extGlBindVertexArray(GLuint array);
	void extGlEnableVertexAttribArray(GLuint index);
	void extGlDisableVertexAttribArray(GLuint index);
	void extGlVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

	//
	void extGlDiscardFramebufferEXT (GLenum target, GLsizei numAttachments, const GLenum *attachments);

	void extGlRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	void extGlFramebufferTexture2DMultisample (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);
	void extGlResolveMultisampleFramebuffer();

private:
	bool initExtFunctions();

private:
    
#ifndef MW_PLATFORM_IOS
    __eglMustCastToProperFunctionPointerType getProcAddress(const c8* funcname);
    
	PFNGLBINDVERTEXARRAYOESPROC pGlBindVertexArrayOES;
	PFNGLDELETEVERTEXARRAYSOESPROC pGlDeleteVertexArrayOES;
	PFNGLGENVERTEXARRAYSOESPROC	pGlGenVertexArrayOES;
	PFNGLISVERTEXARRAYOESPROC pGlIsVertexArrayOES;
	PFNGLDISCARDFRAMEBUFFEREXTPROC pGlDiscardFrameBufferEXT;
    
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC pGlRenderbufferStorageMultisampleEXT;
	PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC pGlFramebufferTexture2DMultisampleEXT;
    
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC pGlRenderbufferStorageMultisampleIMG;
	PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC pGlFramebufferTexture2DMultisampleIMG;
#endif
    

public:
	GLfloat DimAliasedLine[2];
	GLfloat DimAliasedPoint[2];

	u32 MaxVertexShaderConst;
	u32 MaxPixelShaderConst;
	u32 MaxTextureSize;

	u16 Version;
	u16 ShaderLanguageVersion;

	u8 MaxTextureUnits;
	u8 MaxAnisotropy;
	u8 MaxMultiSample;

	bool TextureCompressionS3;
	bool TextureCompressionPVR;
	bool TextureCompressionPVR2;
	bool TextureCompressionETC1;
	bool TextureCompressionATC;
	bool TextureCompression3DC;

	bool TextureCompression;
	bool	MultisampleSupported;
	bool DiscardFrameSupported;

private:
	bool FeatureAvailable[IRR_GLES2_Feature_Count];

};

#endif