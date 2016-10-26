#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "base.h"
#include "GL/glext.h"

enum EOpenGLFeatures
{
	IRR_3DFX_multisample = 0,
	IRR_3DFX_tbuffer,
	IRR_3DFX_texture_compression_FXT1,
	IRR_AMD_draw_buffers_blend,
	IRR_AMD_performance_monitor,
	IRR_AMD_texture_texture4,
	IRR_AMD_vertex_shader_tesselator,
	IRR_APPLE_aux_depth_stencil,
	IRR_APPLE_client_storage,
	IRR_APPLE_element_array,
	IRR_APPLE_fence,
	IRR_APPLE_float_pixels,
	IRR_APPLE_flush_buffer_range,
	IRR_APPLE_object_purgeable,
	IRR_APPLE_rgb_422,
	IRR_APPLE_row_bytes,
	IRR_APPLE_specular_vector,
	IRR_APPLE_texture_range,
	IRR_APPLE_transform_hint,
	IRR_APPLE_vertex_array_object,
	IRR_APPLE_vertex_array_range,
	IRR_APPLE_vertex_program_evaluators,
	IRR_APPLE_ycbcr_422,
	IRR_ARB_color_buffer_float,
	IRR_ARB_compatibility,
	IRR_ARB_copy_buffer,
	IRR_ARB_depth_buffer_float,
	IRR_ARB_depth_clamp,
	IRR_ARB_depth_texture,
	IRR_ARB_draw_buffers,
	IRR_ARB_draw_buffers_blend,
	IRR_ARB_draw_elements_base_vertex,
	IRR_ARB_draw_instanced,
	IRR_ARB_fragment_coord_conventions,
	IRR_ARB_fragment_program,
	IRR_ARB_fragment_program_shadow,
	IRR_ARB_fragment_shader,
	IRR_ARB_framebuffer_object,
	IRR_ARB_framebuffer_sRGB,
	IRR_ARB_geometry_shader4,
	IRR_ARB_half_float_pixel,
	IRR_ARB_half_float_vertex,
	IRR_ARB_imaging,
	IRR_ARB_instanced_arrays,
	IRR_ARB_map_buffer_range,
	IRR_ARB_matrix_palette,
	IRR_ARB_multisample,
	IRR_ARB_multitexture,
	IRR_ARB_occlusion_query,
	IRR_ARB_pixel_buffer_object,
	IRR_ARB_point_parameters,
	IRR_ARB_point_sprite,
	IRR_ARB_provoking_vertex,
	IRR_ARB_sample_shading,
	IRR_ARB_seamless_cube_map,
	IRR_ARB_shader_objects,
	IRR_ARB_shader_texture_lod,
	IRR_ARB_shading_language_100,
	IRR_ARB_shadow,
	IRR_ARB_shadow_ambient,
	IRR_ARB_sync,
	IRR_ARB_texture_border_clamp,
	IRR_ARB_texture_buffer_object,
	IRR_ARB_texture_compression,
	IRR_ARB_texture_compression_rgtc,
	IRR_ARB_texture_cube_map,
	IRR_ARB_texture_cube_map_array,
	IRR_ARB_texture_env_add,
	IRR_ARB_texture_env_combine,
	IRR_ARB_texture_env_crossbar,
	IRR_ARB_texture_env_dot3,
	IRR_ARB_texture_float,
	IRR_ARB_texture_gather,
	IRR_ARB_texture_mirrored_repeat,
	IRR_ARB_texture_multisample,
	IRR_ARB_texture_non_power_of_two,
	IRR_ARB_texture_query_lod,
	IRR_ARB_texture_rectangle,
	IRR_ARB_texture_rg,
	IRR_ARB_transpose_matrix,
	IRR_ARB_uniform_buffer_object,
	IRR_ARB_vertex_array_bgra,
	IRR_ARB_vertex_array_object,
	IRR_ARB_vertex_blend,
	IRR_ARB_vertex_buffer_object,
	IRR_ARB_vertex_program,
	IRR_ARB_vertex_shader,
	IRR_ARB_window_pos,
	IRR_ATI_draw_buffers,
	IRR_ATI_element_array,
	IRR_ATI_envmap_bumpmap,
	IRR_ATI_fragment_shader,
	IRR_ATI_map_object_buffer,
	IRR_ATI_meminfo,
	IRR_ATI_pixel_format_float,
	IRR_ATI_pn_triangles,
	IRR_ATI_separate_stencil,
	IRR_ATI_text_fragment_shader,
	IRR_ATI_texture_env_combine3,
	IRR_ATI_texture_float,
	IRR_ATI_texture_mirror_once,
	IRR_ATI_vertex_array_object,
	IRR_ATI_vertex_attrib_array_object,
	IRR_ATI_vertex_streams,
	IRR_EXT_422_pixels,
	IRR_EXT_abgr,
	IRR_EXT_bgra,
	IRR_EXT_bindable_uniform,
	IRR_EXT_blend_color,
	IRR_EXT_blend_equation_separate,
	IRR_EXT_blend_func_separate,
	IRR_EXT_blend_logic_op,
	IRR_EXT_blend_minmax,
	IRR_EXT_blend_subtract,
	IRR_EXT_clip_volume_hint,
	IRR_EXT_cmyka,
	IRR_EXT_color_subtable,
	IRR_EXT_compiled_vertex_array,
	IRR_EXT_convolution,
	IRR_EXT_coordinate_frame,
	IRR_EXT_copy_texture,
	IRR_EXT_cull_vertex,
	IRR_EXT_depth_bounds_test,
	IRR_EXT_direct_state_access,
	IRR_EXT_draw_buffers2,
	IRR_EXT_draw_instanced,
	IRR_EXT_draw_range_elements,
	IRR_EXT_fog_coord,
	IRR_EXT_framebuffer_blit,
	IRR_EXT_framebuffer_multisample,
	IRR_EXT_framebuffer_object,
	IRR_EXT_framebuffer_sRGB,
	IRR_EXT_geometry_shader4,
	IRR_EXT_gpu_program_parameters,
	IRR_EXT_gpu_shader4,
	IRR_EXT_histogram,
	IRR_EXT_index_array_formats,
	IRR_EXT_index_func,
	IRR_EXT_index_material,
	IRR_EXT_index_texture,
	IRR_EXT_light_texture,
	IRR_EXT_misc_attribute,
	IRR_EXT_multi_draw_arrays,
	IRR_EXT_multisample,
	IRR_EXT_packed_depth_stencil,
	IRR_EXT_packed_float,
	IRR_EXT_packed_pixels,
	IRR_EXT_paletted_texture,
	IRR_EXT_pixel_buffer_object,
	IRR_EXT_pixel_transform,
	IRR_EXT_pixel_transform_color_table,
	IRR_EXT_point_parameters,
	IRR_EXT_polygon_offset,
	IRR_EXT_provoking_vertex,
	IRR_EXT_rescale_normal,
	IRR_EXT_secondary_color,
	IRR_EXT_separate_shader_objects,
	IRR_EXT_separate_specular_color,
	IRR_EXT_shadow_funcs,
	IRR_EXT_shared_texture_palette,
	IRR_EXT_stencil_clear_tag,
	IRR_EXT_stencil_two_side,
	IRR_EXT_stencil_wrap,
	IRR_EXT_subtexture,
	IRR_EXT_texture,
	IRR_EXT_texture3D,
	IRR_EXT_texture_array,
	IRR_EXT_texture_buffer_object,
	IRR_EXT_texture_compression_latc,
	IRR_EXT_texture_compression_rgtc,
	IRR_EXT_texture_compression_s3tc,
	IRR_EXT_texture_cube_map,
	IRR_EXT_texture_env_add,
	IRR_EXT_texture_env_combine,
	IRR_EXT_texture_env_dot3,
	IRR_EXT_texture_filter_anisotropic,
	IRR_EXT_texture_integer,
	IRR_EXT_texture_lod_bias,
	IRR_EXT_texture_mirror_clamp,
	IRR_EXT_texture_object,
	IRR_EXT_texture_perturb_normal,
	IRR_EXT_texture_shared_exponent,
	IRR_EXT_texture_snorm,
	IRR_EXT_texture_sRGB,
	IRR_EXT_texture_swizzle,
	IRR_EXT_timer_query,
	IRR_EXT_transform_feedback,
	IRR_EXT_vertex_array,
	IRR_EXT_vertex_array_bgra,
	IRR_EXT_vertex_shader,
	IRR_EXT_vertex_weighting,
	IRR_FfdMaskSGIX,
	IRR_GREMEDY_frame_terminator,
	IRR_GREMEDY_string_marker,
	IRR_HP_convolution_border_modes,
	IRR_HP_image_transform,
	IRR_HP_occlusion_test,
	IRR_HP_texture_lighting,
	IRR_IBM_cull_vertex,
	IRR_IBM_multimode_draw_arrays,
	IRR_IBM_rasterpos_clip,
	IRR_IBM_texture_mirrored_repeat,
	IRR_IBM_vertex_array_lists,
	IRR_INGR_blend_func_separate,
	IRR_INGR_color_clamp,
	IRR_INGR_interlace_read,
	IRR_INGR_palette_buffer,
	IRR_INTEL_parallel_arrays,
	IRR_INTEL_texture_scissor,
	IRR_MESA_pack_invert,
	IRR_MESA_resize_buffers,
	IRR_MESA_window_pos,
	IRR_MESAX_texture_stack,
	IRR_MESA_ycbcr_texture,
	IRR_NV_blend_square,
	IRR_NV_conditional_render,
	IRR_NV_copy_depth_to_color,
	IRR_NV_copy_image,
	IRR_NV_depth_buffer_float,
	IRR_NV_depth_clamp,
	IRR_NV_evaluators,
	IRR_NV_explicit_multisample,
	IRR_NV_fence,
	IRR_NV_float_buffer,
	IRR_NV_fog_distance,
	IRR_NV_fragment_program,
	IRR_NV_fragment_program2,
	IRR_NV_fragment_program4,
	IRR_NV_fragment_program_option,
	IRR_NV_framebuffer_multisample_coverage,
	IRR_NV_geometry_program4,
	IRR_NV_geometry_shader4,
	IRR_NV_gpu_program4,
	IRR_NV_half_float,
	IRR_NV_light_max_exponent,
	IRR_NV_multisample_filter_hint,
	IRR_NV_occlusion_query,
	IRR_NV_packed_depth_stencil,
	IRR_NV_parameter_buffer_object,
	IRR_NV_parameter_buffer_object2,
	IRR_NV_pixel_data_range,
	IRR_NV_point_sprite,
	IRR_NV_present_video,
	IRR_NV_primitive_restart,
	IRR_NV_register_combiners,
	IRR_NV_register_combiners2,
	IRR_NV_shader_buffer_load,
	IRR_NV_texgen_emboss,
	IRR_NV_texgen_reflection,
	IRR_NV_texture_barrier,
	IRR_NV_texture_compression_vtc,
	IRR_NV_texture_env_combine4,
	IRR_NV_texture_expand_normal,
	IRR_NV_texture_rectangle,
	IRR_NV_texture_shader,
	IRR_NV_texture_shader2,
	IRR_NV_texture_shader3,
	IRR_NV_transform_feedback,
	IRR_NV_transform_feedback2,
	IRR_NV_vertex_array_range,
	IRR_NV_vertex_array_range2,
	IRR_NV_vertex_buffer_unified_memory,
	IRR_NV_vertex_program,
	IRR_NV_vertex_program1_1,
	IRR_NV_vertex_program2,
	IRR_NV_vertex_program2_option,
	IRR_NV_vertex_program3,
	IRR_NV_vertex_program4,
	IRR_NV_video_capture,
	IRR_OES_read_format,
	IRR_OML_interlace,
	IRR_OML_resample,
	IRR_OML_subsample,
	IRR_PGI_misc_hints,
	IRR_PGI_vertex_hints,
	IRR_REND_screen_coordinates,
	IRR_S3_s3tc,
	IRR_SGI_color_matrix,
	IRR_SGI_color_table,
	IRR_SGI_depth_pass_instrument,
	IRR_SGIS_detail_texture,
	IRR_SGIS_fog_function,
	IRR_SGIS_generate_mipmap,
	IRR_SGIS_multisample,
	IRR_SGIS_pixel_texture,
	IRR_SGIS_point_line_texgen,
	IRR_SGIS_point_parameters,
	IRR_SGIS_sharpen_texture,
	IRR_SGIS_texture4D,
	IRR_SGIS_texture_border_clamp,
	IRR_SGIS_texture_color_mask,
	IRR_SGIS_texture_edge_clamp,
	IRR_SGIS_texture_filter4,
	IRR_SGIS_texture_lod,
	IRR_SGIS_texture_select,
	IRR_SGI_texture_color_table,
	IRR_SGIX_async,
	IRR_SGIX_async_histogram,
	IRR_SGIX_async_pixel,
	IRR_SGIX_blend_alpha_minmax,
	IRR_SGIX_calligraphic_fragment,
	IRR_SGIX_clipmap,
	IRR_SGIX_convolution_accuracy,
	IRR_SGIX_depth_pass_instrument,
	IRR_SGIX_depth_texture,
	IRR_SGIX_flush_raster,
	IRR_SGIX_fog_offset,
	IRR_SGIX_fog_scale,
	IRR_SGIX_fragment_lighting,
	IRR_SGIX_framezoom,
	IRR_SGIX_igloo_interface,
	IRR_SGIX_impact_pixel_texture,
	IRR_SGIX_instruments,
	IRR_SGIX_interlace,
	IRR_SGIX_ir_instrument1,
	IRR_SGIX_list_priority,
	IRR_SGIX_pixel_texture,
	IRR_SGIX_pixel_tiles,
	IRR_SGIX_polynomial_ffd,
	IRR_SGIX_reference_plane,
	IRR_SGIX_resample,
	IRR_SGIX_scalebias_hint,
	IRR_SGIX_shadow,
	IRR_SGIX_shadow_ambient,
	IRR_SGIX_sprite,
	IRR_SGIX_subsample,
	IRR_SGIX_tag_sample_buffer,
	IRR_SGIX_texture_add_env,
	IRR_SGIX_texture_coordinate_clamp,
	IRR_SGIX_texture_lod_bias,
	IRR_SGIX_texture_multi_buffer,
	IRR_SGIX_texture_scale_bias,
	IRR_SGIX_texture_select,
	IRR_SGIX_vertex_preclip,
	IRR_SGIX_ycrcb,
	IRR_SGIX_ycrcba,
	IRR_SGIX_ycrcb_subsample,
	IRR_SUN_convolution_border_modes,
	IRR_SUN_global_alpha,
	IRR_SUN_mesh_array,
	IRR_SUN_slice_accum,
	IRR_SUN_triangle_list,
	IRR_SUN_vertex,
	IRR_SUNX_constant_data,
	IRR_WIN_phong_shading,
	IRR_WIN_specular_fog,
	IRR_OpenGL_Feature_Count
};

