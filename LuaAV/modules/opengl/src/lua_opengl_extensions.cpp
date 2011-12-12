#include "lua_opengl_extensions.h"
#include <stdio.h>

#define CHECK_EXTENSION(ext)	GLEW_##ext

Extensions :: Extensions()
: // OpenGL version
	major_version(0),
	minor_version(0),
	release_number(0),
	
	glsl_major_version(0),
	glsl_minor_version(0),
	
	// Shaders
	ext_fragment_shader(false),
	ext_fragment_program(false),
	ext_shader_objects(false),
	ext_shading_language_100(false),
	ext_vertex_program(false),
	ext_vertex_shader(false),
	ext_geometry_shader4(false),
	ext_gpu_shader4(false),
	
	
	// Buffer objects
	ext_framebuffer_object(false),
	ext_pixel_buffer_object(false),
	ext_vertex_buffer_object(false),
	ext_vertex_array(false),
	
	// FBOs
	ext_draw_buffers(false),
	ext_framebuffer_blit(false),
	ext_framebuffer_multisample(false),
	ext_framebuffer_sRGB(false),
	
	// VBOs
	ext_draw_range_elements(false),
	ext_multi_draw_arrays(false),
	
	// textures
	ext_depth_textures(false),
	ext_multitexture(false),
	ext_shadow(false),
	ext_texture_border_clamp(false),
	ext_texture_compression(false),
	ext_texture_cube_map(false),
	ext_texture_float(false),
	ext_texture_mirrored_repeat(false),
	ext_texture_non_power_of_two(false),
	ext_texture_rectangle(false),
	ext_subtexture(false),
	ext_texture(false),
	ext_texture3D(false),
	ext_texture_array(false),
	ext_texture_compression_dxt1(false),
	ext_texture_compression_latc(false),
	ext_texture_compression_rgtc(false),
	ext_texture_compression_s3tc(false),
	ext_texture_edge_clamp(false),
	ext_texture_integer(false),
	ext_texture_mirror_clamp(false),
	ext_texture_sRGB(false),

	// Pixels
	ext_multisample(false),
	ext_color_buffer_float(false),
	ext_half_float_pixel(false),
	ext_422_pixels(false),
	ext_abgr(false),
	ext_bgra(false),
	ext_packed_depth_stencil(false),
	ext_packed_float(false),
	ext_packed_pixels(false),
	ext_half_float(false),


// Rendering
	// Point sprites
	ext_point_parameters(false),
	ext_point_sprite(false),
	
	// Blending
	ext_blend_color(false),
	ext_blend_equation_separate(false),
	ext_blend_func_separate(false),
	ext_blend_logic_op(false),
	ext_blend_minmax(false),
	ext_blend_subtract(false),
	
	// Fog
	ext_fog_coord(false),

	// Apple
	ext_client_storage(false),
	ext_ycbcr_422(false),
	
	// Windows
	ext_swap_hint(false)
{}

Extensions :: ~Extensions()
{}


