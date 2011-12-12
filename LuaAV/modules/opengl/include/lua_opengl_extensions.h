#ifndef LUA_OPENGL_EXTENSIONS
#define LUA_OPENGL_EXTENSIONS 1

#include "lua_opengl_config.h"
#include <string>

using std::string;


class Extensions {
public:
	Extensions();
	~Extensions();
	
	void vertify_extensions();

	bool haveShaders() const {return ext_fragment_shader && ext_vertex_shader;}
	bool haveVertexShader() const {return ext_vertex_shader;}
	bool haveFragmentShader() const {return ext_fragment_shader;}
	bool haveGeometryShader() const {return ext_geometry_shader4;}
	
	bool haveFBO() const {return ext_framebuffer_object;}
	bool havePBO() const {return ext_pixel_buffer_object;}
	bool haveVBO() const {return ext_vertex_buffer_object;}
	//bool haveVertexArray() const {return ext_vertex_array;} extension is wrong, use OpenGL version

	bool haveMRT() const {return ext_draw_buffers;}

//	bool haveSubtexture() const {return ext_subtexture;} extension is wrong, use OpenGL version
	bool have3DTexture() const {return max_3d_texture_size > 0;}
	bool haveFloatTexture() const {return ext_texture_float;}
//	bool haveHalfTexture() const {return ext_subtexture;}
	bool haveRectTexture() const {return ext_texture_rectangle;}
	
	int maxDrawBuffers() const {return max_draw_buffers;}
	int maxTextureSize() const {return max_texture_size;}
	int maxRectTextureSize() const {return max_rectangle_texture_size;}
	int max3DTextureSize() const {return max_3d_texture_size;}
	
	
	int maxElementsVertices() const  {return max_elements_vertices;}
	int maxElementsIndices() const  {return max_elements_indices;}

public:
	// OpenGL server info
	string	vendor;
	string	renderer;
	string	version;

	// OpenGL version
	int		major_version;
	int		minor_version;
	int		release_number;
	
	// GLSL info
	string	glsl_version;
	
	// GLSL version
	int		glsl_major_version;
	int		glsl_minor_version;
	
	// Shaders
	bool	ext_fragment_shader;			///! ARB_fragment_shader
	bool	ext_fragment_program;			///! ARB_fragment_program
	bool	ext_shader_objects;				///! ARB_shader_objects
	bool	ext_shading_language_100;		///! ARB_shading_language_100
	bool	ext_vertex_program;				///! ARB_vertex_program
	bool	ext_vertex_shader;				///! ARB_vertex_shader, EXT_vertex_shader
	bool	ext_geometry_shader4;			///! EXT_geometry_shader4
	bool	ext_gpu_shader4;				///! EXT_gpu_shader4
	
	
	
	// Buffer objects
	bool	ext_framebuffer_object;			///! EXT_framebuffer_object
	bool	ext_pixel_buffer_object;		///! ARB_pixel_buffer_object, EXT_pixel_buffer_object
	bool	ext_vertex_buffer_object;		///! ARB_vertex_buffer_object
	bool	ext_vertex_array;				///! EXT_vertex_array
	
	// FBOs
	bool	ext_draw_buffers;				///! ARB_draw_buffers
	bool	ext_framebuffer_blit;			///! EXT_framebuffer_blit
	bool	ext_framebuffer_multisample;	///! EXT_framebuffer_multisample
	bool	ext_framebuffer_sRGB;			///! EXT_framebuffer_sRGB
	
	// VBOs
	bool	ext_draw_range_elements;		///! EXT_draw_range_elements
	bool	ext_multi_draw_arrays;			///! EXT_multi_draw_arrays
	