class COpenGLExtension
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLExtension);

public:
	COpenGLExtension();
	~COpenGLExtension();

public:
	bool initExtensions();

	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	bool queryOpenGLFeature(EOpenGLFeatures feature) const
	{
		return FeatureAvailable[feature];
	}

	bool canUseVAO() const;

	bool checkFBOStatus();

public:
	//general
	void extGlActiveTexture(GLenum texture);

	void extGlCompressedTexImage2D(GLenum target, GLint level,
		GLenum internalformat, GLsizei width, GLsizei height,
		GLint border, GLsizei imageSize, const void* data);
	void extGlCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);

	//shader 
	/*
	void extGlGenPrograms(GLsizei n, GLuint *programs);
	void extGlBindProgram(GLenum target, GLuint program);
	void extGlProgramString(GLenum target, GLenum format, GLsizei len, const GLvoid *string);
	void extGlLoadProgram(GLenum target, GLuint id, GLsizei len, const GLubyte *string);
	void extGlDeletePrograms(GLsizei n, const GLuint *programs);
	*/

	GLhandleARB extGlCreateShaderObject(GLenum shaderType);
	void extGlShaderSourceARB(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings);
	void extGlCompileShaderARB(GLhandleARB shader);
	void extGlGetObjectParameteriv(GLhandleARB object, GLenum type, GLint *param);
	void extGlGetInfoLog(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);

	GLhandleARB extGlCreateProgramObject(void);
	void extGlAttachObject(GLhandleARB program, GLhandleARB shader);
	void extGlBindFragDataLocation(GLuint program, GLuint color, const char *name);
	void extGlLinkProgramARB(GLhandleARB program);
	void extGlUseProgramObject(GLhandleARB prog);
	void extGlDeleteObject(GLhandleARB object);
	void extGlGetAttachedObjects(GLhandleARB program, GLsizei maxcount, GLsizei* count, GLhandleARB* shaders);

	void extGlGetActiveUniformARB(GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
	GLint extGlGetUniformLocationARB(GLhandleARB program, const char *name);
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
	void extGlBlitFramebufferEXT (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	void extGlBindRenderbuffer(GLenum target, GLuint renderbuffer);
	void extGlDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
	void extGlGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
	void extGlRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void extGlRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	void extGlGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params);
	void extGlFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void extGlInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments);

	// vertex buffer object
	void extGlGenBuffers(GLsizei n, GLuint *buffers);
	void extGlBindBuffer(GLenum target, GLuint buffer);
	void extGlBufferData(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
	void extGlDeleteBuffers(GLsizei n, const GLuint *buffers);
	void extGlBufferSubData (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);

	//
	void extGlDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
	void extGlDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
	void extGlDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex);
	void extGlDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex);

	void extGlGenVertexArrays(GLsizei n, GLuint *arrays);
	void extGlDeleteVertexArrays(GLsizei n, const GLuint *arrays);
	void extGlBindVertexArray(GLuint array);
	void extGlEnableVertexAttribArrayARB(GLuint index);
	void extGlDisableVertexAttribArrayARB(GLuint index);
	void extGlVertexAttribPointerARB(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
	GLint extGlGetAttribLocationARB(GLhandleARB programObj, const GLcharARB *name);

	void extGlTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);