void Extensions :: vertify_extensions() {
	if(glewInit() != GLEW_OK) {
		printf("ERROR: GLEW failed to initialize.  Any use of OpenGL extensions will cause a crash\n");
		return;
	}

	lua_opengl_error("verifying extensions: invaid context (context probably not ready yet)");

	vendor.assign( (const char *)glGetString(GL_VENDOR) );
	renderer.assign( (const char *)glGetString(GL_RENDERER) );
	version.assign( (const char *)glGetString(GL_VERSION) );
	sscanf(version.data(), "%d.%d.%d", &major_version, &minor_version, &release_number);
	
	ext_fragment_shader =		CHECK_EXTENSION(ARB_fragment_shader);
	ext_fragment_program =		CHECK_EXTENSION(ARB_fragment_program);
	ext_shader_objects =		CHECK_EXTENSION(ARB_shader_objects);
	ext_shading_language_100 =	CHECK_EXTENSION(ARB_shading_language_100);
	ext_vertex_program =		CHECK_EXTENSION(ARB_vertex_program);
	ext_vertex_shader =			CHECK_EXTENSION(ARB_vertex_shader) || 
								CHECK_EXTENSION(EXT_vertex_shader);
	
	if(ext_shading_language_100) {
		glsl_version.assign( (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION_ARB) );
		sscanf(glsl_version.data(), "%d.%d", &glsl_major_version, &glsl_minor_version);
	}
	

	ext_geometry_shader4 =		CHECK_EXTENSION(EXT_geometry_shader4);
	ext_gpu_shader4 =			CHECK_EXTENSION(EXT_gpu_shader4);
	

	// Buffer objects
	ext_framebuffer_object =		CHECK_EXTENSION(EXT_framebuffer_object);
	ext_pixel_buffer_object =		CHECK_EXTENSION(ARB_pixel_buffer_object) || 
									CHECK_EXTENSION(EXT_pixel_buffer_object);
	ext_vertex_buffer_object =		CHECK_EXTENSION(ARB_vertex_buffer_object);
	ext_vertex_array =				CHECK_EXTENSION(EXT_vertex_array);
	lua_opengl_error("verifying extensions");

	// FBOs
	ext_draw_buffers =				CHECK_EXTENSION(ARB_draw_buffers);
	ext_framebuffer_blit =			CHECK_EXTENSION(EXT_framebuffer_blit);
	ext_framebuffer_multisample =	CHECK_EXTENSION(EXT_framebuffer_multisample);
	ext_framebuffer_sRGB =			CHECK_EXTENSION(EXT_framebuffer_sRGB);
	
	// VBOs
	ext_draw_range_elements =	CHECK_EXTENSION(EXT_draw_range_elements);
	ext_multi_draw_arrays =		CHECK_EXTENSION(EXT_multi_draw_arrays);
	
	// textures
	ext_depth_textures =			CHECK_EXTENSION(ARB_depth_texture);
	ext_multitexture =				CHECK_EXTENSION(ARB_multitexture);
	ext_shadow =					CHECK_EXTENSION(ARB_shadow);
	ext_texture_border_clamp =		CHECK_EXTENSION(ARB_texture_border_clamp);
	ext_texture_compression =		CHECK_EXTENSION(ARB_texture_compression);
	ext_texture_cube_map =			CHECK_EXTENSION(ARB_texture_cube_map) || 
									CHECK_EXTENSION(EXT_texture_cube_map);
	ext_texture_float =				CHECK_EXTENSION(ARB_texture_float);
	ext_texture_mirrored_repeat =	CHECK_EXTENSION(ARB_texture_mirrored_repeat);
	ext_texture_non_power_of_two =	CHECK_EXTENSION(ARB_texture_non_power_of_two);
	ext_texture_rectangle =			CHECK_EXTENSION(ARB_texture_rectangle) || 
									CHECK_EXTENSION(EXT_texture_rectangle);
	ext_subtexture =				CHECK_EXTENSION(EXT_subtexture);
	ext_texture =					CHECK_EXTENSION(EXT_texture);
	ext_texture3D =					CHECK_EXTENSION(EXT_texture3D);
	ext_texture_array =				CHECK_EXTENSION(EXT_texture_array);
	ext_texture_compression_dxt1 =	CHECK_EXTENSION(EXT_texture_compression_dxt1);
	ext_texture_compression_latc =	CHECK_EXTENSION(EXT_texture_compression_latc);
	ext_texture_compression_rgtc =	CHECK_EXTENSION(EXT_texture_compression_rgtc);
	ext_texture_compression_s3tc =	CHECK_EXTENSION(EXT_texture_compression_s3tc);
	ext_texture_edge_clamp =		CHECK_EXTENSION(EXT_texture_edge_clamp);
	ext_texture_integer =			CHECK_EXTENSION(EXT_texture_integer);
	ext_texture_mirror_clamp =		CHECK_EXTENSION(EXT_texture_mirror_clamp);
	ext_texture_sRGB =				CHECK_EXTENSION(EXT_texture_sRGB);
	lua_opengl_error("verifying extensions");
	
	// Pixels
	ext_multisample =			CHECK_EXTENSION(ARB_multisample) ||
								CHECK_EXTENSION(EXT_multisample);
	ext_color_buffer_float =	CHECK_EXTENSION(ARB_color_buffer_float);
	ext_half_float_pixel =		CHECK_EXTENSION(ARB_half_float_pixel);
	ext_422_pixels =			CHECK_EXTENSION(EXT_422_pixels);
	ext_abgr =					CHECK_EXTENSION(EXT_abgr);
	ext_bgra =					CHECK_EXTENSION(EXT_bgra);
	ext_packed_depth_stencil =	CHECK_EXTENSION(EXT_packed_depth_stencil);
	ext_packed_float =			CHECK_EXTENSION(EXT_packed_float);
	ext_packed_pixels =			CHECK_EXTENSION(EXT_packed_pixels);
	ext_half_float =			CHECK_EXTENSION(NV_half_float);
	lua_opengl_error("verifying extensions");

// Rendering
	// Point sprites
	ext_point_parameters =		CHECK_EXTENSION(ARB_point_parameters) ||
								CHECK_EXTENSION(EXT_point_parameters);
	ext_point_sprite =			CHECK_EXTENSION(ARB_point_sprite);
	lua_opengl_error("verifying extensions");
	// Blending
	ext_blend_color =				CHECK_EXTENSION(EXT_blend_color);
	ext_blend_equation_separate =	CHECK_EXTENSION(EXT_blend_equation_separate);
	ext_blend_func_separate =		CHECK_EXTENSION(EXT_blend_func_separate);
	ext_blend_logic_op =			CHECK_EXTENSION(EXT_blend_logic_op);
	ext_blend_minmax =				CHECK_EXTENSION(EXT_blend_minmax);
	ext_blend_subtract =			CHECK_EXTENSION(EXT_blend_subtract);
	lua_opengl_error("verifying extensions");

	// Fog
	ext_fog_coord =				CHECK_EXTENSION(EXT_fog_coord);

	// Apple
	ext_client_storage =		CHECK_EXTENSION(APPLE_client_storage);
	ext_ycbcr_422 =				CHECK_EXTENSION(APPLE_ycbcr_422);

	// Windows
	ext_swap_hint =				CHECK_EXTENSION(WIN_swap_hint);
	
	// Framebuffers
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_color_attachments);
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &max_renderbuffer_size);
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, max_viewport_dims);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_draw_buffers);
	glGetIntegerv(GL_SUBPIXEL_BITS, &subpixel_bits);
	lua_opengl_error("verifying extensions");
	
	// Points and Lines
	glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, aliased_line_width_range);
	glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, aliased_point_size_range);
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &smooth_line_width_granularity);
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, &smooth_line_width_range);
	glGetFloatv(GL_SMOOTH_POINT_SIZE_GRANULARITY, &smooth_point_size_range);
	glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, &smooth_line_width_range);
	lua_opengl_error("verifying extensions");
	
	// Buffers
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_elements_vertices);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_elements_indices);
	lua_opengl_error("verifying extensions");

	// Textures
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max_3d_texture_size);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max_cube_map_texture_size);
	glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &max_rectangle_texture_size);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &max_rectangle_texture_size);
	glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &max_texture_lod_bias);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_units);

	// Stacks
	glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &max_attrib_stack_depth);
	glGetIntegerv(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, &max_client_attrib_stack_depth);
	glGetIntegerv(GL_MAX_COLOR_MATRIX_STACK_DEPTH, &max_color_matrix_stack_depth);
	glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &max_modelview_stack_depth);
	glGetIntegerv(GL_MAX_NAME_STACK_DEPTH, &max_name_stack_depth);
	glGetIntegerv(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB, &max_programMatrix_stack_depth);
	glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &max_projection_stack_depth);
	glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &max_texture_stack_depth);
	lua_opengl_error("verifying extensions");
	
	// Vertex Programs
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB, &max_vertex_programAddress_registers);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ATTRIBS_ARB, &max_vertex_programAttribs);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &max_vertex_programEnv_parameters);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &max_vertex_programInstructions);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &max_vertex_programLocal_parameters);
//	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_MATRICES_ARB, &max_vertex_programMatrices);
//	gloGetGLError("verifying extensions");
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, &max_vertex_programNative_address_registers);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, &max_vertex_programNative_attribs);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &max_vertex_programNative_instructions);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, &max_vertex_programNative_parameters);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, &max_vertex_programNative_temporaries);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB, &max_vertex_programParameters);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB, &max_vertex_programTemporaries);
	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB, &max_vertex_parameters);