	// textures
	bool	ext_depth_textures;				///! ARB_depth_texture
	bool	ext_multitexture;				///! ARB_multitexture
	bool	ext_shadow;						///! ARB_shadow
	bool	ext_texture_border_clamp;		///! ARB_texture_border_clamp
	bool	ext_texture_compression;		///! ARB_texture_compression
	bool	ext_texture_cube_map;			///! ARB_texture_cube_map, EXT_texture_cube_map
	bool	ext_texture_float;				///! ARB_texture_float
	bool	ext_texture_mirrored_repeat;	///! ARB_texture_mirrored_repeat
	bool	ext_texture_non_power_of_two;	///! ARB_texture_non_power_of_two
	bool	ext_texture_rectangle;			///! ARB_texture_rectangle, EXT_texture_rectangle
	bool	ext_subtexture;					///! EXT_subtexture
	bool	ext_texture;					///! EXT_texture
	bool	ext_texture3D;					///! EXT_texture3D
	bool	ext_texture_array;				///! EXT_texture_array
	bool	ext_texture_compression_dxt1;	///! EXT_texture_compression_dxt1
	bool	ext_texture_compression_latc;	///! EXT_texture_compression_latc
	bool	ext_texture_compression_rgtc;	///! EXT_texture_compression_rgtc
	bool	ext_texture_compression_s3tc;	///! EXT_texture_compression_s3t
	bool	ext_texture_edge_clamp;			///! EXT_texture_edge_clamp
	bool	ext_texture_integer;			///! EXT_texture_integer
	bool	ext_texture_mirror_clamp;		///! EXT_texture_mirror_clamp
	bool	ext_texture_sRGB;				///! EXT_texture_sRGB

	
	// Pixels
	bool	ext_multisample;				///! ARB_multisample, EXT_multisample
	bool	ext_color_buffer_float;			///! ARB_color_buffer_float
	bool	ext_half_float_pixel;			///! ARB_half_float_pixel
	bool	ext_422_pixels;					///! EXT_422_pixels
	bool	ext_abgr;						///! EXT_abgr
	bool	ext_bgra;						///! EXT_bgra
	bool	ext_packed_depth_stencil;		///! EXT_packed_depth_stencil
	bool	ext_packed_float;				///! EXT_packed_float
	bool	ext_packed_pixels;				///! EXT_packed_pixels
	bool	ext_half_float;					///! NV_half_float


// Rendering
	// Point sprites
	bool	ext_point_parameters;			///! ARB_point_parameters, EXT_point_parameters
	bool	ext_point_sprite;				///! ARB_point_sprite
	
	// Blending
	bool	ext_blend_color;				///! EXT_blend_color
	bool	ext_blend_equation_separate;	///! EXT_blend_equation_separate
	bool	ext_blend_func_separate;		///! EXT_blend_func_separate
	bool	ext_blend_logic_op;				///! EXT_blend_logic_op
	bool	ext_blend_minmax;				///! EXT_blend_minmax
	bool	ext_blend_subtract;				///! EXT_blend_subtract
	
	// Fog
	bool	ext_fog_coord;					///! EXT_fog_coord
	

	// Apple
	bool	ext_client_storage;				///! APPLE_client_storage
	bool	ext_ycbcr_422;					///! APPLE_ycbcr_422
	
	// Windows
	bool	ext_swap_hint;					///! WIN_swap_hint
	
///////////////////////////////////////////////////////////////////////////
// Limits
	// Framebuffers
	GLint	max_color_attachments;			///! MAX_COLOR_ATTACHMENTS_EXT
	GLint	max_renderbuffer_size;			///! MAX_RENDERBUFFER_SIZE_EXT
	GLint	max_viewport_dims[2];			///! MAX_VIEWPORT_DIMS
	GLint	max_draw_buffers;				///! MAX_DRAW_BUFFERS_ARB
	GLint	subpixel_bits;					///! SUBPIXEL_BITS
	
	// Points and Lines
	GLint	aliased_line_width_range[2];	///! ALIASED_LINE_WIDTH_RANGE
	GLint	aliased_point_size_range[2];	///! ALIASED_POINT_SIZE_RANGE
	float	smooth_line_width_granularity;	///! SMOOTH_LINE_WIDTH_GRANULARITY
	float	smooth_line_width_range;		///! SMOOTH_LINE_WIDTH_RANGE
	float	smooth_point_size_granularity;	///! SMOOTH_POINT_SIZE_GRANULARITY
	float	smooth_point_size_range;		///! SMOOTH_POINT_SIZE_RANGE
	
	// Buffers
	GLint		max_elements_vertices;		///! MAX_ELEMENTS_VERTICES
	GLint		max_elements_indices;		///! MAX_ELEMENTS_INDICES
	
	// Textures
	GLint	max_3d_texture_size;			///! MAX_3D_TEXTURE_SIZE
	GLint	max_cube_map_texture_size;		///! MAX_CUBE_MAP_TEXTURE_SIZE
	GLint	max_rectangle_texture_size;		///! MAX_RECTANGLE_TEXTURE_SIZE_EXT
	GLint	max_texture_size;				///! MAX_TEXTURE_SIZE
	float	max_texture_lod_bias;			///! MAX_TEXTURE_LOD_BIAS
	GLint	max_texture_max_anisotropy;		///! MAX_TEXTURE_MAX_ANISOTROPY_EXT
	GLint	max_texture_units;				///! MAX_TEXTURE_UNITS
	
	/// something for compression types (list of some form)
	