private:
	bool initExtFunctions();

	void* getProcAddress(const c8* funcname);

private:
	PFNGLACTIVETEXTUREARBPROC pGlActiveTextureARB;
	PFNGLCLIENTACTIVETEXTUREARBPROC		pGlClientActiveTextureARB;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC		pGlCompressedTexImage2D;
	PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC		pGlCompressedTexSubImage2D;

	PFNGLGENPROGRAMSARBPROC pGlGenProgramsARB;
	PFNGLGENPROGRAMSNVPROC pGlGenProgramsNV;
	PFNGLBINDPROGRAMARBPROC pGlBindProgramARB;
	PFNGLBINDPROGRAMNVPROC pGlBindProgramNV;
	PFNGLDELETEPROGRAMSARBPROC pGlDeleteProgramsARB;
	PFNGLDELETEPROGRAMSNVPROC pGlDeleteProgramsNV;
	PFNGLPROGRAMSTRINGARBPROC pGlProgramStringARB;
	PFNGLLOADPROGRAMNVPROC pGlLoadProgramNV;
	PFNGLPROGRAMLOCALPARAMETER4FVARBPROC pGlProgramLocalParameter4fvARB;
	PFNGLCREATESHADEROBJECTARBPROC pGlCreateShaderObjectARB;
	PFNGLSHADERSOURCEARBPROC pGlShaderSourceARB;
	PFNGLCOMPILESHADERARBPROC pGlCompileShaderARB;
	PFNGLCREATEPROGRAMOBJECTARBPROC pGlCreateProgramObjectARB;
	PFNGLBINDFRAGDATALOCATIONPROC	pGlBindFragDataLocation;
	PFNGLATTACHOBJECTARBPROC pGlAttachObjectARB;
	PFNGLLINKPROGRAMARBPROC pGlLinkProgramARB;
	PFNGLUSEPROGRAMOBJECTARBPROC pGlUseProgramObjectARB;
	PFNGLDELETEOBJECTARBPROC pGlDeleteObjectARB;
	PFNGLGETATTACHEDOBJECTSARBPROC pGlGetAttachedObjectsARB;
	PFNGLGETINFOLOGARBPROC pGlGetInfoLogARB;
	PFNGLGETOBJECTPARAMETERIVARBPROC pGlGetObjectParameterivARB;
	PFNGLGETUNIFORMLOCATIONARBPROC pGlGetUniformLocationARB;
	PFNGLUNIFORM1IVARBPROC pGlUniform1ivARB;
	PFNGLUNIFORM1FVARBPROC pGlUniform1fvARB;
	PFNGLUNIFORM2FVARBPROC pGlUniform2fvARB;
	PFNGLUNIFORM3FVARBPROC pGlUniform3fvARB;
	PFNGLUNIFORM4FVARBPROC pGlUniform4fvARB;
	PFNGLUNIFORMMATRIX2FVARBPROC pGlUniformMatrix2fvARB;
	PFNGLUNIFORMMATRIX3FVARBPROC pGlUniformMatrix3fvARB;
	PFNGLUNIFORMMATRIX4FVARBPROC pGlUniformMatrix4fvARB;
	PFNGLUNIFORMMATRIX3X4FVPROC	pGlUniformMatrix3x4;
	PFNGLGETACTIVEUNIFORMARBPROC pGlGetActiveUniformARB;
	PFNGLPROGRAMPARAMETERIARBPROC pGlProgramParameteriARB;
	PFNGLPROGRAMPARAMETERIEXTPROC pGlProgramParameteriEXT;
	PFNGLGETPROGRAMBINARYPROC pGlGetProgramBinary;
	PFNGLPROGRAMBINARYPROC	pGlProgramBinary;

	PFNGLPOINTPARAMETERFARBPROC  pGlPointParameterfARB;
	PFNGLPOINTPARAMETERFVARBPROC pGlPointParameterfvARB;
	PFNGLSTENCILFUNCSEPARATEPROC pGlStencilFuncSeparate;
	PFNGLSTENCILOPSEPARATEPROC pGlStencilOpSeparate;
	PFNGLBINDFRAMEBUFFEREXTPROC pGlBindFramebufferEXT;
	PFNGLDELETEFRAMEBUFFERSEXTPROC pGlDeleteFramebuffersEXT;
	PFNGLGENFRAMEBUFFERSEXTPROC pGlGenFramebuffersEXT;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC pGlCheckFramebufferStatusEXT;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC pGlFramebufferTexture2DEXT;
	PFNGLBLITFRAMEBUFFEREXTPROC pGlBlitFramebufferEXT;
	PFNGLBINDRENDERBUFFEREXTPROC pGlBindRenderbufferEXT;
	PFNGLDELETERENDERBUFFERSEXTPROC pGlDeleteRenderbuffersEXT;
	PFNGLGENRENDERBUFFERSEXTPROC pGlGenRenderbuffersEXT;
	PFNGLRENDERBUFFERSTORAGEEXTPROC pGlRenderbufferStorageEXT;
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC pGlRenderBufferStorageMultisampleEXT;
	PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC pGlGetRenderBufferParameterivEXT;
	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC pGlFramebufferRenderbufferEXT;
	PFNGLINVALIDATEFRAMEBUFFERPROC			pGlInvalidateFramebuffer;
	PFNGLDRAWBUFFERSARBPROC pGlDrawBuffersARB;
	PFNGLDRAWBUFFERSATIPROC pGlDrawBuffersATI;
	PFNGLGENBUFFERSARBPROC pGlGenBuffersARB;
	PFNGLBINDBUFFERARBPROC pGlBindBufferARB;
	PFNGLBUFFERDATAARBPROC pGlBufferDataARB;
	PFNGLDELETEBUFFERSARBPROC pGlDeleteBuffersARB;
	PFNGLBUFFERSUBDATAARBPROC pGlBufferSubDataARB;
	PFNGLGETBUFFERSUBDATAARBPROC pGlGetBufferSubDataARB;
	PFNGLMAPBUFFERARBPROC pGlMapBufferARB;
	PFNGLUNMAPBUFFERARBPROC pGlUnmapBufferARB;
	PFNGLISBUFFERARBPROC pGlIsBufferARB;
	PFNGLGETBUFFERPARAMETERIVARBPROC pGlGetBufferParameterivARB;
	PFNGLGETBUFFERPOINTERVARBPROC pGlGetBufferPointervARB;
	PFNGLPROVOKINGVERTEXPROC pGlProvokingVertexARB;
	PFNGLPROVOKINGVERTEXEXTPROC pGlProvokingVertexEXT;
	PFNGLCOLORMASKINDEXEDEXTPROC pGlColorMaskIndexedEXT;
	PFNGLENABLEINDEXEDEXTPROC pGlEnableIndexedEXT;
	PFNGLDISABLEINDEXEDEXTPROC pGlDisableIndexedEXT;

	PFNGLDRAWRANGEELEMENTSPROC pGlDrawRangeElements;
	PFNGLDRAWELEMENTSBASEVERTEXPROC	pGlDrawElementsBaseVertex;
	PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC	pGlDrawRangeElementsBaseVertex;

	PFNGLGENVERTEXARRAYSPROC		pGlGenVertexArrays;
	PFNGLDELETEVERTEXARRAYSPROC	pGlDeleteVertexArrays;
	PFNGLBINDVERTEXARRAYPROC		pGlBindVertexArray;
	PFNGLENABLEVERTEXATTRIBARRAYARBPROC		pGlEnableVertexAttribArrayARB;
	PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	pGlDisableVertexAttribArrayARB;
	PFNGLVERTEXATTRIBPOINTERARBPROC	pGlVertexAttribPointerARB;
	PFNGLGETATTRIBLOCATIONARBPROC		pGlGetAttribLocationARB;

	PFNGLTEXIMAGE2DMULTISAMPLEPROC		pGlTexImage2DMultisample;

public:

	f32 MaxTextureLODBias;
	GLfloat DimAliasedLine[2];
	GLfloat DimAliasedPoint[2];

	u32 MaxVertexShaderConst;
	u32 MaxPixelShaderConst;
	u32 MaxTextureSize;

	u16 Version;
	u16 ShaderLanguageVersion;

	u8 MaxLights;
	u8 MaxUserClipPlanes;
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
	bool FeatureAvailable[IRR_OpenGL_Feature_Count];
};

#endif