//	glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_VERTEX_ATTRIBS_ARB, &max_vertex_vertex_attribs);
//	gloGetGLError("verifying extensions");

	// Fragment Programs
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB, &max_fragment_programAlu_instructions);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ATTRIBS_ARB, &max_fragment_programAttribs);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &max_fragment_programEnv_parameters);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &max_fragment_programLocal_parameters);
//	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_MATRICES_ARB, &max_fragment_programMatrices);
//	gloGetGLError("verifying extensions");
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &max_fragment_programNative_alu_instructions);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, &max_fragment_programNative_attribs);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &max_fragment_programNative_instructions);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, &max_fragment_programNative_parameters);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, &max_fragment_programNative_temporaries);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &max_fragment_programNative_tex_indirections);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &max_fragment_programNative_tex_instructions);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB, &max_fragment_programParameters);
	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB, &max_fragment_programTemporaries);
//	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_TEXTURE_COORDS_ARB, &max_fragment_texture_coords);
//	gloGetGLError("verifying extensions");
//	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &max_fragment_texture_image_units);
//	gloGetGLError("verifying extensions");

	// Shaders
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &max_combined_texture_image_units);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB, &max_fragment_uniformComponents);
	glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &max_texture_coords);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &max_texture_image_units);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &max_vertrex_attribs);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &max_vertex_texture_image_units);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &max_vertex_uniformComponents);
	
	lua_opengl_error("verifying extensions");
}