	// Stacks
	GLint	max_attrib_stack_depth;			///! MAX_ATTRIB_STACK_DEPTH
	GLint	max_client_attrib_stack_depth;	///! MAX_CLIENT_ATTRIB_STACK_DEPTH
	GLint	max_color_matrix_stack_depth;	///! MAX_COLOR_MATRIX_STACK_DEPTH
	GLint	max_modelview_stack_depth;		///! MAX_MODELVIEW_STACK_DEPTH
	GLint	max_name_stack_depth;			///! MAX_NAME_STACK_DEPTH
	GLint	max_programMatrix_stack_depth;	///! MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB
	GLint	max_projection_stack_depth;		///! MAX_PROJECTION_STACK_DEPTH
	GLint	max_texture_stack_depth;		///! MAX_TEXTURE_STACK_DEPTH
	
	// Vertex/Fragemnt Programs
	GLint	max_vertex_programAddress_registers;	///! MAX_PROGRAM_ADDRESS_REGISTERS_ARB
	GLint	max_vertex_programAttribs;				///! MAX_PROGRAM_ATTRIBS_ARB
	GLint	max_vertex_programEnv_parameters;		///! MAX_PROGRAM_ENV_PARAMETERS_ARB
	GLint	max_vertex_programInstructions;		///! MAX_PROGRAM_INSTRUCTIONS_ARB
	GLint	max_vertex_programLocal_parameters;	///! MAX_PROGRAM_LOCAL_PARAMETERS_ARB
	GLint	max_vertex_programMatrices;			///! MAX_PROGRAM_LOCAL_MATRICES_ARB
	GLint	max_vertex_programNative_address_registers;	///! MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB
	GLint	max_vertex_programNative_attribs;		///! MAX_PROGRAM_NATIVE_ATTRIBS_ARB
	GLint	max_vertex_programNative_instructions;	///! MAX_PROGRAM_NATIVE_INSTRUCTIONS
	GLint	max_vertex_programNative_parameters;	///! MAX_PROGRAM_NATIVE_PARAMETERS_ARB
	GLint	max_vertex_programNative_temporaries;	///! MAX_PROGRAM_NATIVE_TEMPORARIES_ARB
	GLint	max_vertex_programParameters;			///! MAX_PROGRAM_PARAMETERS_ARB
	GLint	max_vertex_programTemporaries;			///! MAX_PROGRAM_TEMPORARIES_ARB
	GLint	max_vertex_parameters;					///! MAX_PROGRAM_PARAMETERS_ARB
	GLint	max_vertex_vertex_attribs;				///! MAX_VERTEX_ATTRIBS_ARB
	
	GLint	max_fragment_programAlu_instructions;	///! MAX_PROGRAM_ALU_INSTRUCTIONS_ARB
	GLint	max_fragment_programAttribs;			///! MAX_PROGRAM_ATTRIBS_ARB
	GLint	max_fragment_programEnv_parameters;		///! MAX_PROGRAM_ENV_PARAMETERS_ARB
	GLint	max_fragment_programLocal_parameters;	///! MAX_PROGRAM_LOCAL_PARAMETERS_ARB
	GLint	max_fragment_programMatrices;			///! MAX_PROGRAM_LOCAL_MATRICES_ARB
	GLint	max_fragment_programNative_alu_instructions;	///! MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB
	GLint	max_fragment_programNative_attribs;		///! MAX_PROGRAM_NATIVE_ATTRIBS_ARB
	GLint	max_fragment_programNative_instructions;	///! MAX_PROGRAM_NATIVE_INSTRUCTIONS
	GLint	max_fragment_programNative_parameters;	///! MAX_PROGRAM_NATIVE_PARAMETERS_ARB
	GLint	max_fragment_programNative_temporaries;	///! MAX_PROGRAM_NATIVE_TEMPORARIES_ARB
	GLint	max_fragment_programNative_tex_indirections;	///! MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB
	GLint	max_fragment_programNative_tex_instructions;	///! MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB
	GLint	max_fragment_programParameters;			///! MAX_PROGRAM_PARAMETERS_ARB
	GLint	max_fragment_programTemporaries;		///! MAX_PROGRAM_TEMPORARIES_ARB
	GLint	max_fragment_texture_coords;			///! MAX_TEXTURE_COORDS_ARB
	GLint	max_fragment_texture_image_units;		///! MAX_TEXTURE_IMAGE_UNITS_ARB
	
	// Shaders
	GLint	max_combined_texture_image_units;	///! MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB
	GLint	max_fragment_uniformComponents;		///! MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB
	GLint	max_texture_coords;					///! MAX_TEXTURE_COORDS_ARB
	GLint	max_texture_image_units;			///! MAX_TEXTURE_IMAGE_UNITS_ARB
	GLint	max_vertrex_attribs;				///! MAX_VERTEX_ATTRIBS_ARB
	GLint	max_vertex_texture_image_units;		///! MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB
	GLint	max_vertex_uniformComponents;		///! MAX_VERTEX_UNIFORM_COMPONENTS_ARB
};

#endif // LUA_OPENGL_EXTENSIONS
