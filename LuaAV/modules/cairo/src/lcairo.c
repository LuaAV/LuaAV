/* -*- C -*-
 *
 * Name:        lcairo.c
 * Purpose:     Lua binding of Cairo
 * Author:      Hakki Dogusan (dogusanh@tr.net)
 * Created:     2007-05-25
 * Copyright:   (c) 2007-2008 Hakki Dogusan
 * License:     MIT (http://www.opensource.org/licenses/mit-license.html)
 * RCS-ID:      $Id$
 * History:     2008-04-12 updated to Cairo-1.6.4
 *              2008-11-15 updated to Cairo-1.8.4
 *              2008-11-25 oo interface added
 *
 */

#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <cairo.h>

#if CAIRO_HAS_SVG_SURFACE
    #include <cairo-svg.h>
#endif

#if CAIRO_HAS_PDF_SURFACE
    #include <cairo-pdf.h>
#endif

#if CAIRO_HAS_PS_SURFACE
    #include <cairo-ps.h>
#endif

#if CAIRO_HAS_XCB_SURFACE
    #include <cairo-xcb.h>
#endif

#if CAIRO_HAS_XLIB_SURFACE
    #include <cairo-xlib.h>
#endif

#if CAIRO_HAS_WIN32_SURFACE
    #include <cairo-win32.h>
#endif

#if CAIRO_HAS_BEOS_SURFACE
    #include <cairo-beos.h>
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
    #include <cairo-directfb.h>
#endif

#if CAIRO_HAS_OS2_SURFACE
    #include <cairo-os2.h>
#endif

#if CAIRO_HAS_GLITZ_SURFACE
    #include <cairo-glitz.h>
#endif

#if CAIRO_HAS_QUARTZ_SURFACE
    #include <cairo-quartz.h>
#endif

#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 6, 0)
#if CAIRO_HAS_ATSUI_FONT
    #include <cairo-atsui.h>
#endif
#endif

#if CAIRO_HAS_FT_FONT
    #include <cairo-ft.h>
#endif



#ifndef LUACAIRO_HAS_OO_INTERFACE
    #define LUACAIRO_HAS_OO_INTERFACE 1
#endif

#define LCAIRO "cairo"



///--------------------------------------------------------------------------
/// Function Declarations and Status
/// ++: ready
/// ww: ready, different usage
/// ..: partial
/// NI: not implemented
///--------------------------------------------------------------------------
static int   l_cairo_version (lua_State* L);                            // ++
static int   l_cairo_version_string (lua_State* L);                     // ++
static int   l_cairo_create (lua_State* L);                             // ++
static int   l_cairo_reference (lua_State* L);                          // ++
static int   l_cairo_destroy (lua_State* L);                            // ++
static int   l_cairo_get_reference_count (lua_State* L);                // ++
static int   l_cairo_get_user_data (lua_State* L);                      // NI
static int   l_cairo_set_user_data (lua_State* L);                      // NI
static int   l_cairo_save (lua_State* L);                               // ++
static int   l_cairo_restore (lua_State* L);                            // ++
static int   l_cairo_push_group (lua_State* L);                         // ++
static int   l_cairo_push_group_with_content (lua_State* L);            // ++
static int   l_cairo_pop_group (lua_State* L);                          // ++
static int   l_cairo_pop_group_to_source (lua_State* L);                // ++
static int   l_cairo_set_operator (lua_State* L);                       // ++
static int   l_cairo_set_source (lua_State* L);                         // ++
static int   l_cairo_set_source_rgb (lua_State* L);                     // ++
static int   l_cairo_set_source_rgba (lua_State* L);                    // ++
static int   l_cairo_set_source_surface (lua_State* L);                 // ++
static int   l_cairo_set_tolerance (lua_State* L);                      // ++
static int   l_cairo_set_antialias (lua_State* L);                      // ++
static int   l_cairo_set_fill_rule (lua_State* L);                      // ++
static int   l_cairo_set_line_width (lua_State* L);                     // ++
static int   l_cairo_set_line_cap (lua_State* L);                       // ++
static int   l_cairo_set_line_join (lua_State* L);                      // ++
static int   l_cairo_set_dash (lua_State* L);                           // ++
static int   l_cairo_set_miter_limit (lua_State* L);                    // ++
static int   l_cairo_translate (lua_State* L);                          // ++
static int   l_cairo_scale (lua_State* L);                              // ++
static int   l_cairo_rotate (lua_State* L);                             // ++
static int   l_cairo_transform (lua_State* L);                          // ++
static int   l_cairo_set_matrix (lua_State* L);                         // ++
static int   l_cairo_identity_matrix (lua_State* L);                    // ++
static int   l_cairo_user_to_device (lua_State* L);                     // ww
static int   l_cairo_user_to_device_distance (lua_State* L);            // ww
static int   l_cairo_device_to_user (lua_State* L);                     // ww
static int   l_cairo_device_to_user_distance (lua_State* L);            // ww
static int   l_cairo_new_path (lua_State* L);                           // ++
static int   l_cairo_move_to (lua_State* L);                            // ++
static int   l_cairo_new_sub_path (lua_State* L);                       // ++
static int   l_cairo_line_to (lua_State* L);                            // ++
static int   l_cairo_curve_to (lua_State* L);                           // ++
static int   l_cairo_arc (lua_State* L);                                // ++
static int   l_cairo_arc_negative (lua_State* L);                       // ++
static int   l_cairo_rel_move_to (lua_State* L);                        // ++
static int   l_cairo_rel_line_to (lua_State* L);                        // ++
static int   l_cairo_rel_curve_to (lua_State* L);                       // ++
static int   l_cairo_rectangle (lua_State* L);                          // ++
static int   l_cairo_close_path (lua_State* L);                         // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_path_extents (lua_State* L);                       // ww
#endif
static int   l_cairo_paint (lua_State* L);                              // ++
static int   l_cairo_paint_with_alpha (lua_State* L);                   // ++
static int   l_cairo_mask (lua_State* L);                               // ++
static int   l_cairo_mask_surface (lua_State* L);                       // ++
static int   l_cairo_stroke (lua_State* L);                             // ++
static int   l_cairo_stroke_preserve (lua_State* L);                    // ++
static int   l_cairo_fill (lua_State* L);                               // ++
static int   l_cairo_fill_preserve (lua_State* L);                      // ++
static int   l_cairo_copy_page (lua_State* L);                          // ++
static int   l_cairo_show_page (lua_State* L);                          // ++
static int   l_cairo_in_stroke (lua_State* L);                          // ++
static int   l_cairo_in_fill (lua_State* L);                            // ++
static int   l_cairo_stroke_extents (lua_State* L);                     // ww
static int   l_cairo_fill_extents (lua_State* L);                       // ww
static int   l_cairo_reset_clip (lua_State* L);                         // ++
static int   l_cairo_clip (lua_State* L);                               // ++
static int   l_cairo_clip_preserve (lua_State* L);                      // ++
static int   l_cairo_clip_extents (lua_State* L);                       // ww
static int   l_cairo_copy_clip_rectangle_list (lua_State* L);           // ++
static int   l_cairo_rectangle_list_destroy (lua_State* L);             // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_glyph_allocate (lua_State* L);                     // NI
static int   l_cairo_glyph_free (lua_State* L);                         // NI
static int   l_cairo_text_cluster_allocate (lua_State* L);              // NI
static int   l_cairo_text_cluster_free (lua_State* L);                  // NI
#endif
static int   l_cairo_font_options_create (lua_State* L);                // ++
static int   l_cairo_font_options_copy (lua_State* L);                  // ++
static int   l_cairo_font_options_destroy (lua_State* L);               // ++
static int   l_cairo_font_options_status (lua_State* L);                // ++
static int   l_cairo_font_options_merge (lua_State* L);                 // ++
static int   l_cairo_font_options_equal (lua_State* L);                 // ++
static int   l_cairo_font_options_hash (lua_State* L);                  // ++
static int   l_cairo_font_options_set_antialias (lua_State* L);         // ++
static int   l_cairo_font_options_get_antialias (lua_State* L);         // ++
static int   l_cairo_font_options_set_subpixel_order (lua_State* L);    // ++
static int   l_cairo_font_options_get_subpixel_order (lua_State* L);    // ++
static int   l_cairo_font_options_set_hint_style (lua_State* L);        // ++
static int   l_cairo_font_options_get_hint_style (lua_State* L);        // ++
static int   l_cairo_font_options_set_hint_metrics (lua_State* L);      // ++
static int   l_cairo_font_options_get_hint_metrics (lua_State* L);      // ++
static int   l_cairo_select_font_face (lua_State* L);                   // ++
static int   l_cairo_set_font_size (lua_State* L);                      // ++
static int   l_cairo_set_font_matrix (lua_State* L);                    // ++
static int   l_cairo_get_font_matrix (lua_State* L);                    // ++
static int   l_cairo_set_font_options (lua_State* L);                   // ++
static int   l_cairo_get_font_options (lua_State* L);                   // ++
static int   l_cairo_set_font_face (lua_State* L);                      // ++
static int   l_cairo_get_font_face (lua_State* L);                      // ++
static int   l_cairo_set_scaled_font (lua_State* L);                    // ++
static int   l_cairo_get_scaled_font (lua_State* L);                    // ++
static int   l_cairo_show_text (lua_State* L);                          // ++
static int   l_cairo_show_glyphs (lua_State* L);                        // NI
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_show_text_glyphs (lua_State* L);                   // NI
#endif
static int   l_cairo_text_path (lua_State* L);                          // ++
static int   l_cairo_glyph_path (lua_State* L);                         // NI
static int   l_cairo_text_extents (lua_State* L);                       // ++
static int   l_cairo_glyph_extents (lua_State* L);                      // NI
static int   l_cairo_font_extents (lua_State* L);                       // ++
static int   l_cairo_font_face_reference (lua_State* L);                // ++
static int   l_cairo_font_face_destroy (lua_State* L);                  // ++
static int   l_cairo_font_face_get_reference_count (lua_State* L);      // ++
static int   l_cairo_font_face_status (lua_State* L);                   // ++
static int   l_cairo_font_face_get_type (lua_State* L);                 // ++
static int   l_cairo_font_face_get_user_data (lua_State* L);            // NI
static int   l_cairo_font_face_set_user_data (lua_State* L);            // NI
static int   l_cairo_scaled_font_create (lua_State* L);                 // ++
static int   l_cairo_scaled_font_reference (lua_State* L);              // ++
static int   l_cairo_scaled_font_destroy (lua_State* L);                // ++
static int   l_cairo_scaled_font_get_reference_count (lua_State* L);    // ++
static int   l_cairo_scaled_font_status (lua_State* L);                 // ++
static int   l_cairo_scaled_font_get_type (lua_State* L);               // ++
static int   l_cairo_scaled_font_get_user_data (lua_State* L);          // NI
static int   l_cairo_scaled_font_set_user_data (lua_State* L);          // NI
static int   l_cairo_scaled_font_extents (lua_State* L);                // ++
static int   l_cairo_scaled_font_text_extents (lua_State* L);           // ++
static int   l_cairo_scaled_font_glyph_extents (lua_State* L);          // NI
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_scaled_font_text_to_glyphs (lua_State* L);         // NI
#endif
static int   l_cairo_scaled_font_get_font_face (lua_State* L);          // ++
static int   l_cairo_scaled_font_get_font_matrix (lua_State* L);        // ++
static int   l_cairo_scaled_font_get_ctm (lua_State* L);                // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_scaled_font_get_scale_matrix (lua_State* L);       // NI
#endif
static int   l_cairo_scaled_font_get_font_options (lua_State* L);       // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_toy_font_face_create (lua_State* L);               // ++
static int   l_cairo_toy_font_face_get_family (lua_State* L);           // ++
static int   l_cairo_toy_font_face_get_slant (lua_State* L);            // ++
static int   l_cairo_toy_font_face_get_weight (lua_State* L);           // ++
static int   l_cairo_user_font_face_create (lua_State* L);              // ++
static int   l_cairo_user_font_face_set_init_func (lua_State* L);       // NI
static int   l_cairo_user_font_face_set_render_glyph_func (lua_State* L);//NI
static int   l_cairo_user_font_face_set_text_to_glyphs_func (lua_State* L);//NI
static int   l_cairo_user_font_face_set_unicode_to_glyph_func (lua_State* L);//NI
static int   l_cairo_user_font_face_get_init_func (lua_State* L);       // NI
static int   l_cairo_user_font_face_get_render_glyph_func (lua_State* L);//NI
static int   l_cairo_user_font_face_get_text_to_glyphs_func (lua_State* L);//NI
static int   l_cairo_user_font_face_get_unicode_to_glyph_func (lua_State* L);//NI
#endif
static int   l_cairo_get_operator (lua_State* L);                       // ++
static int   l_cairo_get_source (lua_State* L);                         // ++
static int   l_cairo_get_tolerance (lua_State* L);                      // ++
static int   l_cairo_get_antialias (lua_State* L);                      // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_has_current_point (lua_State* L);                  // ++
#endif
static int   l_cairo_get_current_point (lua_State* L);                  // ww
static int   l_cairo_get_fill_rule (lua_State* L);                      // ++
static int   l_cairo_get_line_width (lua_State* L);                     // ++
static int   l_cairo_get_line_cap (lua_State* L);                       // ++
static int   l_cairo_get_line_join (lua_State* L);                      // ++
static int   l_cairo_get_miter_limit (lua_State* L);                    // ++
static int   l_cairo_get_dash_count (lua_State* L);                     // ++
static int   l_cairo_get_dash (lua_State* L);                           // NI
static int   l_cairo_get_matrix (lua_State* L);                         // ++
static int   l_cairo_get_target (lua_State* L);                         // ++
static int   l_cairo_get_group_target (lua_State* L);                   // ++
static int   l_cairo_copy_path (lua_State* L);                          // ++
static int   l_cairo_copy_path_flat (lua_State* L);                     // ++
static int   l_cairo_append_path (lua_State* L);                        // ++
static int   l_cairo_path_destroy (lua_State* L);                       // ++
static int   l_cairo_status (lua_State* L);                             // ++
static int   l_cairo_status_to_string (lua_State* L);                   // ++
static int   l_cairo_surface_create_similar (lua_State* L);             // ++
static int   l_cairo_surface_reference (lua_State* L);                  // ++
static int   l_cairo_surface_finish (lua_State* L);                     // ++
static int   l_cairo_surface_destroy (lua_State* L);                    // ++
static int   l_cairo_surface_get_reference_count (lua_State* L);        // ++
static int   l_cairo_surface_status (lua_State* L);                     // ++
static int   l_cairo_surface_get_type (lua_State* L);                   // ++
static int   l_cairo_surface_get_content (lua_State* L);                // ++
#if CAIRO_HAS_PNG_FUNCTIONS
static int   l_cairo_surface_write_to_png (lua_State* L);               // ++
static int   l_cairo_surface_write_to_png_stream (lua_State* L);        // NI
#endif /* CAIRO_HAS_PNG_FUNCTIONS */
static int   l_cairo_surface_get_user_data (lua_State* L);              // NI
static int   l_cairo_surface_set_user_data (lua_State* L);              // NI
static int   l_cairo_surface_get_font_options (lua_State* L);           // ++
static int   l_cairo_surface_flush (lua_State* L);                      // ++
static int   l_cairo_surface_mark_dirty (lua_State* L);                 // ++
static int   l_cairo_surface_mark_dirty_rectangle (lua_State* L);       // ++
static int   l_cairo_surface_set_device_offset (lua_State* L);          // ++
static int   l_cairo_surface_get_device_offset (lua_State* L);          // ww
static int   l_cairo_surface_set_fallback_resolution (lua_State* L);    // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_surface_get_fallback_resolution (lua_State* L);    // ++
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_surface_copy_page (lua_State* L);                  // ++
static int   l_cairo_surface_show_page (lua_State* L);                  // ++
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
static int   l_cairo_surface_has_show_text_glyphs (lua_State* L);       // ++
#endif
static int   l_cairo_image_surface_create (lua_State* L);               // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_format_stride_for_width (lua_State* L);            // ++
#endif
static int   l_cairo_image_surface_create_for_data (lua_State* L);      // NI
static int   l_cairo_image_surface_get_data (lua_State* L);             // ++
static int   l_cairo_image_surface_get_format (lua_State* L);           // ++
static int   l_cairo_image_surface_get_width (lua_State* L);            // ++
static int   l_cairo_image_surface_get_height (lua_State* L);           // ++
static int   l_cairo_image_surface_get_stride (lua_State* L);           // ++
#if CAIRO_HAS_PNG_FUNCTIONS
static int   l_cairo_image_surface_create_from_png (lua_State* L);      // ++
static int   l_cairo_image_surface_create_from_png_stream (lua_State* L);//NI
#endif /* CAIRO_HAS_PNG_FUNCTIONS */
static int   l_cairo_pattern_create_rgb (lua_State* L);                 // ++
static int   l_cairo_pattern_create_rgba (lua_State* L);                // ++
static int   l_cairo_pattern_create_for_surface (lua_State* L);         // ++
static int   l_cairo_pattern_create_linear (lua_State* L);              // ++
static int   l_cairo_pattern_create_radial (lua_State* L);              // ++
static int   l_cairo_pattern_reference (lua_State* L);                  // ++
static int   l_cairo_pattern_destroy (lua_State* L);                    // ++
static int   l_cairo_pattern_get_reference_count (lua_State* L);        // ++
static int   l_cairo_pattern_status (lua_State* L);                     // ++
static int   l_cairo_pattern_get_user_data (lua_State* L);              // NI
static int   l_cairo_pattern_set_user_data (lua_State* L);              // NI
static int   l_cairo_pattern_get_type (lua_State* L);                   // ++
static int   l_cairo_pattern_add_color_stop_rgb (lua_State* L);         // ++
static int   l_cairo_pattern_add_color_stop_rgba (lua_State* L);        // ++
static int   l_cairo_pattern_set_matrix (lua_State* L);                 // ++
static int   l_cairo_pattern_get_matrix (lua_State* L);                 // ++
static int   l_cairo_pattern_set_extend (lua_State* L);                 // ++
static int   l_cairo_pattern_get_extend (lua_State* L);                 // ++
static int   l_cairo_pattern_set_filter (lua_State* L);                 // ++
static int   l_cairo_pattern_get_filter (lua_State* L);                 // ++
static int   l_cairo_pattern_get_rgba (lua_State* L);                   // ww
static int   l_cairo_pattern_get_surface (lua_State* L);                // ww
static int   l_cairo_pattern_get_color_stop_rgba (lua_State* L);        // ww
static int   l_cairo_pattern_get_color_stop_count (lua_State* L);       // ww
static int   l_cairo_pattern_get_linear_points (lua_State* L);          // ww
static int   l_cairo_pattern_get_radial_circles (lua_State* L);         // ww
static int   l_cairo_matrix_init (lua_State* L);                        // ++
static int   l_cairo_matrix_init_identity (lua_State* L);               // ++
static int   l_cairo_matrix_init_translate (lua_State* L);              // ++
static int   l_cairo_matrix_init_scale (lua_State* L);                  // ++
static int   l_cairo_matrix_init_rotate (lua_State* L);                 // ++
static int   l_cairo_matrix_translate (lua_State* L);                   // ++
static int   l_cairo_matrix_scale (lua_State* L);                       // ++
static int   l_cairo_matrix_rotate (lua_State* L);                      // ++
static int   l_cairo_matrix_invert (lua_State* L);                      // ++
static int   l_cairo_matrix_multiply (lua_State* L);                    // ++
static int   l_cairo_matrix_transform_distance (lua_State* L);          // ww
static int   l_cairo_matrix_transform_point (lua_State* L);             // ww
static int   l_cairo_debug_reset_static_data (lua_State* L);            // ++
#if CAIRO_HAS_SVG_SURFACE
static int   l_cairo_svg_surface_create (lua_State* L);                 // ++
static int   l_cairo_svg_surface_create_for_stream (lua_State* L);      // NI
static int   l_cairo_svg_surface_restrict_to_version (lua_State* L);    // ++
static int   l_cairo_svg_get_versions (lua_State* L);                   // NI
static int   l_cairo_svg_version_to_string (lua_State* L);              // ++
#endif /* CAIRO_HAS_SVG_SURFACE */
#if CAIRO_HAS_PDF_SURFACE
static int   l_cairo_pdf_surface_create (lua_State* L);                 // ++
static int   l_cairo_pdf_surface_create_for_stream (lua_State* L);      // NI
static int   l_cairo_pdf_surface_set_size (lua_State* L);               // ++
#endif /* CAIRO_HAS_PDF_SURFACE */
#if CAIRO_HAS_PS_SURFACE
static int   l_cairo_ps_surface_create (lua_State* L);                  // ++
static int   l_cairo_ps_surface_create_for_stream (lua_State* L);       // NI
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_ps_surface_restrict_to_level (lua_State* L);       // ++
static int   l_cairo_ps_get_levels (lua_State* L);                      // NI
static int   l_cairo_ps_level_to_string (lua_State* L);                 // ++
static int   l_cairo_ps_surface_set_eps (lua_State* L);                 // ++
static int   l_cairo_ps_surface_get_eps (lua_State* L);                 // ++
#endif
static int   l_cairo_ps_surface_set_size (lua_State* L);                // ++
static int   l_cairo_ps_surface_dsc_comment (lua_State* L);             // ++
static int   l_cairo_ps_surface_dsc_begin_setup (lua_State* L);         // ++
static int   l_cairo_ps_surface_dsc_begin_page_setup (lua_State* L);    // ++
#endif /* CAIRO_HAS_PS_SURFACE */
#if CAIRO_HAS_XCB_SURFACE
static int   l_cairo_xcb_surface_create (lua_State* L);                 // NI
static int   l_cairo_xcb_surface_create_for_bitmap (lua_State* L);      // NI
static int   l_cairo_xcb_surface_set_size (lua_State* L);               // ++
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
static int   l_cairo_xcb_surface_create_with_xrender_format (lua_State* L);//NI
#endif
#endif /* CAIRO_HAS_XCB_SURFACE */
#if CAIRO_HAS_XLIB_SURFACE
static int   l_cairo_xlib_surface_create (lua_State* L);                // NI
static int   l_cairo_xlib_surface_create_for_bitmap (lua_State* L);     // NI
static int   l_cairo_xlib_surface_set_size (lua_State* L);              // ++
static int   l_cairo_xlib_surface_set_drawable (lua_State* L);          // NI
static int   l_cairo_xlib_surface_get_display (lua_State* L);           // ++
static int   l_cairo_xlib_surface_get_drawable (lua_State* L);          // NI
static int   l_cairo_xlib_surface_get_screen (lua_State* L);            // ++
static int   l_cairo_xlib_surface_get_visual (lua_State* L);            // ++
static int   l_cairo_xlib_surface_get_depth (lua_State* L);             // ++
static int   l_cairo_xlib_surface_get_width (lua_State* L);             // ++
static int   l_cairo_xlib_surface_get_height (lua_State* L);            // ++
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
static int   l_cairo_xlib_surface_create_with_xrender_format (lua_State* L);//NI
#endif
#endif /* CAIRO_HAS_XLIB_SURFACE */
#if CAIRO_HAS_WIN32_SURFACE
static int   l_cairo_win32_surface_create (lua_State* L);               // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_win32_printing_surface_create (lua_State* L);      // ++
#endif
static int   l_cairo_win32_surface_create_with_ddb (lua_State* L);      // ++
static int   l_cairo_win32_surface_create_with_dib (lua_State* L);      // ++
static int   l_cairo_win32_surface_get_dc (lua_State* L);               // ++
static int   l_cairo_win32_surface_get_image (lua_State* L);            // ++
#if CAIRO_HAS_WIN32_FONT
static int   l_cairo_win32_font_face_create_for_logfontw (lua_State* L);// ++
static int   l_cairo_win32_font_face_create_for_hfont (lua_State* L);   // ++
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_win32_font_face_create_for_logfontw_hfont (lua_State* L);//++
#endif
static int   l_cairo_win32_scaled_font_select_font (lua_State* L);      // ++
static int   l_cairo_win32_scaled_font_done_font (lua_State* L);        // ++
static int   l_cairo_win32_scaled_font_get_metrics_factor (lua_State* L);//++
static int   l_cairo_win32_scaled_font_get_logical_to_device (lua_State* L);//++
static int   l_cairo_win32_scaled_font_get_device_to_logical (lua_State* L);//++
#endif /* CAIRO_HAS_WIN32_FONT */
#endif /* CAIRO_HAS_WIN32_SURFACE */
#if CAIRO_HAS_BEOS_SURFACE
static int   l_cairo_beos_surface_create (lua_State* L);                // ++
static int   l_cairo_beos_surface_create_for_bitmap (lua_State* L);     // ++
#endif /* CAIRO_HAS_BEOS_SURFACE */
#if CAIRO_HAS_DIRECTFB_SURFACE
static int   l_cairo_directfb_surface_create (lua_State* L);            // ++
#endif /*CAIRO_HAS_DIRECTFB_SURFACE*/
#if CAIRO_HAS_OS2_SURFACE
static int   l_cairo_os2_init (lua_State* L);                           // ++
static int   l_cairo_os2_fini (lua_State* L);                           // ++
static int   l_cairo_os2_surface_create (lua_State* L);                 // NI
static int   l_cairo_os2_surface_set_hwnd (lua_State* L);               // NI
static int   l_cairo_os2_surface_set_size (lua_State* L);               // ++
static int   l_cairo_os2_surface_refresh_window (lua_State* L);         // NI
static int   l_cairo_os2_surface_set_manual_window_refresh (lua_State* L);//++
static int   l_cairo_os2_surface_get_manual_window_refresh (lua_State* L);//++
#endif /* CAIRO_HAS_OS2_SURFACE */
#if CAIRO_HAS_GLITZ_SURFACE
static int   l_cairo_glitz_surface_create (lua_State* L);               // ++
#endif /* CAIRO_HAS_GLITZ_SURFACE */
#if CAIRO_HAS_QUARTZ_SURFACE
static int   l_cairo_quartz_surface_create (lua_State* L);              // ++
static int   l_cairo_quartz_surface_create_for_cg_context (lua_State* L);//NI
static int   l_cairo_quartz_surface_get_cg_context (lua_State* L);      // NI
#ifdef CAIRO_HAS_QUARTZ_FONT
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_quartz_font_face_create_for_cgfont (lua_State* L); // NI
static int   l_cairo_quartz_font_face_create_for_atsu_font_id (lua_State* L);//NI
#endif
#endif /* CAIRO_HAS_QUARTZ_FONT */
#endif /* CAIRO_HAS_QUARTZ_SURFACE */
#if CAIRO_HAS_ATSUI_FONT
#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 6, 0)
static int   l_cairo_atsui_font_face_create_for_atsu_font_id (lua_State* L);//NI
#endif
#endif /* CAIRO_HAS_ATSUI_FONT */
#if CAIRO_HAS_FT_FONT
static int   l_cairo_ft_font_face_create_for_pattern (lua_State* L);    // NI
static int   l_cairo_ft_font_options_substitute (lua_State* L);         // NI
static int   l_cairo_ft_font_face_create_for_ft_face (lua_State* L);    // NI
static int   l_cairo_ft_scaled_font_lock_face (lua_State* L);           // NI
static int   l_cairo_ft_scaled_font_unlock_face (lua_State* L);         // NI
#endif /* CAIRO_HAS_FT_FONT */



///--------------------------------------------------------------------------
/// utilities
///--------------------------------------------------------------------------
static void * check_lightuserdata (lua_State *L, int idx);
static void * get_userdata (lua_State *L, int ud, const char *tname);
static double get_numfield (lua_State* L, const char* key);
static void   set_strfield (lua_State* L, const char* key, const char* value);
static void   set_intfield (lua_State* L, const char* key, int value);

static cairo_t*              get_cairo_t (lua_State *L, int idx);
static cairo_surface_t*      get_cairo_surface_t (lua_State *L, int idx);
static cairo_pattern_t*      get_cairo_pattern_t (lua_State *L, int idx);
static cairo_font_options_t* get_cairo_font_options_t (lua_State *L, int idx);
static cairo_matrix_t*       get_cairo_matrix_t (lua_State *L, int idx);
static cairo_text_extents_t* get_cairo_text_extents_t (lua_State *L, int idx);
static cairo_font_extents_t* get_cairo_font_extents_t (lua_State *L, int idx);
static cairo_scaled_font_t*  get_cairo_scaled_font_t (lua_State *L, int idx);
static cairo_font_face_t*    get_cairo_font_face_t (lua_State *L, int idx);
static cairo_path_t*         get_cairo_path_t (lua_State *L, int idx);



///--------------------------------------------------------------------------
/// Helper functions - not part of Cairo API
///--------------------------------------------------------------------------

#if LUACAIRO_HAS_OO_INTERFACE
// oo creators for lightuserdata
// syntax: o = cairo.xxxFromLUD(lud, transfer_ownership (true/false))
static int new_ContextFromLUD (lua_State *L);
static int new_SurfaceFromLUD (lua_State *L);
static int new_PatternFromLUD (lua_State *L);
static int new_FontOptionsFromLUD (lua_State *L);
static int new_ScaledFontFromLUD (lua_State *L);
static int new_FontFaceFromLUD (lua_State *L);
#endif


#if CAIRO_HAS_XLIB_SURFACE
    // #include <X11/Xlib.h>
    // #include <gtk/gtk.h>
//    #include <gdk/gdk.h>
#endif

static int l_CreateContext(lua_State* L)
{
#if CAIRO_HAS_WIN32_SURFACE
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_t *cr = cairo_create(cairo_win32_surface_create( hdc ));
    lua_pushlightuserdata(L, cr);
    return 1;
#endif

#ifdef GDK_WINDOWING_X11
    GdkDrawable *drawable = (GdkDrawable *) check_lightuserdata(L, 1);
    cairo_t *cr = gdk_cairo_create( drawable ) ;
    lua_pushlightuserdata(L, cr);
    return 1;
#endif

    luaL_error(L, "not implemented!");
    return 0;
}

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
static int l_CreatePrintingContext(lua_State* L)
{
#if CAIRO_HAS_WIN32_SURFACE
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_t *cr = cairo_create(cairo_win32_printing_surface_create( hdc ));
    lua_pushlightuserdata(L, cr);
    return 1;
#endif

#ifdef GDK_WINDOWING_X11
    luaL_error(L, "not implemented!");
#endif

    luaL_error(L, "not implemented!");
    return 0;
}
#endif



///--------------------------------------------------------------------------
/// utilities
///--------------------------------------------------------------------------

static void * check_lightuserdata(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TLIGHTUSERDATA);
    return lua_touserdata(L, idx);
}

//hd.. Borrowed from lauxlib.c
//hd.. LUALIB_API void *luaL_checkudata (lua_State *L, int ud, const char *tname) {
static void* get_userdata (lua_State *L, int ud, const char *tname) {
  void *p = lua_touserdata(L, ud);
  if (p != NULL) {                                /* value is a userdata? */
    if (lua_getmetatable(L, ud)) {                /* does it have a metatable? */
      lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get correct metatable */
      if (lua_rawequal(L, -1, -2)) {              /* does it have the correct mt? */
        lua_pop(L, 2);                            /* remove both metatables */
        return p;
      }
    }
  }
  //hd.. luaL_typerror(L, ud, tname);  /* else error */
  //hd.. return NULL;  /* to avoid warnings */
  return NULL;
}


/* PIL2:page:232,233 assume that table is at the stack top */

//unused
//static const char* get_strfield(lua_State* L, const char* key)
//{
//    lua_getfield(L, -1, key);
//    const char* value = luaL_optstring(L, -1, "");
//    lua_pop(L, 1); /* remove value */
//    return value;
//}

static double get_numfield(lua_State* L, const char* key)
{
    lua_getfield(L, -1, key);
    double value = luaL_optnumber(L, -1, 0.0);
    lua_pop(L, 1); /* remove value */
    return value;
}

//unused
//static int get_intfield(lua_State* L, const char* key)
//{
//    lua_getfield(L, -1, key);
//    int value = luaL_optinteger(L, -1, 0);
//    lua_pop(L, 1); /* remove value */
//    return value;
//}

static void set_strfield(lua_State* L, const char* key, const char* value)
{
    lua_pushstring(L, value);
    lua_setfield(L, -2, key);
}

//unused
//static void set_numfield(lua_State* L, const char* key, double value)
//{
//    lua_pushnumber(L, value);
//    lua_setfield(L, -2, key);
//}

static void set_intfield(lua_State* L, const char* key, int value)
{
    lua_pushinteger(L, value);
    lua_setfield(L, -2, key);
}




///--------------------------------------------------------------------------
/// Utilities for struct binding
/// Used for cairo_matrix_t, cairo_text_extents_t, cairo_font_extents_t
/// Adapted from: http://lua-users.org/wiki/BindingWithMembersAndMethods
///--------------------------------------------------------------------------

//unused
//static int Xet_get_int (lua_State *L, void *v)
//{
//    lua_pushnumber(L, *(int*)v);
//    return 1;
//}

//unused
//static int Xet_set_int (lua_State *L, void *v)
//{
//    *(int*)v = luaL_checkint(L, 3);
//    return 0;
//}

static int Xet_get_number (lua_State *L, void *v)
{
    lua_pushnumber(L, *(lua_Number*)v);
    return 1;
}

static int Xet_set_number (lua_State *L, void *v)
{
    *(lua_Number*)v = luaL_checknumber(L, 3);
    return 0;
}

//unused
//static int Xet_get_string (lua_State *L, void *v)
//{
//    lua_pushstring(L, (char*)v );
//    return 1;
//}

typedef int (*Xet_func) (lua_State *L, void *v);

/* member info for get and set handlers */
typedef const struct {
    const char *name;       /* member name */
    Xet_func    func;       /* get or set function for type of member */
    size_t      offset;     /* offset of member within your_t */
}  Xet_reg_pre;

typedef Xet_reg_pre * Xet_reg;

static void Xet_add (lua_State *L, Xet_reg l)
{
    for (; l->name; l++) {
        lua_pushstring(L, l->name);
        lua_pushlightuserdata(L, (void*)l);
        lua_settable(L, -3);
    }
}

static int Xet_call (lua_State *L)
{
    /* for get: stack has userdata, index, lightuserdata */
    /* for set: stack has userdata, index, value, lightuserdata */
    Xet_reg m = (Xet_reg)lua_touserdata(L, -1);  /* member info */
    lua_pop(L, 1);                               /* drop lightuserdata */
    luaL_checktype(L, 1, LUA_TUSERDATA);
    return m->func(L, (void *)((char *)lua_touserdata(L, 1) + m->offset));
}

static int Xet_index_handler (lua_State *L)
{
    /* stack has userdata, index */
    lua_pushvalue(L, 2);                       /* dup index */
    lua_rawget(L, lua_upvalueindex(1));        /* lookup member by name */
    if (!lua_islightuserdata(L, -1)) {
        lua_pop(L, 1);                         /* drop value */
        lua_pushvalue(L, 2);                   /* dup index */
        lua_gettable(L, lua_upvalueindex(2));  /* else try methods */
        if (lua_isnil(L, -1))                  /* invalid member */
            luaL_error(L, "cannot get member '%s'", lua_tostring(L, 2));
        return 1;
    }
    return Xet_call(L);                        /* call get function */
}

static int Xet_newindex_handler (lua_State *L)
{
    /* stack has userdata, index, value */
    lua_pushvalue(L, 2);                       /* dup index */
    lua_rawget(L, lua_upvalueindex(1));        /* lookup member by name */
    if (!lua_islightuserdata(L, -1))           /* invalid member */
        luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));
    return Xet_call(L);                        /* call set function */
}



///--------------------------------------------------------------------------
/// Matrix userdata for cairo_matrix_t
///--------------------------------------------------------------------------

#define check_Matrix_ud(L, idx) \
    (cairo_matrix_t *) luaL_checkudata(L, idx, LCAIRO ".Matrix.mt")

static cairo_matrix_t check_Matrix(lua_State* L, int idx)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    lua_pushvalue(L, idx); // copy table to top

    cairo_matrix_t matrix;
    matrix.xx = get_numfield(L, "xx");
    matrix.yx = get_numfield(L, "yx");
    matrix.xy = get_numfield(L, "xy");
    matrix.yy = get_numfield(L, "yy");
    matrix.x0 = get_numfield(L, "x0");
    matrix.y0 = get_numfield(L, "y0");

    lua_pop(L, 1); // remove copied table
    return matrix;
}

static int new_Matrix (lua_State *L)
{
    cairo_matrix_t mtin = {0, 0, 0, 0, 0, 0};
    int top = lua_gettop(L);
    if (top > 1) mtin = check_Matrix(L, 2);

    cairo_matrix_t *m = (cairo_matrix_t *) lua_newuserdata(L, sizeof(cairo_matrix_t));
    *m = mtin;

    luaL_getmetatable(L, LCAIRO ".Matrix.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int tostring_Matrix (lua_State *L)
{
    cairo_matrix_t *m = check_Matrix_ud(L, 1);
    lua_pushfstring(L, "xx:%f, yx:%f, xy:%f, yy:%f, x0:%f, y0:%f",
        m->xx, m->yx, m->xy, m->yy, m->x0, m->y0);
    return 1;
}

static const struct luaL_Reg Matrix_meta_methods[] = {
    {"__tostring",                               tostring_Matrix},
    {NULL, NULL}
};

static const struct luaL_Reg Matrix_methods[] = {
    {"new",                                      new_Matrix},
    {"init",                                     l_cairo_matrix_init},
    {"init_identity",                            l_cairo_matrix_init_identity},
    {"init_translate",                           l_cairo_matrix_init_translate},
    {"init_scale",                               l_cairo_matrix_init_scale},
    {"init_rotate",                              l_cairo_matrix_init_rotate},
    {"translate",                                l_cairo_matrix_translate},
    {"scale",                                    l_cairo_matrix_scale},
    {"rotate",                                   l_cairo_matrix_rotate},
    {"invert",                                   l_cairo_matrix_invert},
    {"multiply",                                 l_cairo_matrix_multiply},
    {"transform_distance",                       l_cairo_matrix_transform_distance},
    {"transform_point",                          l_cairo_matrix_transform_point},
    {NULL, NULL}
};

static const Xet_reg_pre Matrix_getters[] = {
    {"xx",    Xet_get_number, offsetof(cairo_matrix_t, xx) },
    {"yx",    Xet_get_number, offsetof(cairo_matrix_t, yx) },
    {"xy",    Xet_get_number, offsetof(cairo_matrix_t, xy) },
    {"yy",    Xet_get_number, offsetof(cairo_matrix_t, yy) },
    {"x0",    Xet_get_number, offsetof(cairo_matrix_t, x0) },
    {"y0",    Xet_get_number, offsetof(cairo_matrix_t, y0) },
    {0, 0, 0}
};

static const Xet_reg_pre Matrix_setters[] = {
    {"xx",    Xet_set_number, offsetof(cairo_matrix_t, xx) },
    {"yx",    Xet_set_number, offsetof(cairo_matrix_t, yx) },
    {"xy",    Xet_set_number, offsetof(cairo_matrix_t, xy) },
    {"yy",    Xet_set_number, offsetof(cairo_matrix_t, yy) },
    {"x0",    Xet_set_number, offsetof(cairo_matrix_t, x0) },
    {"y0",    Xet_set_number, offsetof(cairo_matrix_t, y0) },
    {0, 0, 0}
};


static int luaopen_lmatrix (lua_State *L)
{
    int metatable, methods;

    /* create methods table, & add it to the table of globals */
    //hd.. luaL_openlib(L, YOUR_T, your_methods, 0);
    luaL_register(L, LCAIRO ".Matrix", Matrix_methods);
    methods = lua_gettop(L);

    /* create metatable for your_t, & add it to the registry */
    luaL_newmetatable(L, LCAIRO ".Matrix.mt");
    //hd.. luaL_openlib(L, 0, your_meta_methods, 0);  /* fill metatable */
    luaL_register(L, NULL, Matrix_meta_methods);
    metatable = lua_gettop(L);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);      /* dup methods table*/
    lua_rawset(L, metatable);       /* hide metatable:
                                       metatable.__metatable = methods */
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, metatable);    /* upvalue index 1 */
    Xet_add(L, Matrix_getters);     /* fill metatable with getters */
    lua_pushvalue(L, methods);      /* upvalue index 2 */
    lua_pushcclosure(L, Xet_index_handler, 2);
    lua_rawset(L, metatable);       /* metatable.__index = index_handler */

    lua_pushliteral(L, "__newindex");
    lua_newtable(L);                /* table for members you can set */
    Xet_add(L, Matrix_setters);     /* fill with setters */
    lua_pushcclosure(L, Xet_newindex_handler, 1);
    lua_rawset(L, metatable);       /* metatable.__newindex = newindex_handler */

    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_Matrix);
    lua_settable(L, -3);
    lua_setmetatable(L, methods);

    lua_pop(L, 1);                  /* drop metatable */
    //hd.. return 1;                /* return methods on the stack */
    lua_pop(L, 1);                  //hd.. drop methods
    return 0;
}

//unused, obsolete
//static int set_Matrix (lua_State *L)
//{
//    cairo_matrix_t *m = check_Matrix_ud(L, 1);
//    const char* field = luaL_checkstring(L, 2);
//    double val = luaL_checknumber(L, 3);
//    if      (0 == strcmp("xx", field)) m->xx = val;
//    else if (0 == strcmp("yx", field)) m->yx = val;
//    else if (0 == strcmp("xy", field)) m->xy = val;
//    else if (0 == strcmp("yy", field)) m->yy = val;
//    else if (0 == strcmp("x0", field)) m->x0 = val;
//    else if (0 == strcmp("y0", field)) m->y0 = val;
//    else luaL_error(L, "unknown field");
//    return 0;
//}
//
//static int get_Matrix (lua_State *L)
//{
//    cairo_matrix_t *m = check_Matrix_ud(L, 1);
//    const char* field = luaL_checkstring(L, 2);
//    double val = 0;
//    if      (0 == strcmp("xx", field)) val = m->xx;
//    else if (0 == strcmp("yx", field)) val = m->yx;
//    else if (0 == strcmp("xy", field)) val = m->xy;
//    else if (0 == strcmp("yy", field)) val = m->yy;
//    else if (0 == strcmp("x0", field)) val = m->x0;
//    else if (0 == strcmp("y0", field)) val = m->y0;
//    else luaL_error(L, "unknown field");
//    lua_pushnumber(L, val);
//    return 1;
//}
//
//static const struct luaL_Reg Matrix_lib_f [] = {
//    {"new", new_Matrix},
//    {NULL, NULL}
//};
//
//static const struct luaL_Reg Matrix_lib_m [] = {
//    {"__newindex", set_Matrix},
//    {"__index",    get_Matrix},
//    {"__tostring", tostring_Matrix},
//    {NULL, NULL}
//};
//
//static int luaopen_lmatrix (lua_State *L)
//{
//    luaL_newmetatable(L, LCAIRO ".Matrix.mt");
//    // lua_pushvalue(L, -1);
//    // lua_setfield(L, -2, "__index");
//    luaL_register(L, NULL, Matrix_lib_m);
//    luaL_register(L, LCAIRO ".Matrix", Matrix_lib_f);
//    lua_newtable(L);
//    lua_pushstring(L, "__call");
//    lua_pushcfunction(L, new_Matrix);
//    lua_settable(L, -3);
//    lua_setmetatable(L, -2);
//    lua_pop(L, 1);
//
//    lua_pop(L, 1);
//    return 0;
//}



///--------------------------------------------------------------------------
/// TextExtents userdata for cairo_text_extents_t
///--------------------------------------------------------------------------

#define check_TextExtents_ud(L, idx) \
    (cairo_text_extents_t *) luaL_checkudata(L, idx, LCAIRO ".TextExtents.mt")

static cairo_text_extents_t check_TextExtents(lua_State* L, int idx)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    lua_pushvalue(L, idx); // copy table to top

    cairo_text_extents_t extents;
    extents.x_bearing = get_numfield(L, "x_bearing");
    extents.y_bearing = get_numfield(L, "y_bearing");
    extents.width     = get_numfield(L, "width");
    extents.height    = get_numfield(L, "height");
    extents.x_advance = get_numfield(L, "x_advance");
    extents.y_advance = get_numfield(L, "y_advance");

    lua_pop(L, 1); // remove copied table
    return extents;
}

static int new_TextExtents (lua_State *L)
{
    cairo_text_extents_t tein = {0, 0, 0, 0, 0, 0};
    int top = lua_gettop(L);
    if (top > 1) tein = check_TextExtents(L, 2);

    cairo_text_extents_t *te = (cairo_text_extents_t *) lua_newuserdata(L, sizeof(cairo_text_extents_t));
    *te = tein;

    luaL_getmetatable(L, LCAIRO ".TextExtents.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int tostring_TextExtents (lua_State *L)
{
    cairo_text_extents_t *te = check_TextExtents_ud(L, 1);
    lua_pushfstring(L, "x_bearing:%f, y_bearing:%f, width:%f, height:%f, x_advance:%f, y_advance:%f",
        te->x_bearing, te->y_bearing, te->width, te->height, te->x_advance, te->y_advance);
    return 1;
}

static const struct luaL_Reg TextExtents_meta_methods[] = {
    {"__tostring",                               tostring_TextExtents},
    {NULL, NULL}
};

static const struct luaL_Reg TextExtents_methods[] = {
    {"new",                                      new_TextExtents},
    {NULL, NULL}
};

static const Xet_reg_pre TextExtents_getters[] = {
    {"x_bearing",    Xet_get_number, offsetof(cairo_text_extents_t, x_bearing) },
    {"y_bearing",    Xet_get_number, offsetof(cairo_text_extents_t, y_bearing) },
    {"width",        Xet_get_number, offsetof(cairo_text_extents_t, width)     },
    {"height",       Xet_get_number, offsetof(cairo_text_extents_t, height)    },
    {"x_advance",    Xet_get_number, offsetof(cairo_text_extents_t, x_advance) },
    {"y_advance",    Xet_get_number, offsetof(cairo_text_extents_t, y_advance) },
    {0, 0, 0}
};

static const Xet_reg_pre TextExtents_setters[] = {
    {"x_bearing",    Xet_set_number, offsetof(cairo_text_extents_t, x_bearing) },
    {"y_bearing",    Xet_set_number, offsetof(cairo_text_extents_t, y_bearing) },
    {"width",        Xet_set_number, offsetof(cairo_text_extents_t, width)     },
    {"height",       Xet_set_number, offsetof(cairo_text_extents_t, height)    },
    {"x_advance",    Xet_set_number, offsetof(cairo_text_extents_t, x_advance) },
    {"y_advance",    Xet_set_number, offsetof(cairo_text_extents_t, y_advance) },
    {0, 0, 0}
};


static int luaopen_ltextextents (lua_State *L)
{
    int metatable, methods;

    /* create methods table, & add it to the table of globals */
    //hd.. luaL_openlib(L, YOUR_T, your_methods, 0);
    luaL_register(L, LCAIRO ".TextExtents", TextExtents_methods);
    methods = lua_gettop(L);

    /* create metatable for your_t, & add it to the registry */
    luaL_newmetatable(L, LCAIRO ".TextExtents.mt");
    //hd.. luaL_openlib(L, 0, your_meta_methods, 0);  /* fill metatable */
    luaL_register(L, NULL, TextExtents_meta_methods);
    metatable = lua_gettop(L);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);           /* dup methods table*/
    lua_rawset(L, metatable);            /* hide metatable:
                                            metatable.__metatable = methods */
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, metatable);         /* upvalue index 1 */
    Xet_add(L, TextExtents_getters);     /* fill metatable with getters */
    lua_pushvalue(L, methods);           /* upvalue index 2 */
    lua_pushcclosure(L, Xet_index_handler, 2);
    lua_rawset(L, metatable);            /* metatable.__index = index_handler */

    lua_pushliteral(L, "__newindex");
    lua_newtable(L);                     /* table for members you can set */
    Xet_add(L, TextExtents_setters);     /* fill with setters */
    lua_pushcclosure(L, Xet_newindex_handler, 1);
    lua_rawset(L, metatable);            /* metatable.__newindex = newindex_handler */

    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_TextExtents);
    lua_settable(L, -3);
    lua_setmetatable(L, methods);

    lua_pop(L, 1);                       /* drop metatable */
    //hd.. return 1;                     /* return methods on the stack */
    lua_pop(L, 1);                       //hd.. drop methods
    return 0;
}

//unused, obsolete
//static int set_TextExtents (lua_State *L)
//{
//    cairo_text_extents_t *te = check_TextExtents_ud(L, 1);
//    const char* field = luaL_checkstring(L, 2);
//    double val = luaL_checknumber(L, 3);
//    if      (0 == strcmp("x_bearing", field)) te->x_bearing = val;
//    else if (0 == strcmp("y_bearing", field)) te->y_bearing = val;
//    else if (0 == strcmp("width",     field)) te->width = val;
//    else if (0 == strcmp("height",    field)) te->height = val;
//    else if (0 == strcmp("x_advance", field)) te->x_advance = val;
//    else if (0 == strcmp("y_advance", field)) te->y_advance = val;
//    else luaL_error(L, "unknown field");
//    return 0;
//}
//
//static int get_TextExtents (lua_State *L)
//{
//    cairo_text_extents_t *te = check_TextExtents_ud(L, 1);
//    const char* field = luaL_checkstring(L, 2);
//    double val = 0;
//    if      (0 == strcmp("x_bearing", field)) val = te->x_bearing;
//    else if (0 == strcmp("y_bearing", field)) val = te->y_bearing;
//    else if (0 == strcmp("width",     field)) val = te->width;
//    else if (0 == strcmp("height",    field)) val = te->height;
//    else if (0 == strcmp("x_advance", field)) val = te->x_advance;
//    else if (0 == strcmp("y_advance", field)) val = te->y_advance;
//    else luaL_error(L, "unknown field");
//    lua_pushnumber(L, val);
//    return 1;
//}
//
//static const struct luaL_Reg TextExtents_lib_f [] = {
//    {"new", new_TextExtents},
//    {NULL, NULL}
//};
//
//static const struct luaL_Reg TextExtents_lib_m [] = {
//    {"__newindex", set_TextExtents},
//    {"__index",    get_TextExtents},
//    {"__tostring", tostring_TextExtents},
//    {NULL, NULL}
//};
//
//static int luaopen_ltextextents (lua_State *L)
//{
//    luaL_newmetatable(L, LCAIRO ".TextExtents.mt");
//    // lua_pushvalue(L, -1);
//    // lua_setfield(L, -2, "__index");
//    luaL_register(L, NULL, TextExtents_lib_m);
//    luaL_register(L, LCAIRO ".TextExtents", TextExtents_lib_f);
//    lua_newtable(L);
//    lua_pushstring(L, "__call");
//    lua_pushcfunction(L, new_TextExtents);
//    lua_settable(L, -3);
//    lua_setmetatable(L, -2);
//    lua_pop(L, 1);
//
//    lua_pop(L, 1);
//    return 0;
//}




///--------------------------------------------------------------------------
/// FontExtents userdata for cairo_font_extents_t
///--------------------------------------------------------------------------

#define check_FontExtents_ud(L, idx) \
    (cairo_font_extents_t *) luaL_checkudata(L, idx, LCAIRO ".FontExtents.mt")

static cairo_font_extents_t check_FontExtents (lua_State* L, int idx)
{
    luaL_checktype(L, idx, LUA_TTABLE);
    lua_pushvalue(L, idx); // copy table to top

    cairo_font_extents_t extents;
    extents.ascent        = get_numfield(L, "ascent");
    extents.descent       = get_numfield(L, "descent");
    extents.height        = get_numfield(L, "height");
    extents.max_x_advance = get_numfield(L, "max_x_advance");
    extents.max_y_advance = get_numfield(L, "max_y_advance");

    lua_pop(L, 1); // remove copied table
    return extents;
}

static int new_FontExtents (lua_State *L)
{
    cairo_font_extents_t fein = {0, 0, 0, 0, 0};
    int top = lua_gettop(L);
    if (top > 1) fein = check_FontExtents (L, 2);

    cairo_font_extents_t *fe = (cairo_font_extents_t *) lua_newuserdata(L, sizeof(cairo_font_extents_t));
    *fe = fein;

    luaL_getmetatable(L, LCAIRO ".FontExtents.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int tostring_FontExtents (lua_State *L)
{
    cairo_font_extents_t *fe = check_FontExtents_ud(L, 1);
    lua_pushfstring(L, "ascent:%f, descent:%f, height:%f, max_x_advance:%f, max_y_advance:%f",
        fe->ascent, fe->descent, fe->height, fe->max_x_advance, fe->max_y_advance);
    return 1;
}

static const struct luaL_Reg FontExtents_meta_methods[] = {
    {"__tostring",                               tostring_FontExtents},
    {NULL, NULL}
};

static const struct luaL_Reg FontExtents_methods[] = {
    {"new",                                      new_FontExtents},
    {NULL, NULL}
};

static const Xet_reg_pre FontExtents_getters[] = {
    {"ascent",        Xet_get_number, offsetof(cairo_font_extents_t, ascent)        },
    {"descent",       Xet_get_number, offsetof(cairo_font_extents_t, descent)       },
    {"height",        Xet_get_number, offsetof(cairo_font_extents_t, height)        },
    {"max_x_advance", Xet_get_number, offsetof(cairo_font_extents_t, max_x_advance) },
    {"max_y_advance", Xet_get_number, offsetof(cairo_font_extents_t, max_y_advance) },
    {0, 0, 0}
};

static const Xet_reg_pre FontExtents_setters[] = {
    {"ascent",        Xet_set_number, offsetof(cairo_font_extents_t, ascent)        },
    {"descent",       Xet_set_number, offsetof(cairo_font_extents_t, descent)       },
    {"height",        Xet_set_number, offsetof(cairo_font_extents_t, height)        },
    {"max_x_advance", Xet_set_number, offsetof(cairo_font_extents_t, max_x_advance) },
    {"max_y_advance", Xet_set_number, offsetof(cairo_font_extents_t, max_y_advance) },
    {0, 0, 0}
};


static int luaopen_lfontextents (lua_State *L)
{
    int metatable, methods;

    /* create methods table, & add it to the table of globals */
    //hd.. luaL_openlib(L, YOUR_T, your_methods, 0);
    luaL_register(L, LCAIRO ".FontExtents", FontExtents_methods);
    methods = lua_gettop(L);

    /* create metatable for your_t, & add it to the registry */
    luaL_newmetatable(L, LCAIRO ".FontExtents.mt");
    //hd.. luaL_openlib(L, 0, your_meta_methods, 0);  /* fill metatable */
    luaL_register(L, NULL, FontExtents_meta_methods);
    metatable = lua_gettop(L);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);           /* dup methods table*/
    lua_rawset(L, metatable);            /* hide metatable:
                                            metatable.__metatable = methods */
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, metatable);         /* upvalue index 1 */
    Xet_add(L, FontExtents_getters);     /* fill metatable with getters */
    lua_pushvalue(L, methods);           /* upvalue index 2 */
    lua_pushcclosure(L, Xet_index_handler, 2);
    lua_rawset(L, metatable);            /* metatable.__index = index_handler */

    lua_pushliteral(L, "__newindex");
    lua_newtable(L);                     /* table for members you can set */
    Xet_add(L, FontExtents_setters);     /* fill with setters */
    lua_pushcclosure(L, Xet_newindex_handler, 1);
    lua_rawset(L, metatable);            /* metatable.__newindex = newindex_handler */

    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_FontExtents);
    lua_settable(L, -3);
    lua_setmetatable(L, methods);

    lua_pop(L, 1);                       /* drop metatable */
    //hd.. return 1;                     /* return methods on the stack */
    lua_pop(L, 1);                       //hd.. drop methods
    return 0;
}

//unused, obsolete
//static int set_FontExtents (lua_State *L)
//{
//    cairo_font_extents_t *fe = check_FontExtents_ud(L, 1);
//    const char* field = luaL_checkstring(L, 2);
//    double val = luaL_checknumber(L, 3);
//    if      (0 == strcmp("ascent",        field)) fe->ascent = val;
//    else if (0 == strcmp("descent",       field)) fe->descent = val;
//    else if (0 == strcmp("height",        field)) fe->height= val;
//    else if (0 == strcmp("max_x_advance", field)) fe->max_x_advance = val;
//    else if (0 == strcmp("max_y_advance", field)) fe->max_y_advance = val;
//    else luaL_error(L, "unknown field");
//    return 0;
//}
//
//static int get_FontExtents (lua_State *L)
//{
//    cairo_font_extents_t *fe = check_FontExtents_ud(L, 1);
//    const char* field = luaL_checkstring(L, 2);
//    double val = 0;
//    if      (0 == strcmp("ascent",        field)) val = fe->ascent;
//    else if (0 == strcmp("descent",       field)) val = fe->descent;
//    else if (0 == strcmp("height",        field)) val = fe->height;
//    else if (0 == strcmp("max_x_advance", field)) val = fe->max_x_advance;
//    else if (0 == strcmp("max_y_advance", field)) val = fe->max_y_advance;
//    else luaL_error(L, "unknown field");
//    lua_pushnumber(L, val);
//    return 1;
//}
//
//static const struct luaL_Reg FontExtents_lib_f [] = {
//    {"new", new_FontExtents},
//    {NULL, NULL}
//};
//
//static const struct luaL_Reg FontExtents_lib_m [] = {
//    {"__newindex", set_FontExtents},
//    {"__index",    get_FontExtents},
//    {"__tostring", tostring_FontExtents},
//    {NULL, NULL}
//};
//
//static int luaopen_lfontextents (lua_State *L)
//{
//    luaL_newmetatable(L, LCAIRO ".FontExtents.mt");                  // mt
//  //lua_pushvalue(L, -1);                                            // mt mt
//  //lua_setfield(L, -2, "__index");                                  // mt          (mt.__index = mt)
//    luaL_register(L, NULL, FontExtents_lib_m);                       // mt
//    luaL_register(L, LCAIRO ".FontExtents", FontExtents_lib_f);      // mt ft
//    lua_newtable(L);                                                 // mt ft t
//    lua_pushstring(L, "__call");                                     // mt ft t "__call"
//    lua_pushcfunction(L, new_FontExtents);                           // mt ft t "__call" fun
//    lua_settable(L, -3);                                             // mt ft t     (t.__call = fun)
//    lua_setmetatable(L, -2);                                         // mt ft       (ft.meta = t)
//    lua_pop(L, 1);                                                   // mt
//
//    lua_pop(L, 1);
//    return 0;
//}




///--------------------------------------------------------------------------
/// Context userdata for cairo_t
///--------------------------------------------------------------------------
#if LUACAIRO_HAS_OO_INTERFACE

typedef struct _Context
{
    cairo_t* cr_;
    int      havecr_;
} Context;

#define check_Context_ud(L, idx) \
    (Context *) luaL_checkudata(L, idx, LCAIRO ".Context.mt")

static int new_Context (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.Context

  //{"create",                                   l_cairo_create},
    cairo_surface_t* target = get_cairo_surface_t (L, 1);
    cairo_t *cr = cairo_create(target);

    Context *o = (Context *) lua_newuserdata(L, sizeof(Context));
    o->cr_     = cr;
    o->havecr_ = 1;

    luaL_getmetatable(L, LCAIRO ".Context.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int new_ContextFromLUD (lua_State *L)
{
    cairo_t *cr = (cairo_t *) check_lightuserdata(L, 1);
    int havecr  = lua_toboolean(L, 2);

    Context *o = (Context *) lua_newuserdata(L, sizeof(Context));
    o->cr_     = cr;
    o->havecr_ = havecr;

    luaL_getmetatable(L, LCAIRO ".Context.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int gc_Context (lua_State *L)
{
    Context *o = check_Context_ud(L, 1);

    if (o->cr_ && o->havecr_)
    {
        cairo_destroy (o->cr_);
        o->cr_     = NULL;
        o->havecr_ = 0;
    }

    return 0;
}

static int tostring_Context (lua_State *L)
{
    Context *o = check_Context_ud(L, 1);
    lua_pushfstring(L, "Context (%p), cairo_t (%p)", (void*)o, (void*)o->cr_);
    return 1;
}

static const struct luaL_Reg Context_lib_f [] = {
    {"new", new_Context},
    {NULL, NULL}
};

static const struct luaL_Reg Context_lib_m [] = {
    {"__gc",                                     gc_Context},
    {"__tostring",                               tostring_Context},

    {"version",                                  l_cairo_version},
    {"version_string",                           l_cairo_version_string},
  //{"create",                                   l_cairo_create},
    {"reference",                                l_cairo_reference},
    {"destroy",                                  gc_Context},
    {"get_reference_count",                      l_cairo_get_reference_count},
    {"get_user_data",                            l_cairo_get_user_data},
    {"set_user_data",                            l_cairo_set_user_data},
    {"save",                                     l_cairo_save},
    {"restore",                                  l_cairo_restore},
    {"push_group",                               l_cairo_push_group},
    {"push_group_with_content",                  l_cairo_push_group_with_content},
    {"pop_group",                                l_cairo_pop_group},
    {"pop_group_to_source",                      l_cairo_pop_group_to_source},
    {"set_operator",                             l_cairo_set_operator},
    {"set_source",                               l_cairo_set_source},
    {"set_source_rgb",                           l_cairo_set_source_rgb},
    {"set_source_rgba",                          l_cairo_set_source_rgba},
    {"set_source_surface",                       l_cairo_set_source_surface},
    {"set_tolerance",                            l_cairo_set_tolerance},
    {"set_antialias",                            l_cairo_set_antialias},
    {"set_fill_rule",                            l_cairo_set_fill_rule},
    {"set_line_width",                           l_cairo_set_line_width},
    {"set_line_cap",                             l_cairo_set_line_cap},
    {"set_line_join",                            l_cairo_set_line_join},
    {"set_dash",                                 l_cairo_set_dash},
    {"set_miter_limit",                          l_cairo_set_miter_limit},
    {"translate",                                l_cairo_translate},
    {"scale",                                    l_cairo_scale},
    {"rotate",                                   l_cairo_rotate},
    {"transform",                                l_cairo_transform},
    {"set_matrix",                               l_cairo_set_matrix},
    {"identity_matrix",                          l_cairo_identity_matrix},
    {"user_to_device",                           l_cairo_user_to_device},
    {"user_to_device_distance",                  l_cairo_user_to_device_distance},
    {"device_to_user",                           l_cairo_device_to_user},
    {"device_to_user_distance",                  l_cairo_device_to_user_distance},
    {"new_path",                                 l_cairo_new_path},
    {"move_to",                                  l_cairo_move_to},
    {"new_sub_path",                             l_cairo_new_sub_path},
    {"line_to",                                  l_cairo_line_to},
    {"curve_to",                                 l_cairo_curve_to},
    {"arc",                                      l_cairo_arc},
    {"arc_negative",                             l_cairo_arc_negative},
    {"rel_move_to",                              l_cairo_rel_move_to},
    {"rel_line_to",                              l_cairo_rel_line_to},
    {"rel_curve_to",                             l_cairo_rel_curve_to},
    {"rectangle",                                l_cairo_rectangle},
    {"close_path",                               l_cairo_close_path},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"path_extents",                             l_cairo_path_extents},
#endif
    {"paint",                                    l_cairo_paint},
    {"paint_with_alpha",                         l_cairo_paint_with_alpha},
    {"mask",                                     l_cairo_mask},
    {"mask_surface",                             l_cairo_mask_surface},
    {"stroke",                                   l_cairo_stroke},
    {"stroke_preserve",                          l_cairo_stroke_preserve},
    {"fill",                                     l_cairo_fill},
    {"fill_preserve",                            l_cairo_fill_preserve},
    {"copy_page",                                l_cairo_copy_page},
    {"show_page",                                l_cairo_show_page},
    {"in_stroke",                                l_cairo_in_stroke},
    {"in_fill",                                  l_cairo_in_fill},
    {"stroke_extents",                           l_cairo_stroke_extents},
    {"fill_extents",                             l_cairo_fill_extents},
    {"reset_clip",                               l_cairo_reset_clip},
    {"clip",                                     l_cairo_clip},
    {"clip_preserve",                            l_cairo_clip_preserve},
    {"clip_extents",                             l_cairo_clip_extents},
    {"copy_clip_rectangle_list",                 l_cairo_copy_clip_rectangle_list},
    {"rectangle_list_destroy",                   l_cairo_rectangle_list_destroy},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"glyph_allocate",                           l_cairo_glyph_allocate},
    {"glyph_free",                               l_cairo_glyph_free},
    {"text_cluster_allocate",                    l_cairo_text_cluster_allocate},
    {"text_cluster_free",                        l_cairo_text_cluster_free},
#endif
    {"select_font_face",                         l_cairo_select_font_face},
    {"set_font_size",                            l_cairo_set_font_size},
    {"set_font_matrix",                          l_cairo_set_font_matrix},
    {"get_font_matrix",                          l_cairo_get_font_matrix},
    {"set_font_options",                         l_cairo_set_font_options},
    {"get_font_options",                         l_cairo_get_font_options},
    {"set_font_face",                            l_cairo_set_font_face},
    {"get_font_face",                            l_cairo_get_font_face},
    {"set_scaled_font",                          l_cairo_set_scaled_font},
    {"get_scaled_font",                          l_cairo_get_scaled_font},
    {"show_text",                                l_cairo_show_text},
    {"show_glyphs",                              l_cairo_show_glyphs},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"show_text_glyphs",                         l_cairo_show_text_glyphs},
#endif
    {"text_path",                                l_cairo_text_path},
    {"glyph_path",                               l_cairo_glyph_path},
    {"text_extents",                             l_cairo_text_extents},
    {"glyph_extents",                            l_cairo_glyph_extents},
    {"font_extents",                             l_cairo_font_extents},
    {"get_operator",                             l_cairo_get_operator},
    {"get_source",                               l_cairo_get_source},
    {"get_tolerance",                            l_cairo_get_tolerance},
    {"get_antialias",                            l_cairo_get_antialias},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"has_current_point",                        l_cairo_has_current_point},
#endif
    {"get_current_point",                        l_cairo_get_current_point},
    {"get_fill_rule",                            l_cairo_get_fill_rule},
    {"get_line_width",                           l_cairo_get_line_width},
    {"get_line_cap",                             l_cairo_get_line_cap},
    {"get_line_join",                            l_cairo_get_line_join},
    {"get_miter_limit",                          l_cairo_get_miter_limit},
    {"get_dash_count",                           l_cairo_get_dash_count},
    {"get_dash",                                 l_cairo_get_dash},
    {"get_matrix",                               l_cairo_get_matrix},
    {"get_target",                               l_cairo_get_target},
    {"get_group_target",                         l_cairo_get_group_target},
    {"copy_path",                                l_cairo_copy_path},
    {"copy_path_flat",                           l_cairo_copy_path_flat},
    {"append_path",                              l_cairo_append_path},
    {"path_destroy",                             l_cairo_path_destroy},
    {"status",                                   l_cairo_status},
    {"status_to_string",                         l_cairo_status_to_string},
  //{"debug_reset_static_data",                  l_cairo_debug_reset_static_data},

    {NULL, NULL}
};


static int luaopen_lcontext (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".Context.mt");          // mt
    lua_pushvalue(L, -1);                                // mt mt
    lua_setfield(L, -2, "__index");                      // mt       (mt.__index = mt)
    luaL_register(L, NULL, Context_lib_m);               // mt
    luaL_register(L, LCAIRO ".Context", Context_lib_f);  // mt ct
    lua_newtable(L);                                     // mt ct t
    lua_pushstring(L, "__call");                         // mt ct t "__call"
    lua_pushcfunction(L, new_Context);                   // mt ct t "__call" fun
    lua_settable(L, -3);                                 // mt ct t  (t.__call = fun)
    lua_setmetatable(L, -2);                             // mt ct    (ct.meta = t)
    lua_pop(L, 1);                                       // mt

    lua_pop(L, 1);
    return 0;
}

#endif  /* LUACAIRO_HAS_OO_INTERFACE */



///--------------------------------------------------------------------------
/// Surface userdata for cairo_surface_t
///--------------------------------------------------------------------------
#if LUACAIRO_HAS_OO_INTERFACE

typedef struct _Surface
{
    cairo_surface_t*  cs_;
    int               havecs_;
    int               typecs_;
} Surface;


static Surface* check_Surface_ud(lua_State *L, int idx)
{
    Surface* cs = NULL;

//?? #if CAIRO_HAS_IMAGE_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".ImageSurface.mt");
    // if (cs == NULL) printf("cs is not ImageSurface\n");
//?? #endif

#if CAIRO_HAS_PDF_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".PdfSurface.mt");
    // if (cs == NULL) printf("cs is not PdfSurface\n");
#endif

#if CAIRO_HAS_PS_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".PsSurface.mt");
    // if (cs == NULL) printf("cs is not PsSurface\n");
#endif

#if CAIRO_HAS_XLIB_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".XlibSurface.mt");
    // if (cs == NULL) printf("cs is not XlibSurface\n");
#endif

#if CAIRO_HAS_XCB_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".XcbSurface.mt");
    // if (cs == NULL) printf("cs is not XcbSurface\n");
#endif

#if CAIRO_HAS_GLITZ_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".GlitzSurface.mt");
    // if (cs == NULL) printf("cs is not GlitzSurface\n");
#endif

#if CAIRO_HAS_QUARTZ_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".QuartzSurface.mt");
    // if (cs == NULL) printf("cs is not QuartzSurface\n");
#endif

#if CAIRO_HAS_WIN32_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".Win32Surface.mt");
    // if (cs == NULL) printf("cs is not Win32Surface\n");
#endif

#if CAIRO_HAS_BEOS_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".BeosSurface.mt");
    // if (cs == NULL) printf("cs is not BeosSurface\n");
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".DirectFbSurface.mt");
    // if (cs == NULL) printf("cs is not DirectFbSurface\n");
#endif

#if CAIRO_HAS_SVG_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".SvgSurface.mt");
    // if (cs == NULL) printf("cs is not SvgSurface\n");
#endif

#if CAIRO_HAS_OS2_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".Os2Surface.mt");
    // if (cs == NULL) printf("cs is not Os2Surface\n");
#endif

#if CAIRO_HAS_WIN32_SURFACE
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".Win32PrintingSurface.mt");
    // if (cs == NULL) printf("cs is not Win32PrintingSurface\n");
#endif
#endif

#if CAIRO_HAS_QUARTZ_IMAGE_SURFACE
    if (cs == NULL) cs = (Surface*) get_userdata(L, idx, LCAIRO ".QuartzImageSurface.mt");
    // if (cs == NULL) printf("cs is not QuartzImageSurface\n");
#endif

    if (cs == NULL) luaL_typerror(L, idx, LCAIRO ".Surface");

    return cs;
}

static int new_Surface (lua_State *L, const char *tname, cairo_surface_t* cs, int typecs, int havecs)
{
    Surface *o = (Surface *) lua_newuserdata(L, sizeof(Surface));
    o->cs_     = cs;
    o->havecs_ = havecs;
    o->typecs_ = typecs;

    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);

    return 1;
}

static int new_SurfaceFromLUD (lua_State *L)
{
    cairo_surface_t *cs = (cairo_surface_t *) check_lightuserdata(L, 1);
    int havecs          = lua_toboolean(L, 2);
    int typecs          = cairo_surface_get_type(cs);

    switch (typecs) {
    case CAIRO_SURFACE_TYPE_IMAGE:          return new_Surface(L, LCAIRO ".ImageSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_PDF:            return new_Surface(L, LCAIRO ".PdfSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_PS:             return new_Surface(L, LCAIRO ".PsSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_XLIB:           return new_Surface(L, LCAIRO ".XlibSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_XCB:            return new_Surface(L, LCAIRO ".XcbSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_GLITZ:          return new_Surface(L, LCAIRO ".GlitzSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_QUARTZ:         return new_Surface(L, LCAIRO ".QuartzSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_WIN32:          return new_Surface(L, LCAIRO ".Win32Surface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_BEOS:           return new_Surface(L, LCAIRO ".BeosSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_DIRECTFB:       return new_Surface(L, LCAIRO ".DirectFbSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_SVG:            return new_Surface(L, LCAIRO ".SvgSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_OS2:            return new_Surface(L, LCAIRO ".Os2Surface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_WIN32_PRINTING: return new_Surface(L, LCAIRO ".Win32PrintingSurface.mt", cs, typecs, havecs);
    case CAIRO_SURFACE_TYPE_QUARTZ_IMAGE:   return new_Surface(L, LCAIRO ".QuartzImageSurface.mt", cs, typecs, havecs);
    }

    return 0;
}

static int gc_Surface (lua_State *L)
{
    Surface *o = check_Surface_ud(L, 1);

    if (o->cs_ && o->havecs_)
    {
        cairo_surface_destroy(o->cs_);
        o->cs_     = NULL;
        o->havecs_ = 0;
        o->typecs_ = 0;
    }

    return 0;
}

static int tostring_Surface (lua_State *L)
{
    Surface *o = check_Surface_ud(L, 1);
    lua_pushfstring(L, "Surface (%p), cairo_surface_t (%p), Type:%d", (void*)o, (void*)o->cs_, o->typecs_);
    return 1;
}



//---------------------------------------------------------------------------
// ImageSurface userdata for oo usage
//---------------------------------------------------------------------------
//??#if CAIRO_HAS_IMAGE_SURFACE

static int new_ImageSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.ImageSurface

    //FIXME
    //{"create_for_data",                          l_cairo_image_surface_create_for_data},
    //{"create_from_png",                          l_cairo_image_surface_create_from_png},
    //{"create_from_png_stream",                   l_cairo_image_surface_create_from_png_stream},

    //{"create",                                   l_cairo_image_surface_create},
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    cairo_surface_t *cs = cairo_image_surface_create (format, width, height);

    return new_Surface(L, LCAIRO ".ImageSurface.mt", cs, CAIRO_SURFACE_TYPE_IMAGE, 1);
}

static const struct luaL_Reg ImageSurface_lib_f [] = {
    {"new", new_ImageSurface},
    {NULL, NULL}
};

static const struct luaL_Reg ImageSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif
  //{"create",                                   l_cairo_image_surface_create},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"format_stride_for_width",                  l_cairo_format_stride_for_width},
#endif
  //{"create_for_data",                          l_cairo_image_surface_create_for_data},
    {"get_data",                                 l_cairo_image_surface_get_data},
    {"get_format",                               l_cairo_image_surface_get_format},
    {"get_width",                                l_cairo_image_surface_get_width},
    {"get_height",                               l_cairo_image_surface_get_height},
    {"get_stride",                               l_cairo_image_surface_get_stride},
#if CAIRO_HAS_PNG_FUNCTIONS
  //{"create_from_png",                          l_cairo_image_surface_create_from_png},
  //{"create_from_png_stream",                   l_cairo_image_surface_create_from_png_stream},
#endif

    {NULL, NULL}
};


static int luaopen_lImageSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".ImageSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, ImageSurface_lib_m);
    luaL_register(L, LCAIRO ".ImageSurface", ImageSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_ImageSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

//?? #endif  /* CAIRO_HAS_IMAGE_SURFACE */



//---------------------------------------------------------------------------
// PdfSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_PDF_SURFACE

static int new_PdfSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.PdfSurface

    //FIXME
    //{"create_for_stream",            l_cairo_pdf_surface_create_for_stream},

    //{"create",                       l_cairo_pdf_surface_create},
    const char *filename = luaL_checkstring(L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_surface_t *cs = cairo_pdf_surface_create(filename, width_in_points, height_in_points);

    return new_Surface(L, LCAIRO ".PdfSurface.mt", cs, CAIRO_SURFACE_TYPE_PDF, 1);
}

static const struct luaL_Reg PdfSurface_lib_f [] = {
    {"new", new_PdfSurface},
    {NULL, NULL}
};

static const struct luaL_Reg PdfSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_PDF_SURFACE
  //{"create",                                   l_cairo_pdf_surface_create},
  //{"create_for_stream",                        l_cairo_pdf_surface_create_for_stream},
    {"set_size",                                 l_cairo_pdf_surface_set_size},
#endif

    {NULL, NULL}
};

static int luaopen_lPdfSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".PdfSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, PdfSurface_lib_m);
    luaL_register(L, LCAIRO ".PdfSurface", PdfSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_PdfSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_PDF_SURFACE */



//---------------------------------------------------------------------------
// PsSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_PS_SURFACE

static int new_PsSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.PsSurface

    //FIXME
    //{"create_for_stream",             l_cairo_ps_surface_create_for_stream},

    //{"create",                        l_cairo_ps_surface_create},
    const char *filename = luaL_checkstring(L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_surface_t *cs = cairo_ps_surface_create(filename, width_in_points, height_in_points);

    return new_Surface(L, LCAIRO ".PsSurface.mt", cs, CAIRO_SURFACE_TYPE_PS, 1);
}

static const struct luaL_Reg PsSurface_lib_f [] = {
    {"new", new_PsSurface},
    {NULL, NULL}
};

static const struct luaL_Reg PsSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_PS_SURFACE
  //{"create",                                   l_cairo_ps_surface_create},
  //{"create_for_stream",                        l_cairo_ps_surface_create_for_stream},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"restrict_to_level",                        l_cairo_ps_surface_restrict_to_level},
    {"get_levels",                               l_cairo_ps_get_levels},
    {"level_to_string",                          l_cairo_ps_level_to_string},
    {"set_eps",                                  l_cairo_ps_surface_set_eps},
    {"get_eps",                                  l_cairo_ps_surface_get_eps},
#endif
    {"set_size",                                 l_cairo_ps_surface_set_size},
    {"dsc_comment",                              l_cairo_ps_surface_dsc_comment},
    {"dsc_begin_setup",                          l_cairo_ps_surface_dsc_begin_setup},
    {"dsc_begin_page_setup",                     l_cairo_ps_surface_dsc_begin_page_setup},
#endif

    {NULL, NULL}
};

static int luaopen_lPsSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".PsSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, PsSurface_lib_m);
    luaL_register(L, LCAIRO ".PsSurface", PsSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_PsSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_PS_SURFACE */



//---------------------------------------------------------------------------
// XlibSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_XLIB_SURFACE

static int new_XlibSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.XlibSurface

    //FIXME
    //{"create",                      l_cairo_xlib_surface_create},
    //{"create_for_bitmap",           l_cairo_xlib_surface_create_for_bitmap},
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
    //{"create_with_xrender_format",  l_cairo_xlib_surface_create_with_xrender_format},
#endif

    cairo_surface_t* cs = NULL;

    return new_Surface(L, LCAIRO ".XlibSurface.mt", cs, CAIRO_SURFACE_TYPE_XLIB, 1);
}

static const struct luaL_Reg XlibSurface_lib_f [] = {
    {"new", new_XlibSurface},
    {NULL, NULL}
};

static const struct luaL_Reg XlibSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_XLIB_SURFACE
  //{"create",                                   l_cairo_xlib_surface_create},
  //{"create_for_bitmap",                        l_cairo_xlib_surface_create_for_bitmap},
    {"set_size",                                 l_cairo_xlib_surface_set_size},
    {"set_drawable",                             l_cairo_xlib_surface_set_drawable},
    {"get_display",                              l_cairo_xlib_surface_get_display},
    {"get_drawable",                             l_cairo_xlib_surface_get_drawable},
    {"get_screen",                               l_cairo_xlib_surface_get_screen},
    {"get_visual",                               l_cairo_xlib_surface_get_visual},
    {"get_depth",                                l_cairo_xlib_surface_get_depth},
    {"get_width",                                l_cairo_xlib_surface_get_width},
    {"get_height",                               l_cairo_xlib_surface_get_height},
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
  //{"create_with_xrender_format",               l_cairo_xlib_surface_create_with_xrender_format},
#endif
#endif

    {NULL, NULL}
};

static int luaopen_lXlibSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".XlibSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, XlibSurface_lib_m);
    luaL_register(L, LCAIRO ".XlibSurface", XlibSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_XlibSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_XLIB_SURFACE */



//---------------------------------------------------------------------------
// XcbSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_XCB_SURFACE

static int new_XcbSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.XcbSurface

    //FIXME
    //{"create",                                   l_cairo_xcb_surface_create},
    //{"create_for_bitmap",                        l_cairo_xcb_surface_create_for_bitmap},
    //{"create_with_xrender_format",               l_cairo_xcb_surface_create_with_xrender_format},

    cairo_surface_t* cs = NULL;

    return new_Surface(L, LCAIRO ".XcbSurface.mt", cs, CAIRO_SURFACE_TYPE_XCB, 1);
}

static const struct luaL_Reg XcbSurface_lib_f [] = {
    {"new", new_XcbSurface},
    {NULL, NULL}
};

static const struct luaL_Reg XcbSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_XCB_SURFACE
  //{"create",                                   l_cairo_xcb_surface_create},
  //{"create_for_bitmap",                        l_cairo_xcb_surface_create_for_bitmap},
    {"set_size",                                 l_cairo_xcb_surface_set_size},
  //{"create_with_xrender_format",               l_cairo_xcb_surface_create_with_xrender_format},
#endif

    {NULL, NULL}
};

static int luaopen_lXcbSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".XcbSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, XcbSurface_lib_m);
    luaL_register(L, LCAIRO ".XcbSurface", XcbSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_XcbSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_XCB_SURFACE */



//---------------------------------------------------------------------------
// GlitzSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_GLITZ_SURFACE

static int new_GlitzSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.GlitzSurface

    //{"create",                                   l_cairo_glitz_surface_create},
    glitz_surface_t *surface = (glitz_surface_t *) check_lightuserdata(L, 1);
    cairo_surface_t *cs = cairo_glitz_surface_create (surface);

    return new_Surface(L, LCAIRO ".GlitzSurface.mt", cs, CAIRO_SURFACE_TYPE_GLITZ, 1);
}

static const struct luaL_Reg GlitzSurface_lib_f [] = {
    {"new", new_GlitzSurface},
    {NULL, NULL}
};

static const struct luaL_Reg GlitzSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_GLITZ_SURFACE
  //{"create",                                   l_cairo_glitz_surface_create},
#endif

    {NULL, NULL}
};

static int luaopen_lGlitzSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".GlitzSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, GlitzSurface_lib_m);
    luaL_register(L, LCAIRO ".GlitzSurface", GlitzSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_GlitzSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_GLITZ_SURFACE */



//---------------------------------------------------------------------------
// QuartzSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_QUARTZ_SURFACE

static int new_QuartzSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.QuartzSurface

    //FIXME
    //{"create_for_cg_context",                    l_cairo_quartz_surface_create_for_cg_context},

    //{"create",                                   l_cairo_quartz_surface_create},
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 1);
    unsigned int width = luaL_checknumber(L, 2);
    unsigned int height = luaL_checknumber(L, 3);
    cairo_surface_t *cs = cairo_quartz_surface_create (format, width, height);

    return new_Surface(L, LCAIRO ".QuartzSurface.mt", cs, CAIRO_SURFACE_TYPE_QUARTZ, 1);
}

static const struct luaL_Reg QuartzSurface_lib_f [] = {
    {"new", new_QuartzSurface},
    {NULL, NULL}
};

static const struct luaL_Reg QuartzSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_QUARTZ_SURFACE
  //{"create",                                   l_cairo_quartz_surface_create},
  //{"create_for_cg_context",                    l_cairo_quartz_surface_create_for_cg_context},
    {"get_cg_context",                           l_cairo_quartz_surface_get_cg_context},
#endif

    {NULL, NULL}
};

static int luaopen_lQuartzSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".QuartzSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, QuartzSurface_lib_m);
    luaL_register(L, LCAIRO ".QuartzSurface", QuartzSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_QuartzSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_QUARTZ_SURFACE */



//---------------------------------------------------------------------------
// Win32Surface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_WIN32_SURFACE

static int new_Win32Surface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.Win32Surface

    //FIXME
    //{"create_with_ddb",                          l_cairo_win32_surface_create_with_ddb},
    //{"create_with_dib",                          l_cairo_win32_surface_create_with_dib},

    //{"create",                                   l_cairo_win32_surface_create},
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_surface_t *cs = cairo_win32_surface_create(hdc);

    return new_Surface(L, LCAIRO ".Win32Surface.mt", cs, CAIRO_SURFACE_TYPE_WIN32, 1);
}

static const struct luaL_Reg Win32Surface_lib_f [] = {
    {"new", new_Win32Surface},
    {NULL, NULL}
};

static const struct luaL_Reg Win32Surface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_WIN32_SURFACE
  //{"create",                                   l_cairo_win32_surface_create},
  //{"create_with_ddb",                          l_cairo_win32_surface_create_with_ddb},
  //{"create_with_dib",                          l_cairo_win32_surface_create_with_dib},
    {"get_dc",                                   l_cairo_win32_surface_get_dc},
    {"get_image",                                l_cairo_win32_surface_get_image},
#endif

    {NULL, NULL}
};

static int luaopen_lWin32Surface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".Win32Surface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Win32Surface_lib_m);
    luaL_register(L, LCAIRO ".Win32Surface", Win32Surface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_Win32Surface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_WIN32_SURFACE */



//---------------------------------------------------------------------------
// BeosSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_BEOS_SURFACE

static int new_BeosSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.BeosSurface

    //FIXME
    //{"create_for_bitmap",                        l_cairo_beos_surface_create_for_bitmap},

    //{"create",                                   l_cairo_beos_surface_create},
    BView* view = (BView *) check_lightuserdata(L, 1);
    cairo_surface_t *cs = cairo_beos_surface_create (view);

    return new_Surface(L, LCAIRO ".BeosSurface.mt", cs, CAIRO_SURFACE_TYPE_BEOS, 1);
}

static const struct luaL_Reg BeosSurface_lib_f [] = {
    {"new", new_BeosSurface},
    {NULL, NULL}
};

static const struct luaL_Reg BeosSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_BEOS_SURFACE
  //{"create",                                   l_cairo_beos_surface_create},
  //{"create_for_bitmap",                        l_cairo_beos_surface_create_for_bitmap},
#endif

    {NULL, NULL}
};

static int luaopen_lBeosSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".BeosSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, BeosSurface_lib_m);
    luaL_register(L, LCAIRO ".BeosSurface", BeosSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_BeosSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_BEOS_SURFACE */



//---------------------------------------------------------------------------
// DirectFbSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_DIRECTFB_SURFACE

static int new_DirectFbSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.DirectFbSurface

    //{"create",                                   l_cairo_directfb_surface_create},
    IDirectFB *dfb = (IDirectFB *) check_lightuserdata(L, 1);
    IDirectFBSurface *surface = (IDirectFBSurface *) check_lightuserdata(L, 2);
    cairo_surface_t *cs = cairo_directfb_surface_create (dfb, surface);

    return new_Surface(L, LCAIRO ".DirectFbSurface.mt", cs, CAIRO_SURFACE_TYPE_DIRECTFB, 1);
}

static const struct luaL_Reg DirectFbSurface_lib_f [] = {
    {"new", new_DirectFbSurface},
    {NULL, NULL}
};

static const struct luaL_Reg DirectFbSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
  //{"create",                                   l_cairo_directfb_surface_create},
#endif

    {NULL, NULL}
};

static int luaopen_lDirectFbSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".DirectFbSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, DirectFbSurface_lib_m);
    luaL_register(L, LCAIRO ".DirectFbSurface", DirectFbSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_DirectFbSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_DIRECTFB_SURFACE */



//---------------------------------------------------------------------------
// SvgSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_SVG_SURFACE

static int new_SvgSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.SvgSurface

    //FIXME
    //{"create_for_stream",                        l_cairo_svg_surface_create_for_stream},

    //{"create",                                   l_cairo_svg_surface_create},
    const char *filename = luaL_checkstring(L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_surface_t *cs = cairo_svg_surface_create(filename, width_in_points, height_in_points);

    return new_Surface(L, LCAIRO ".SvgSurface.mt", cs, CAIRO_SURFACE_TYPE_SVG, 1);
}

static const struct luaL_Reg SvgSurface_lib_f [] = {
    {"new", new_SvgSurface},
    {NULL, NULL}
};

static const struct luaL_Reg SvgSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_SVG_SURFACE
  //{"create",                                   l_cairo_svg_surface_create},
  //{"create_for_stream",                        l_cairo_svg_surface_create_for_stream},
    {"restrict_to_version",                      l_cairo_svg_surface_restrict_to_version},
    {"get_versions",                             l_cairo_svg_get_versions},
    {"version_to_string",                        l_cairo_svg_version_to_string},
#endif

    {NULL, NULL}
};

static int luaopen_lSvgSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".SvgSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, SvgSurface_lib_m);
    luaL_register(L, LCAIRO ".SvgSurface", SvgSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_SvgSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_SVG_SURFACE */



//---------------------------------------------------------------------------
// Os2Surface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_OS2_SURFACE

static int new_Os2Surface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.Os2Surface

    //FIXME
    //{"create",                                   l_cairo_os2_surface_create},
    cairo_surface_t* cs = NULL;

    return new_Surface(L, LCAIRO ".Os2Surface.mt", cs, CAIRO_SURFACE_TYPE_OS2, 1);
}

static const struct luaL_Reg Os2Surface_lib_f [] = {
    {"new", new_Os2Surface},
    {NULL, NULL}
};

static const struct luaL_Reg Os2Surface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_OS2_SURFACE
  //{"os2_init",                                 l_cairo_os2_init},
  //{"os2_fini",                                 l_cairo_os2_fini},
  //{"create",                                   l_cairo_os2_surface_create},
    {"set_hwnd",                                 l_cairo_os2_surface_set_hwnd},
    {"set_size",                                 l_cairo_os2_surface_set_size},
    {"refresh_window",                           l_cairo_os2_surface_refresh_window},
    {"set_manual_window_refresh",                l_cairo_os2_surface_set_manual_window_refresh},
    {"get_manual_window_refresh",                l_cairo_os2_surface_get_manual_window_refresh},
#endif

    {NULL, NULL}
};

static int luaopen_lOs2Surface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".Os2Surface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Os2Surface_lib_m);
    luaL_register(L, LCAIRO ".Os2Surface", Os2Surface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_Os2Surface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_OS2_SURFACE */



//---------------------------------------------------------------------------
// Win32PrintingSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_WIN32_SURFACE
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)

static int new_Win32PrintingSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.Win32PrintingSurface

    //{"create",                                   l_cairo_win32_printing_surface_create},
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_surface_t *cs = cairo_win32_printing_surface_create(hdc);

    return new_Surface(L, LCAIRO ".Win32PrintingSurface.mt", cs, CAIRO_SURFACE_TYPE_WIN32_PRINTING, 1);
}

static const struct luaL_Reg Win32PrintingSurface_lib_f [] = {
    {"new", new_Win32PrintingSurface},
    {NULL, NULL}
};

static const struct luaL_Reg Win32PrintingSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

#if CAIRO_HAS_WIN32_SURFACE
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  //{"create",                                   l_cairo_win32_printing_surface_create},
#endif
#endif

    {NULL, NULL}
};

static int luaopen_lWin32PrintingSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".Win32PrintingSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Win32PrintingSurface_lib_m);
    luaL_register(L, LCAIRO ".Win32PrintingSurface", Win32PrintingSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_Win32PrintingSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0) */
#endif  /* CAIRO_HAS_WIN32_SURFACE */



//---------------------------------------------------------------------------
// QuartzImageSurface userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_QUARTZ_IMAGE_SURFACE

static int new_QuartzImageSurface (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.QuartzImageSurface

    //FIXME
    // create?
    cairo_surface_t* cs = NULL;

    return new_Surface(L, LCAIRO ".QuartzImageSurface.mt", cs, CAIRO_SURFACE_TYPE_QUARTZ_IMAGE, 1);
}

static const struct luaL_Reg QuartzImageSurface_lib_f [] = {
    {"new", new_QuartzImageSurface},
    {NULL, NULL}
};

static const struct luaL_Reg QuartzImageSurface_lib_m [] = {
    {"__gc",                                     gc_Surface},
    {"__tostring",                               tostring_Surface},

    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"reference",                                l_cairo_surface_reference},
    {"finish",                                   l_cairo_surface_finish},
    {"destroy",                                  gc_Surface},
    {"get_reference_count",                      l_cairo_surface_get_reference_count},
    {"status",                                   l_cairo_surface_status},
    {"get_type",                                 l_cairo_surface_get_type},
    {"get_content",                              l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"write_to_png",                             l_cairo_surface_write_to_png},
    {"write_to_png_stream",                      l_cairo_surface_write_to_png_stream},
#endif
    {"get_user_data",                            l_cairo_surface_get_user_data},
    {"set_user_data",                            l_cairo_surface_set_user_data},
    {"get_font_options",                         l_cairo_surface_get_font_options},
    {"flush",                                    l_cairo_surface_flush},
    {"mark_dirty",                               l_cairo_surface_mark_dirty},
    {"mark_dirty_rectangle",                     l_cairo_surface_mark_dirty_rectangle},
    {"set_device_offset",                        l_cairo_surface_set_device_offset},
    {"get_device_offset",                        l_cairo_surface_get_device_offset},
    {"set_fallback_resolution",                  l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_fallback_resolution",                  l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"copy_page",                                l_cairo_surface_copy_page},
    {"show_page",                                l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"has_show_text_glyphs",                     l_cairo_surface_has_show_text_glyphs},
#endif

    {NULL, NULL}
};

static int luaopen_lQuartzImageSurface (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".QuartzImageSurface.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, QuartzImageSurface_lib_m);
    luaL_register(L, LCAIRO ".QuartzImageSurface", QuartzImageSurface_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_QuartzImageSurface);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif /* CAIRO_HAS_QUARTZ_IMAGE_SURFACE */



//---------------------------------------------------------------------------
// Surface userdatas for oo usage
//---------------------------------------------------------------------------

static int luaopen_lsurface (lua_State *L)
{
//?? #if CAIRO_HAS_IMAGE_SURFACE
    luaopen_lImageSurface (L);
//?? #endif

#if CAIRO_HAS_PDF_SURFACE
    luaopen_lPdfSurface (L);
#endif

#if CAIRO_HAS_PS_SURFACE
    luaopen_lPsSurface (L);
#endif

#if CAIRO_HAS_XLIB_SURFACE
    luaopen_lXlibSurface (L);
#endif

#if CAIRO_HAS_XCB_SURFACE
    luaopen_lXcbSurface (L);
#endif

#if CAIRO_HAS_GLITZ_SURFACE
    luaopen_lGlitzSurface (L);
#endif

#if CAIRO_HAS_QUARTZ_SURFACE
    luaopen_lQuartzSurface (L);
#endif

#if CAIRO_HAS_WIN32_SURFACE
    luaopen_lWin32Surface (L);
#endif

#if CAIRO_HAS_BEOS_SURFACE
    luaopen_lBeosSurface (L);
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
    luaopen_lDirectFbSurface (L);
#endif

#if CAIRO_HAS_SVG_SURFACE
    luaopen_lSvgSurface (L);
#endif

#if CAIRO_HAS_OS2_SURFACE
    luaopen_lOs2Surface (L);
#endif

#if CAIRO_HAS_WIN32_SURFACE
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    luaopen_lWin32PrintingSurface (L);
#endif
#endif

#if CAIRO_HAS_QUARTZ_IMAGE_SURFACE
    luaopen_lQuartzImageSurface (L);
#endif

    return 0;
}

#endif  /* LUACAIRO_HAS_OO_INTERFACE */



///--------------------------------------------------------------------------
/// Pattern userdata for cairo_pattern_t
///--------------------------------------------------------------------------
#if LUACAIRO_HAS_OO_INTERFACE

typedef struct _Pattern
{
    cairo_pattern_t* cp_;
    int              havecp_;
    int              typecp_;
} Pattern;

static Pattern* check_Pattern_ud(lua_State *L, int idx)
{
    Pattern* cp = NULL;
    //if (!cp) cp = (Pattern*) get_userdata(L, idx, LCAIRO ".Pattern.mt");
    if (!cp) cp = (Pattern*) get_userdata(L, idx, LCAIRO ".SolidPattern.mt");
    if (!cp) cp = (Pattern*) get_userdata(L, idx, LCAIRO ".SurfacePattern.mt");
    //if (!cp) cp = (Pattern*) get_userdata(L, idx, LCAIRO ".Gradient.mt");
    if (!cp) cp = (Pattern*) get_userdata(L, idx, LCAIRO ".LinearGradient.mt");
    if (!cp) cp = (Pattern*) get_userdata(L, idx, LCAIRO ".RadialGradient.mt");
    if (!cp) luaL_typerror(L, idx, LCAIRO ".Pattern");
    return cp;
}

static int new_Pattern (lua_State *L, const char *tname, cairo_pattern_t* cp, int typecp, int havecp)
{
    Pattern *o = (Pattern *) lua_newuserdata(L, sizeof(Pattern));
    o->cp_     = cp;
    o->havecp_ = havecp;
    o->typecp_ = typecp;

    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);

    return 1;
}

static int new_PatternFromLUD (lua_State *L)
{
    cairo_pattern_t *cp = (cairo_pattern_t *) check_lightuserdata(L, 1);
    int havecp          = lua_toboolean(L, 2);
    int typecp          = cairo_pattern_get_type(cp);

    switch (typecp) {
    case CAIRO_PATTERN_TYPE_SOLID:   return new_Pattern(L, LCAIRO ".SolidPattern.mt", cp, typecp, havecp);
    case CAIRO_PATTERN_TYPE_SURFACE: return new_Pattern(L, LCAIRO ".SurfacePattern.mt", cp, typecp, havecp);
    case CAIRO_PATTERN_TYPE_LINEAR:  return new_Pattern(L, LCAIRO ".LinearGradient.mt", cp, typecp, havecp);
    case CAIRO_PATTERN_TYPE_RADIAL:  return new_Pattern(L, LCAIRO ".RadialGradient.mt", cp, typecp, havecp);
    }

    return 0;
}

static int gc_Pattern (lua_State *L)
{
    Pattern *o = check_Pattern_ud(L, 1);

    if (o->cp_ && o->havecp_)
    {
        cairo_pattern_destroy(o->cp_);
        o->cp_     = NULL;
        o->havecp_ = 0;
        o->typecp_ = 0;
    }

    return 0;
}

static int tostring_Pattern (lua_State *L)
{
    Pattern *o = check_Pattern_ud(L, 1);
    lua_pushfstring(L, "Pattern (%p), cairo_pattern_t (%p), Type:%d", (void*)o, (void*)o->cp_, o->typecp_);
    return 1;
}



//---------------------------------------------------------------------------
// SolidPattern userdata for oo usage
//---------------------------------------------------------------------------

static int new_SolidPattern (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.SolidPattern

    //{"create_rgb",                       l_cairo_pattern_create_rgb},
    //{"create_rgba",                      l_cairo_pattern_create_rgba},

    double red = luaL_checknumber(L, 1);
    double green = luaL_checknumber(L, 2);
    double blue = luaL_checknumber(L, 3);
    double alpha = luaL_optnumber(L, 4, -1);

    cairo_pattern_t* cp = NULL;

    if (alpha < 0)
        cp = cairo_pattern_create_rgb (red, green, blue);
    else
        cp = cairo_pattern_create_rgba (red, green, blue, alpha);

    return new_Pattern(L, LCAIRO ".SolidPattern.mt", cp, CAIRO_PATTERN_TYPE_SOLID, 1);
}

static const struct luaL_Reg SolidPattern_lib_f [] = {
    {"new", new_SolidPattern},
    {NULL, NULL}
};

static const struct luaL_Reg SolidPattern_lib_m [] = {
    {"__gc",                                     gc_Pattern},
    {"__tostring",                               tostring_Pattern},

  //{"create_rgb",                               l_cairo_pattern_create_rgb},
  //{"create_rgba",                              l_cairo_pattern_create_rgba},
  //{"create_for_surface",                       l_cairo_pattern_create_for_surface},
  //{"create_linear",                            l_cairo_pattern_create_linear},
  //{"create_radial",                            l_cairo_pattern_create_radial},
    {"reference",                                l_cairo_pattern_reference},
    {"destroy",                                  gc_Pattern},
    {"get_reference_count",                      l_cairo_pattern_get_reference_count},
    {"status",                                   l_cairo_pattern_status},
    {"get_user_data",                            l_cairo_pattern_get_user_data},
    {"set_user_data",                            l_cairo_pattern_set_user_data},
    {"get_type",                                 l_cairo_pattern_get_type},
  //{"add_color_stop_rgb",                       l_cairo_pattern_add_color_stop_rgb},
  //{"add_color_stop_rgba",                      l_cairo_pattern_add_color_stop_rgba},
    {"set_matrix",                               l_cairo_pattern_set_matrix},
    {"get_matrix",                               l_cairo_pattern_get_matrix},
  //{"set_extend",                               l_cairo_pattern_set_extend},
  //{"get_extend",                               l_cairo_pattern_get_extend},
  //{"set_filter",                               l_cairo_pattern_set_filter},
  //{"get_filter",                               l_cairo_pattern_get_filter},
    {"get_rgba",                                 l_cairo_pattern_get_rgba},
  //{"get_surface",                              l_cairo_pattern_get_surface},
  //{"get_color_stop_rgba",                      l_cairo_pattern_get_color_stop_rgba},
  //{"get_color_stop_count",                     l_cairo_pattern_get_color_stop_count},
  //{"get_linear_points",                        l_cairo_pattern_get_linear_points},
  //{"get_radial_circles",                       l_cairo_pattern_get_radial_circles},

    {NULL, NULL}
};


static int luaopen_lSolidPattern (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".SolidPattern.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, SolidPattern_lib_m);
    luaL_register(L, LCAIRO ".SolidPattern", SolidPattern_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_SolidPattern);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}



//---------------------------------------------------------------------------
// SurfacePattern userdata for oo usage
//---------------------------------------------------------------------------

static int new_SurfacePattern (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.SurfacePattern

    //{"create_for_surface",               l_cairo_pattern_create_for_surface},
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_pattern_t *cp = cairo_pattern_create_for_surface (surface);

    return new_Pattern(L, LCAIRO ".SurfacePattern.mt", cp, CAIRO_PATTERN_TYPE_SURFACE, 1);
}

static const struct luaL_Reg SurfacePattern_lib_f [] = {
    {"new", new_SurfacePattern},
    {NULL, NULL}
};

static const struct luaL_Reg SurfacePattern_lib_m [] = {
    {"__gc",                                     gc_Pattern},
    {"__tostring",                               tostring_Pattern},

  //{"create_rgb",                               l_cairo_pattern_create_rgb},
  //{"create_rgba",                              l_cairo_pattern_create_rgba},
  //{"create_for_surface",                       l_cairo_pattern_create_for_surface},
  //{"create_linear",                            l_cairo_pattern_create_linear},
  //{"create_radial",                            l_cairo_pattern_create_radial},
    {"reference",                                l_cairo_pattern_reference},
    {"destroy",                                  gc_Pattern},
    {"get_reference_count",                      l_cairo_pattern_get_reference_count},
    {"status",                                   l_cairo_pattern_status},
    {"get_user_data",                            l_cairo_pattern_get_user_data},
    {"set_user_data",                            l_cairo_pattern_set_user_data},
    {"get_type",                                 l_cairo_pattern_get_type},
  //{"add_color_stop_rgb",                       l_cairo_pattern_add_color_stop_rgb},
  //{"add_color_stop_rgba",                      l_cairo_pattern_add_color_stop_rgba},
    {"set_matrix",                               l_cairo_pattern_set_matrix},
    {"get_matrix",                               l_cairo_pattern_get_matrix},
    {"set_extend",                               l_cairo_pattern_set_extend},
    {"get_extend",                               l_cairo_pattern_get_extend},
    {"set_filter",                               l_cairo_pattern_set_filter},
    {"get_filter",                               l_cairo_pattern_get_filter},
  //{"get_rgba",                                 l_cairo_pattern_get_rgba},
    {"get_surface",                              l_cairo_pattern_get_surface},
  //{"get_color_stop_rgba",                      l_cairo_pattern_get_color_stop_rgba},
  //{"get_color_stop_count",                     l_cairo_pattern_get_color_stop_count},
  //{"get_linear_points",                        l_cairo_pattern_get_linear_points},
  //{"get_radial_circles",                       l_cairo_pattern_get_radial_circles},

    {NULL, NULL}
};


static int luaopen_lSurfacePattern (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".SurfacePattern.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, SurfacePattern_lib_m);
    luaL_register(L, LCAIRO ".SurfacePattern", SurfacePattern_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_SurfacePattern);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}



//---------------------------------------------------------------------------
// LinearGradient userdata for oo usage
//---------------------------------------------------------------------------

static int new_LinearGradient (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.LinearGradient

    //{"create_linear",                    l_cairo_pattern_create_linear},
    double x0 = luaL_checknumber(L, 1);
    double y0 = luaL_checknumber(L, 2);
    double x1 = luaL_checknumber(L, 3);
    double y1 = luaL_checknumber(L, 4);
    cairo_pattern_t *cp = cairo_pattern_create_linear(x0, y0, x1, y1);

    return new_Pattern(L, LCAIRO ".LinearGradient.mt", cp, CAIRO_PATTERN_TYPE_LINEAR, 1);
}

static const struct luaL_Reg LinearGradient_lib_f [] = {
    {"new", new_LinearGradient},
    {NULL, NULL}
};

static const struct luaL_Reg LinearGradient_lib_m [] = {
    {"__gc",                                     gc_Pattern},
    {"__tostring",                               tostring_Pattern},

  //{"create_rgb",                               l_cairo_pattern_create_rgb},
  //{"create_rgba",                              l_cairo_pattern_create_rgba},
  //{"create_for_surface",                       l_cairo_pattern_create_for_surface},
  //{"create_linear",                            l_cairo_pattern_create_linear},
  //{"create_radial",                            l_cairo_pattern_create_radial},
    {"reference",                                l_cairo_pattern_reference},
    {"destroy",                                  gc_Pattern},
    {"get_reference_count",                      l_cairo_pattern_get_reference_count},
    {"status",                                   l_cairo_pattern_status},
    {"get_user_data",                            l_cairo_pattern_get_user_data},
    {"set_user_data",                            l_cairo_pattern_set_user_data},
    {"get_type",                                 l_cairo_pattern_get_type},
    {"add_color_stop_rgb",                       l_cairo_pattern_add_color_stop_rgb},
    {"add_color_stop_rgba",                      l_cairo_pattern_add_color_stop_rgba},
    {"set_matrix",                               l_cairo_pattern_set_matrix},
    {"get_matrix",                               l_cairo_pattern_get_matrix},
  //{"set_extend",                               l_cairo_pattern_set_extend},
  //{"get_extend",                               l_cairo_pattern_get_extend},
  //{"set_filter",                               l_cairo_pattern_set_filter},
  //{"get_filter",                               l_cairo_pattern_get_filter},
  //{"get_rgba",                                 l_cairo_pattern_get_rgba},
  //{"get_surface",                              l_cairo_pattern_get_surface},
    {"get_color_stop_rgba",                      l_cairo_pattern_get_color_stop_rgba},
    {"get_color_stop_count",                     l_cairo_pattern_get_color_stop_count},
    {"get_linear_points",                        l_cairo_pattern_get_linear_points},
  //{"get_radial_circles",                       l_cairo_pattern_get_radial_circles},

    {NULL, NULL}
};


static int luaopen_lLinearGradient (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".LinearGradient.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, LinearGradient_lib_m);
    luaL_register(L, LCAIRO ".LinearGradient", LinearGradient_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_LinearGradient);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}



//---------------------------------------------------------------------------
// RadialGradient userdata for oo usage
//---------------------------------------------------------------------------

static int new_RadialGradient (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.RadialGradient

    //{"create_radial",                    l_cairo_pattern_create_radial},
    double cx0 = luaL_checknumber(L, 1);
    double cy0 = luaL_checknumber(L, 2);
    double radius0 = luaL_checknumber(L, 3);
    double cx1 = luaL_checknumber(L, 4);
    double cy1 = luaL_checknumber(L, 5);
    double radius1 = luaL_checknumber(L, 6);
    cairo_pattern_t *cp = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);

    return new_Pattern(L, LCAIRO ".RadialGradient.mt", cp, CAIRO_PATTERN_TYPE_RADIAL, 1);
}

static const struct luaL_Reg RadialGradient_lib_f [] = {
    {"new", new_RadialGradient},
    {NULL, NULL}
};

static const struct luaL_Reg RadialGradient_lib_m [] = {
    {"__gc",                                     gc_Pattern},
    {"__tostring",                               tostring_Pattern},

  //{"create_rgb",                               l_cairo_pattern_create_rgb},
  //{"create_rgba",                              l_cairo_pattern_create_rgba},
  //{"create_for_surface",                       l_cairo_pattern_create_for_surface},
  //{"create_linear",                            l_cairo_pattern_create_linear},
  //{"create_radial",                            l_cairo_pattern_create_radial},
    {"reference",                                l_cairo_pattern_reference},
    {"destroy",                                  gc_Pattern},
    {"get_reference_count",                      l_cairo_pattern_get_reference_count},
    {"status",                                   l_cairo_pattern_status},
    {"get_user_data",                            l_cairo_pattern_get_user_data},
    {"set_user_data",                            l_cairo_pattern_set_user_data},
    {"get_type",                                 l_cairo_pattern_get_type},
    {"add_color_stop_rgb",                       l_cairo_pattern_add_color_stop_rgb},
    {"add_color_stop_rgba",                      l_cairo_pattern_add_color_stop_rgba},
    {"set_matrix",                               l_cairo_pattern_set_matrix},
    {"get_matrix",                               l_cairo_pattern_get_matrix},
  //{"set_extend",                               l_cairo_pattern_set_extend},
  //{"get_extend",                               l_cairo_pattern_get_extend},
  //{"set_filter",                               l_cairo_pattern_set_filter},
  //{"get_filter",                               l_cairo_pattern_get_filter},
  //{"get_rgba",                                 l_cairo_pattern_get_rgba},
  //{"get_surface",                              l_cairo_pattern_get_surface},
    {"get_color_stop_rgba",                      l_cairo_pattern_get_color_stop_rgba},
    {"get_color_stop_count",                     l_cairo_pattern_get_color_stop_count},
  //{"get_linear_points",                        l_cairo_pattern_get_linear_points},
    {"get_radial_circles",                       l_cairo_pattern_get_radial_circles},

    {NULL, NULL}
};


static int luaopen_lRadialGradient (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".RadialGradient.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, RadialGradient_lib_m);
    luaL_register(L, LCAIRO ".RadialGradient", RadialGradient_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_RadialGradient);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}



//---------------------------------------------------------------------------
// Pattern userdatas for oo usage
//---------------------------------------------------------------------------

static int luaopen_lpattern (lua_State *L)
{
    luaopen_lSolidPattern (L);
    luaopen_lSurfacePattern (L);
    luaopen_lLinearGradient (L);
    luaopen_lRadialGradient (L);

    return 0;
}

#endif  /* LUACAIRO_HAS_OO_INTERFACE */




///--------------------------------------------------------------------------
/// FontOptions userdata for cairo_font_options_t
///--------------------------------------------------------------------------
#if LUACAIRO_HAS_OO_INTERFACE

typedef struct _FontOptions
{
    cairo_font_options_t* fo_;
    int                   havefo_;
} FontOptions;

#define check_FontOptions_ud(L, idx) \
    (FontOptions *) luaL_checkudata(L, idx, LCAIRO ".FontOptions.mt")

static int new_FontOptions (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.FontOptions

  //{"create",                                   l_cairo_font_options_create},
    cairo_font_options_t *fo = cairo_font_options_create();

    FontOptions *o = (FontOptions *) lua_newuserdata(L, sizeof(FontOptions));
    o->fo_     = fo;
    o->havefo_ = 1;

    luaL_getmetatable(L, LCAIRO ".FontOptions.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int new_FontOptionsFromLUD (lua_State *L)
{
    cairo_font_options_t *fo = (cairo_font_options_t *) check_lightuserdata(L, 1);
    int havefo  = lua_toboolean(L, 2);

    FontOptions *o = (FontOptions *) lua_newuserdata(L, sizeof(FontOptions));
    o->fo_     = fo;
    o->havefo_ = havefo;

    luaL_getmetatable(L, LCAIRO ".FontOptions.mt");
    lua_setmetatable(L, -2);

    return 1;
}

static int gc_FontOptions (lua_State *L)
{
    FontOptions *o = check_FontOptions_ud(L, 1);

    if (o->fo_ && o->havefo_)
    {
        cairo_font_options_destroy (o->fo_);
        o->fo_     = NULL;
        o->havefo_ = 0;
    }

    return 0;
}

static int tostring_FontOptions (lua_State *L)
{
    FontOptions *o = check_FontOptions_ud(L, 1);
    lua_pushfstring(L, "FontOptions (%p), cairo_font_options_t (%p)", (void*)o, (void*)o->fo_);
    return 1;
}

static const struct luaL_Reg FontOptions_lib_f [] = {
    {"new", new_FontOptions},
    {NULL, NULL}
};

static const struct luaL_Reg FontOptions_lib_m [] = {
    {"__gc",                                     gc_FontOptions},
    {"__tostring",                               tostring_FontOptions},

  //{"create",                                   l_cairo_font_options_create},
    {"copy",                                     l_cairo_font_options_copy},
    {"destroy",                                  gc_FontOptions},
    {"status",                                   l_cairo_font_options_status},
    {"merge",                                    l_cairo_font_options_merge},
    {"equal",                                    l_cairo_font_options_equal},
    {"hash",                                     l_cairo_font_options_hash},
    {"set_antialias",                            l_cairo_font_options_set_antialias},
    {"get_antialias",                            l_cairo_font_options_get_antialias},
    {"set_subpixel_order",                       l_cairo_font_options_set_subpixel_order},
    {"get_subpixel_order",                       l_cairo_font_options_get_subpixel_order},
    {"set_hint_style",                           l_cairo_font_options_set_hint_style},
    {"get_hint_style",                           l_cairo_font_options_get_hint_style},
    {"set_hint_metrics",                         l_cairo_font_options_set_hint_metrics},
    {"get_hint_metrics",                         l_cairo_font_options_get_hint_metrics},

    {NULL, NULL}
};


static int luaopen_lfontoptions (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".FontOptions.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, FontOptions_lib_m);
    luaL_register(L, LCAIRO ".FontOptions", FontOptions_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_FontOptions);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);

    lua_pop(L, 1);
    return 0;
}

#endif  /* LUACAIRO_HAS_OO_INTERFACE */




///--------------------------------------------------------------------------
/// ScaledFont userdata for cairo_scaled_font_t
///--------------------------------------------------------------------------
#if LUACAIRO_HAS_OO_INTERFACE

typedef struct _ScaledFont
{
    cairo_scaled_font_t* sf_;
    int                  havesf_;
    int                  typesf_; //0:ScaledFont, 1:Win32ScaledFont, 2:FtScaledFont
} ScaledFont;


static ScaledFont* check_ScaledFont_ud(lua_State *L, int idx)
{
    ScaledFont* sf = NULL;

    if (!sf) sf = (ScaledFont*) get_userdata(L, idx, LCAIRO ".ScaledFont.mt");
#if CAIRO_HAS_WIN32_FONT
    if (!sf) sf = (ScaledFont*) get_userdata(L, idx, LCAIRO ".Win32ScaledFont.mt");
#endif
#if CAIRO_HAS_FT_FONT
    if (!sf) sf = (ScaledFont*) get_userdata(L, idx, LCAIRO ".FtScaledFont.mt");
#endif
    if (!sf) luaL_typerror(L, idx, LCAIRO ".ScaledFont");

    return sf;
}

static int new_ScaledFont (lua_State *L, const char *tname, cairo_scaled_font_t* sf, int typesf, int havesf)
{
    ScaledFont *o = (ScaledFont *) lua_newuserdata(L, sizeof(ScaledFont));
    o->sf_     = sf;
    o->havesf_ = havesf;
    o->typesf_ = typesf;

    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);

    return 1;
}

static int new_ScaledFontFromLUD (lua_State *L)
{
    cairo_scaled_font_t *sf = (cairo_scaled_font_t *) check_lightuserdata(L, 1);
    int havesf              = lua_toboolean(L, 2);
    int typesf              = 0; //FIXME assume ScaledFont

    switch (typesf) {
    case 0: return new_ScaledFont(L, LCAIRO ".ScaledFont.mt", sf, typesf, havesf);
#if CAIRO_HAS_WIN32_FONT
    case 1: return new_ScaledFont(L, LCAIRO ".Win32ScaledFont.mt", sf, typesf, havesf);
#endif
#if CAIRO_HAS_FT_FONT
    case 2: return new_ScaledFont(L, LCAIRO ".FtScaledFont.mt", sf, typesf, havesf);
#endif
    }

    return 0;
}

static int gc_ScaledFont (lua_State *L)
{
    ScaledFont *o = check_ScaledFont_ud(L, 1);

    if (o->sf_ && o->havesf_)
    {
        cairo_scaled_font_destroy(o->sf_);
        o->sf_     = NULL;
        o->havesf_ = 0;
        o->typesf_ = 0;
    }

    return 0;
}

static int tostring_ScaledFont (lua_State *L)
{
    ScaledFont *o = check_ScaledFont_ud(L, 1);
    lua_pushfstring(L, "ScaledFont (%p), cairo_scaled_font_t (%p), Type:%d", (void*)o, (void*)o->sf_, o->typesf_);
    return 1;
}




//---------------------------------------------------------------------------
// ScaledFont userdata for oo usage
//---------------------------------------------------------------------------

static int new_ScaledFont0 (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.ScaledFont

    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    const cairo_matrix_t *font_matrix = get_cairo_matrix_t (L, 2);
    const cairo_matrix_t *ctm = get_cairo_matrix_t (L, 3);
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 4);
    cairo_scaled_font_t *sf = cairo_scaled_font_create (font_face, font_matrix, ctm, options);

    return new_ScaledFont(L, LCAIRO ".ScaledFont.mt", sf, 0, 1);
}

static const struct luaL_Reg ScaledFont0_lib_f [] = {
    {"new", new_ScaledFont0},
    {NULL, NULL}
};

static const struct luaL_Reg ScaledFont0_lib_m [] = {
    {"__gc",                                     gc_ScaledFont},
    {"__tostring",                               tostring_ScaledFont},

  //{"create",                                   l_cairo_scaled_font_create},
    {"reference",                                l_cairo_scaled_font_reference},
    {"destroy",                                  gc_ScaledFont},
    {"get_reference_count",                      l_cairo_scaled_font_get_reference_count},
    {"status",                                   l_cairo_scaled_font_status},
    {"get_type",                                 l_cairo_scaled_font_get_type},
    {"get_user_data",                            l_cairo_scaled_font_get_user_data},
    {"set_user_data",                            l_cairo_scaled_font_set_user_data},
    {"extents",                                  l_cairo_scaled_font_extents},
    {"text_extents",                             l_cairo_scaled_font_text_extents},
    {"glyph_extents",                            l_cairo_scaled_font_glyph_extents},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"text_to_glyphs",                           l_cairo_scaled_font_text_to_glyphs},
#endif
    {"get_font_face",                            l_cairo_scaled_font_get_font_face},
    {"get_font_matrix",                          l_cairo_scaled_font_get_font_matrix},
    {"get_ctm",                                  l_cairo_scaled_font_get_ctm},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_scale_matrix",                         l_cairo_scaled_font_get_scale_matrix},
#endif
    {"get_font_options",                         l_cairo_scaled_font_get_font_options},

    {NULL, NULL}
};

static int luaopen_lScaledFont0 (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".ScaledFont.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, ScaledFont0_lib_m);
    luaL_register(L, LCAIRO ".ScaledFont", ScaledFont0_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_ScaledFont0);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}




//---------------------------------------------------------------------------
// Win32ScaledFont userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_WIN32_FONT

static int new_Win32ScaledFont (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.Win32ScaledFont

    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    const cairo_matrix_t *font_matrix = get_cairo_matrix_t (L, 2);
    const cairo_matrix_t *ctm = get_cairo_matrix_t (L, 3);
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 4);
    cairo_scaled_font_t *sf = cairo_scaled_font_create (font_face, font_matrix, ctm, options);

    return new_ScaledFont(L, LCAIRO ".Win32ScaledFont.mt", sf, 1, 1);
}

static const struct luaL_Reg Win32ScaledFont_lib_f [] = {
    {"new", new_Win32ScaledFont},
    {NULL, NULL}
};

static const struct luaL_Reg Win32ScaledFont_lib_m [] = {
    {"__gc",                                     gc_ScaledFont},
    {"__tostring",                               tostring_ScaledFont},

  //{"create",                                   l_cairo_scaled_font_create},
    {"reference",                                l_cairo_scaled_font_reference},
    {"destroy",                                  gc_ScaledFont},
    {"get_reference_count",                      l_cairo_scaled_font_get_reference_count},
    {"status",                                   l_cairo_scaled_font_status},
    {"get_type",                                 l_cairo_scaled_font_get_type},
    {"get_user_data",                            l_cairo_scaled_font_get_user_data},
    {"set_user_data",                            l_cairo_scaled_font_set_user_data},
    {"extents",                                  l_cairo_scaled_font_extents},
    {"text_extents",                             l_cairo_scaled_font_text_extents},
    {"glyph_extents",                            l_cairo_scaled_font_glyph_extents},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"text_to_glyphs",                           l_cairo_scaled_font_text_to_glyphs},
#endif
    {"get_font_face",                            l_cairo_scaled_font_get_font_face},
    {"get_font_matrix",                          l_cairo_scaled_font_get_font_matrix},
    {"get_ctm",                                  l_cairo_scaled_font_get_ctm},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_scale_matrix",                         l_cairo_scaled_font_get_scale_matrix},
#endif
    {"get_font_options",                         l_cairo_scaled_font_get_font_options},
#if CAIRO_HAS_WIN32_FONT
    {"select_font",                              l_cairo_win32_scaled_font_select_font},
    {"done_font",                                l_cairo_win32_scaled_font_done_font},
    {"get_metrics_factor",                       l_cairo_win32_scaled_font_get_metrics_factor},
    {"get_logical_to_device",                    l_cairo_win32_scaled_font_get_logical_to_device},
    {"get_device_to_logical",                    l_cairo_win32_scaled_font_get_device_to_logical},
#endif

    {NULL, NULL}
};

static int luaopen_lWin32ScaledFont (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".Win32ScaledFont.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Win32ScaledFont_lib_m);
    luaL_register(L, LCAIRO ".Win32ScaledFont", Win32ScaledFont_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_Win32ScaledFont);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_WIN32_FONT */




//---------------------------------------------------------------------------
// FtScaledFont userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_FT_FONT

static int new_FtScaledFont (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.FtScaledFont

    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    const cairo_matrix_t *font_matrix = get_cairo_matrix_t (L, 2);
    const cairo_matrix_t *ctm = get_cairo_matrix_t (L, 3);
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 4);
    cairo_scaled_font_t *sf = cairo_scaled_font_create (font_face, font_matrix, ctm, options);

    return new_ScaledFont(L, LCAIRO ".FtScaledFont.mt", sf, 2, 1);
}

static const struct luaL_Reg FtScaledFont_lib_f [] = {
    {"new", new_FtScaledFont},
    {NULL, NULL}
};

static const struct luaL_Reg FtScaledFont_lib_m [] = {
    {"__gc",                                     gc_ScaledFont},
    {"__tostring",                               tostring_ScaledFont},

  //{"create",                                   l_cairo_scaled_font_create},
    {"reference",                                l_cairo_scaled_font_reference},
    {"destroy",                                  gc_ScaledFont},
    {"get_reference_count",                      l_cairo_scaled_font_get_reference_count},
    {"status",                                   l_cairo_scaled_font_status},
    {"get_type",                                 l_cairo_scaled_font_get_type},
    {"get_user_data",                            l_cairo_scaled_font_get_user_data},
    {"set_user_data",                            l_cairo_scaled_font_set_user_data},
    {"extents",                                  l_cairo_scaled_font_extents},
    {"text_extents",                             l_cairo_scaled_font_text_extents},
    {"glyph_extents",                            l_cairo_scaled_font_glyph_extents},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"text_to_glyphs",                           l_cairo_scaled_font_text_to_glyphs},
#endif
    {"get_font_face",                            l_cairo_scaled_font_get_font_face},
    {"get_font_matrix",                          l_cairo_scaled_font_get_font_matrix},
    {"get_ctm",                                  l_cairo_scaled_font_get_ctm},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"get_scale_matrix",                         l_cairo_scaled_font_get_scale_matrix},
#endif
    {"get_font_options",                         l_cairo_scaled_font_get_font_options},

#if CAIRO_HAS_FT_FONT
    {"lock_face",                                l_cairo_ft_scaled_font_lock_face},
    {"unlock_face",                              l_cairo_ft_scaled_font_unlock_face},
#endif

    {NULL, NULL}
};

static int luaopen_lFtScaledFont (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".FtScaledFont.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, FtScaledFont_lib_m);
    luaL_register(L, LCAIRO ".FtScaledFont", FtScaledFont_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_FtScaledFont);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_FT_FONT */


static int luaopen_lscaledfont (lua_State *L)
{
    luaopen_lScaledFont0 (L);
#if CAIRO_HAS_WIN32_FONT
    luaopen_lWin32ScaledFont (L);
#endif
#if CAIRO_HAS_FT_FONT
    luaopen_lFtScaledFont (L);
#endif
    return 0;
}

#endif  /* LUACAIRO_HAS_OO_INTERFACE */




///--------------------------------------------------------------------------
/// FontFace userdata for cairo_font_face_t
///--------------------------------------------------------------------------
#if LUACAIRO_HAS_OO_INTERFACE

typedef struct _FontFace
{
    cairo_font_face_t* ff_;
    int                haveff_;
    int                typeff_;
} FontFace;

static FontFace* check_FontFace_ud(lua_State *L, int idx)
{
    FontFace* ff = NULL;

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    if (!ff) ff = (FontFace*) get_userdata(L, idx, LCAIRO ".ToyFontFace.mt");
#endif
#if CAIRO_HAS_FT_FONT
    if (!ff) ff = (FontFace*) get_userdata(L, idx, LCAIRO ".FtFontFace.mt");
#endif
#if CAIRO_HAS_WIN32_FONT
    if (!ff) ff = (FontFace*) get_userdata(L, idx, LCAIRO ".Win32FontFace.mt");
#endif
#ifdef CAIRO_HAS_QUARTZ_FONT
    if (!ff) ff = (FontFace*) get_userdata(L, idx, LCAIRO ".QuartzFontFace.mt");
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    if (!ff) ff = (FontFace*) get_userdata(L, idx, LCAIRO ".UserFontFace.mt");
#endif
    if (!ff) luaL_typerror(L, idx, LCAIRO ".FontFace");

    return ff;
}

static int new_FontFace (lua_State *L, const char *tname, cairo_font_face_t* ff, int typeff, int haveff)
{
    FontFace *o = (FontFace *) lua_newuserdata(L, sizeof(FontFace));
    o->ff_     = ff;
    o->haveff_ = haveff;
    o->typeff_ = typeff;

    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);

    return 1;
}

static int new_FontFaceFromLUD (lua_State *L)
{
    cairo_font_face_t *ff = (cairo_font_face_t *) check_lightuserdata(L, 1);
    int haveff            = lua_toboolean(L, 2);
    int typeff            = cairo_font_face_get_type (ff);

    switch (typeff) {
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    case CAIRO_FONT_TYPE_TOY:    return new_FontFace(L, LCAIRO ".ToyFontFace.mt", ff, typeff, haveff);
#endif
#if CAIRO_HAS_FT_FONT
    case CAIRO_FONT_TYPE_FT:     return new_FontFace(L, LCAIRO ".FtFontFace.mt", ff, typeff, haveff);
#endif
#if CAIRO_HAS_WIN32_FONT
    case CAIRO_FONT_TYPE_WIN32:  return new_FontFace(L, LCAIRO ".Win32FontFace.mt", ff, typeff, haveff);
#endif
#ifdef CAIRO_HAS_QUARTZ_FONT
    case CAIRO_FONT_TYPE_QUARTZ: return new_FontFace(L, LCAIRO ".QuartzFontFace.mt", ff, typeff, haveff);
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    case CAIRO_FONT_TYPE_USER:   return new_FontFace(L, LCAIRO ".UserFontFace.mt", ff, typeff, haveff);
#endif
    }

    return 0;
}

static int gc_FontFace (lua_State *L)
{
    FontFace *o = check_FontFace_ud(L, 1);

    if (o->ff_ && o->haveff_)
    {
        cairo_font_face_destroy (o->ff_);
        o->ff_     = NULL;
        o->haveff_ = 0;
        o->typeff_ = 0;
    }

    return 0;
}

static int tostring_FontFace (lua_State *L)
{
    FontFace *o = check_FontFace_ud(L, 1);
    lua_pushfstring(L, "FontFace (%p), cairo_font_face_t (%p), Type:%d", (void*)o, (void*)o->ff_, o->typeff_);
    return 1;
}




//---------------------------------------------------------------------------
// ToyFontFace userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)

static int new_ToyFontFace (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.ToyFontFace

    const char *family = luaL_checkstring(L, 1);
    cairo_font_slant_t slant = (cairo_font_slant_t) luaL_checkinteger(L, 2);
    cairo_font_weight_t weight = (cairo_font_weight_t) luaL_checkinteger(L, 3);
    cairo_font_face_t *ff = cairo_toy_font_face_create (family, slant, weight);

    return new_FontFace(L, LCAIRO ".ToyFontFace.mt", ff, CAIRO_FONT_TYPE_TOY, 1);
}

static const struct luaL_Reg ToyFontFace_lib_f [] = {
    {"new", new_ToyFontFace},
    {NULL, NULL}
};

static const struct luaL_Reg ToyFontFace_lib_m [] = {
    {"__gc",                                     gc_FontFace},
    {"__tostring",                               tostring_FontFace},

    {"reference",                                l_cairo_font_face_reference},
    {"destroy",                                  gc_FontFace},
    {"get_reference_count",                      l_cairo_font_face_get_reference_count},
    {"status",                                   l_cairo_font_face_status},
    {"get_type",                                 l_cairo_font_face_get_type},
    {"get_user_data",                            l_cairo_font_face_get_user_data},
    {"set_user_data",                            l_cairo_font_face_set_user_data},

  //{"create",                                   l_cairo_toy_font_face_create},
    {"get_family",                               l_cairo_toy_font_face_get_family},
    {"get_slant",                                l_cairo_toy_font_face_get_slant},
    {"get_weight",                               l_cairo_toy_font_face_get_weight},

    {NULL, NULL}
};

static int luaopen_lToyFontFace (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".ToyFontFace.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, ToyFontFace_lib_m);
    luaL_register(L, LCAIRO ".ToyFontFace", ToyFontFace_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_ToyFontFace);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0) */




//---------------------------------------------------------------------------
// FtFontFace userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_FT_FONT

static int new_FtFontFace (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.FtFontFace

    //FIXME
    //{"ft_font_face_create_for_pattern",          l_cairo_ft_font_face_create_for_pattern},
    //{"ft_font_face_create_for_ft_face",          l_cairo_ft_font_face_create_for_ft_face},
    cairo_font_face_t *ff = NULL;

    return new_FontFace(L, LCAIRO ".FtFontFace.mt", ff, CAIRO_FONT_TYPE_FT, 1);
}

static const struct luaL_Reg FtFontFace_lib_f [] = {
    {"new", new_FtFontFace},
    {NULL, NULL}
};

static const struct luaL_Reg FtFontFace_lib_m [] = {
    {"__gc",                                     gc_FontFace},
    {"__tostring",                               tostring_FontFace},

    {"reference",                                l_cairo_font_face_reference},
    {"destroy",                                  gc_FontFace},
    {"get_reference_count",                      l_cairo_font_face_get_reference_count},
    {"status",                                   l_cairo_font_face_status},
    {"get_type",                                 l_cairo_font_face_get_type},
    {"get_user_data",                            l_cairo_font_face_get_user_data},
    {"set_user_data",                            l_cairo_font_face_set_user_data},

  //{"create_for_pattern",                       l_cairo_ft_font_face_create_for_pattern},
  //{"ft_font_options_substitute",               l_cairo_ft_font_options_substitute},
  //{"create_for_ft_face",                       l_cairo_ft_font_face_create_for_ft_face},

    {NULL, NULL}
};

static int luaopen_lFtFontFace (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".FtFontFace.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, FtFontFace_lib_m);
    luaL_register(L, LCAIRO ".FtFontFace", FtFontFace_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_FtFontFace);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_FT_FONT */




//---------------------------------------------------------------------------
// Win32FontFace userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_HAS_WIN32_FONT

static int new_Win32FontFace (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.Win32FontFace

    //FIXME
    //{"win32_font_face_create_for_logfontw",      l_cairo_win32_font_face_create_for_logfontw},
    //{"win32_font_face_create_for_hfont",         l_cairo_win32_font_face_create_for_hfont},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    //{"win32_font_face_create_for_logfontw_hfont",l_cairo_win32_font_face_create_for_logfontw_hfont},
#endif

    LOGFONTW *logfont = (LOGFONTW *) check_lightuserdata(L, 1);
    cairo_font_face_t *ff = cairo_win32_font_face_create_for_logfontw (logfont);

    return new_FontFace(L, LCAIRO ".Win32FontFace.mt", ff, CAIRO_FONT_TYPE_WIN32, 1);
}

static const struct luaL_Reg Win32FontFace_lib_f [] = {
    {"new", new_Win32FontFace},
    {NULL, NULL}
};

static const struct luaL_Reg Win32FontFace_lib_m [] = {
    {"__gc",                                     gc_FontFace},
    {"__tostring",                               tostring_FontFace},

    {"reference",                                l_cairo_font_face_reference},
    {"destroy",                                  gc_FontFace},
    {"get_reference_count",                      l_cairo_font_face_get_reference_count},
    {"status",                                   l_cairo_font_face_status},
    {"get_type",                                 l_cairo_font_face_get_type},
    {"get_user_data",                            l_cairo_font_face_get_user_data},
    {"set_user_data",                            l_cairo_font_face_set_user_data},

  //{"create_for_logfontw",                      l_cairo_win32_font_face_create_for_logfontw},
  //{"create_for_hfont",                         l_cairo_win32_font_face_create_for_hfont},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  //{"create_for_logfontw_hfont",                l_cairo_win32_font_face_create_for_logfontw_hfont},
#endif

    {NULL, NULL}
};

static int luaopen_lWin32FontFace (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".Win32FontFace.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Win32FontFace_lib_m);
    luaL_register(L, LCAIRO ".Win32FontFace", Win32FontFace_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_Win32FontFace);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_WIN32_FONT */




//---------------------------------------------------------------------------
// QuartzFontFace userdata for oo usage
//---------------------------------------------------------------------------
#ifdef CAIRO_HAS_QUARTZ_FONT

static int new_QuartzFontFace (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.QuartzFontFace

    //FIXME
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    //{"quartz_font_face_create_for_cgfont",       l_cairo_quartz_font_face_create_for_cgfont},
    //{"quartz_font_face_create_for_atsu_font_id", l_cairo_quartz_font_face_create_for_atsu_font_id},
#endif

    cairo_font_face_t *ff = NULL;

    return new_FontFace(L, LCAIRO ".QuartzFontFace.mt", ff, CAIRO_FONT_TYPE_QUARTZ, 1);
}

static const struct luaL_Reg QuartzFontFace_lib_f [] = {
    {"new", new_QuartzFontFace},
    {NULL, NULL}
};

static const struct luaL_Reg QuartzFontFace_lib_m [] = {
    {"__gc",                                     gc_FontFace},
    {"__tostring",                               tostring_FontFace},

    {"reference",                                l_cairo_font_face_reference},
    {"destroy",                                  gc_FontFace},
    {"get_reference_count",                      l_cairo_font_face_get_reference_count},
    {"status",                                   l_cairo_font_face_status},
    {"get_type",                                 l_cairo_font_face_get_type},
    {"get_user_data",                            l_cairo_font_face_get_user_data},
    {"set_user_data",                            l_cairo_font_face_set_user_data},

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  //{"create_for_cgfont",                        l_cairo_quartz_font_face_create_for_cgfont},
  //{"create_for_atsu_font_id",                  l_cairo_quartz_font_face_create_for_atsu_font_id},
#endif

    {NULL, NULL}
};

static int luaopen_lQuartzFontFace (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".QuartzFontFace.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, QuartzFontFace_lib_m);
    luaL_register(L, LCAIRO ".QuartzFontFace", QuartzFontFace_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_QuartzFontFace);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_HAS_QUARTZ_FONT */




//---------------------------------------------------------------------------
// UserFontFace userdata for oo usage
//---------------------------------------------------------------------------
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)

static int new_UserFontFace (lua_State *L)
{
    lua_remove(L, 1); // remove cairo.UserFontFace

    cairo_font_face_t *ff = cairo_user_font_face_create ();

    return new_FontFace(L, LCAIRO ".UserFontFace.mt", ff, CAIRO_FONT_TYPE_USER, 1);
}

static const struct luaL_Reg UserFontFace_lib_f [] = {
    {"new", new_UserFontFace},
    {NULL, NULL}
};

static const struct luaL_Reg UserFontFace_lib_m [] = {
    {"__gc",                                     gc_FontFace},
    {"__tostring",                               tostring_FontFace},

    {"reference",                                l_cairo_font_face_reference},
    {"destroy",                                  gc_FontFace},
    {"get_reference_count",                      l_cairo_font_face_get_reference_count},
    {"status",                                   l_cairo_font_face_status},
    {"get_type",                                 l_cairo_font_face_get_type},
    {"get_user_data",                            l_cairo_font_face_get_user_data},
    {"set_user_data",                            l_cairo_font_face_set_user_data},

  //{"create",                                   l_cairo_user_font_face_create},
    {"set_init_func",                            l_cairo_user_font_face_set_init_func},
    {"set_render_glyph_func",                    l_cairo_user_font_face_set_render_glyph_func},
    {"set_text_to_glyphs_func",                  l_cairo_user_font_face_set_text_to_glyphs_func},
    {"set_unicode_to_glyph_func",                l_cairo_user_font_face_set_unicode_to_glyph_func},
    {"get_init_func",                            l_cairo_user_font_face_get_init_func},
    {"get_render_glyph_func",                    l_cairo_user_font_face_get_render_glyph_func},
    {"get_text_to_glyphs_func",                  l_cairo_user_font_face_get_text_to_glyphs_func},
    {"get_unicode_to_glyph_func",                l_cairo_user_font_face_get_unicode_to_glyph_func},

    {NULL, NULL}
};

static int luaopen_lUserFontFace (lua_State *L)
{
    luaL_newmetatable(L, LCAIRO ".UserFontFace.mt");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, UserFontFace_lib_m);
    luaL_register(L, LCAIRO ".UserFontFace", UserFontFace_lib_f);
    lua_newtable(L);
    lua_pushstring(L, "__call");
    lua_pushcfunction(L, new_UserFontFace);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
    lua_pop(L, 1);
    return 0;
}

#endif  /* CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0) */


static int luaopen_lfontface (lua_State *L)
{
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    luaopen_lToyFontFace (L);
#endif
#if CAIRO_HAS_FT_FONT
    luaopen_lFtFontFace (L);
#endif
#if CAIRO_HAS_WIN32_FONT
    luaopen_lWin32FontFace (L);
#endif
#if CAIRO_HAS_QUARTZ_FONT
    luaopen_lQuartzFontFace (L);
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    luaopen_lUserFontFace (L);
#endif
    return 0;
}

#endif  /* LUACAIRO_HAS_OO_INTERFACE */




///--------------------------------------------------------------------------
/// Getters
///--------------------------------------------------------------------------

static cairo_matrix_t* get_cairo_matrix_t (lua_State *L, int idx)
{
    return check_Matrix_ud(L, idx);
}

static cairo_text_extents_t* get_cairo_text_extents_t (lua_State *L, int idx)
{
    return check_TextExtents_ud(L, idx);
}

static cairo_font_extents_t* get_cairo_font_extents_t (lua_State *L, int idx)
{
    return check_FontExtents_ud(L, idx);
}

static cairo_t* get_cairo_t (lua_State *L, int idx)
{
    int t = lua_type(L, idx);

    if (t == LUA_TLIGHTUSERDATA)
    {
        return (cairo_t *) check_lightuserdata(L, idx);
    }
#if LUACAIRO_HAS_OO_INTERFACE
    else if (t == LUA_TUSERDATA)
    {
        Context *o = check_Context_ud(L, idx);
        return o->cr_;

    }
#endif
    luaL_error(L, "cairo_t expected!");
    return NULL;
}

static cairo_surface_t* get_cairo_surface_t (lua_State *L, int idx)
{
    int t = lua_type(L, idx);

    if (t == LUA_TLIGHTUSERDATA)
    {
        return (cairo_surface_t *) check_lightuserdata(L, idx);
    }
#if LUACAIRO_HAS_OO_INTERFACE
    else if (t == LUA_TUSERDATA)
    {
        Surface *o = check_Surface_ud(L, idx);
        return o->cs_;

    }
#endif
    luaL_error(L, "cairo_surface_t expected!");
    return NULL;
}

static cairo_pattern_t* get_cairo_pattern_t (lua_State *L, int idx)
{
    int t = lua_type(L, idx);

    if (t == LUA_TLIGHTUSERDATA)
    {
        return (cairo_pattern_t *) check_lightuserdata(L, idx);
    }
#if LUACAIRO_HAS_OO_INTERFACE
    else if (t == LUA_TUSERDATA)
    {
        Pattern *o = check_Pattern_ud(L, idx);
        return o->cp_;

    }
#endif
    luaL_error(L, "cairo_pattern_t expected!");
    return NULL;
}

static cairo_font_options_t* get_cairo_font_options_t (lua_State *L, int idx)
{
    int t = lua_type(L, idx);

    if (t == LUA_TLIGHTUSERDATA)
    {
        return (cairo_font_options_t *) check_lightuserdata(L, idx);
    }
#if LUACAIRO_HAS_OO_INTERFACE
    else if (t == LUA_TUSERDATA)
    {
        FontOptions *o = check_FontOptions_ud(L, idx);
        return o->fo_;

    }
#endif
    luaL_error(L, "cairo_font_options_t expected!");
    return NULL;
}

static cairo_scaled_font_t* get_cairo_scaled_font_t (lua_State *L, int idx)
{
    int t = lua_type(L, idx);

    if (t == LUA_TLIGHTUSERDATA)
    {
        return (cairo_scaled_font_t *) check_lightuserdata(L, idx);
    }
#if LUACAIRO_HAS_OO_INTERFACE
    else if (t == LUA_TUSERDATA)
    {
        ScaledFont *o = check_ScaledFont_ud(L, idx);
        return o->sf_;

    }
#endif
    luaL_error(L, "cairo_scaled_font_t expected!");
    return NULL;
}

static cairo_font_face_t* get_cairo_font_face_t (lua_State *L, int idx)
{
    int t = lua_type(L, idx);

    if (t == LUA_TLIGHTUSERDATA)
    {
        return (cairo_font_face_t *) check_lightuserdata(L, idx);
    }
#if LUACAIRO_HAS_OO_INTERFACE
    else if (t == LUA_TUSERDATA)
    {
        FontFace *o = check_FontFace_ud (L, idx);
        return o->ff_;

    }
#endif
    luaL_error(L, "cairo_font_face_t expected!");
    return NULL;
}



//TODO
static cairo_path_t* get_cairo_path_t (lua_State *L, int idx)
{
    return (cairo_path_t *) check_lightuserdata(L, idx);
}




// should be unnecessary when all classes implemented
static void remove_Context(lua_State* L, int idx)
{
#if LUACAIRO_HAS_OO_INTERFACE
    if (get_userdata(L, idx, LCAIRO ".Context.mt"))
    {
        lua_remove(L, idx);
    }
#endif
}




///--------------------------------------------------------------------------
/// Cairo API
///--------------------------------------------------------------------------


// cairo_public int
// cairo_version (void);
static int l_cairo_version(lua_State* L)
{
    int v = cairo_version();
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public const char*
// cairo_version_string (void);
static int l_cairo_version_string(lua_State* L)
{
    const char* v = cairo_version_string();
    lua_pushstring(L, v);
    return 1;
}


/* Functions for manipulating state objects */

// cairo_public cairo_t *
// cairo_create (cairo_surface_t *target);
static int l_cairo_create(lua_State* L)
{
    cairo_surface_t *target = get_cairo_surface_t (L, 1);
    cairo_t *v = cairo_create(target);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public cairo_t *
// cairo_reference (cairo_t *cr);
static int l_cairo_reference(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_t *v = cairo_reference(cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public void
// cairo_destroy (cairo_t *cr);
static int l_cairo_destroy(lua_State* L)
{
    cairo_t *cr = (cairo_t *) check_lightuserdata(L, 1);
    cairo_destroy(cr);
    return 0;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public unsigned int
// cairo_get_reference_count (cairo_t *cr);
static int l_cairo_get_reference_count(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    unsigned int v = cairo_get_reference_count(cr);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public void *
// cairo_get_user_data (cairo_t *cr,
//              const cairo_user_data_key_t *key);
static int l_cairo_get_user_data(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_status_t
// cairo_set_user_data (cairo_t *cr,
//              const cairo_user_data_key_t *key,
//              void *user_data,
//              cairo_destroy_func_t destroy);
static int l_cairo_set_user_data(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_save (cairo_t *cr);
static int l_cairo_save(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_save(cr);
    return 0;
}


// cairo_public void
// cairo_restore (cairo_t *cr);
static int l_cairo_restore(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_restore(cr);
    return 0;
}


// cairo_public void
// cairo_push_group (cairo_t *cr);
static int l_cairo_push_group(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_push_group(cr);
    return 0;
}


// cairo_public void
// cairo_push_group_with_content (cairo_t *cr,
//          cairo_content_t content);
static int l_cairo_push_group_with_content(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_content_t content = (cairo_content_t) luaL_checkinteger(L, 2);
    cairo_push_group_with_content(cr, content);
    return 0;
}


// cairo_public cairo_pattern_t *
// cairo_pop_group (cairo_t *cr);
static int l_cairo_pop_group(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_pattern_t *v = cairo_pop_group(cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_pop_group_to_source (cairo_t *cr);
static int l_cairo_pop_group_to_source(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_pop_group_to_source(cr);
    return 0;
}


/* Modify state  */

// cairo_public void
// cairo_set_operator (cairo_t *cr,
//              cairo_operator_t op);
static int l_cairo_set_operator(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_operator_t op = (cairo_operator_t) luaL_checkinteger(L, 2);
    cairo_set_operator(cr, op);
    return 0;
}


// cairo_public void
// cairo_set_source (cairo_t *cr,
//              cairo_pattern_t *source);
static int l_cairo_set_source(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_pattern_t *source = get_cairo_pattern_t(L, 2);
    cairo_set_source(cr, source);
    return 0;
}


// cairo_public void
// cairo_set_source_rgb (cairo_t *cr,
//              double red, double green, double blue);
static int l_cairo_set_source_rgb(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double red = luaL_checknumber(L, 2);
    double green = luaL_checknumber(L, 3);
    double blue = luaL_checknumber(L, 4);
    cairo_set_source_rgb(cr, red, green, blue);
    return 0;
}


// cairo_public void
// cairo_set_source_rgba (cairo_t *cr,
//                double red, double green, double blue,
//                double alpha);
static int l_cairo_set_source_rgba(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double red = luaL_checknumber(L, 2);
    double green = luaL_checknumber(L, 3);
    double blue = luaL_checknumber(L, 4);
    double alpha = luaL_checknumber(L, 5);
    cairo_set_source_rgba(cr, red, green, blue, alpha);
    return 0;
}


// cairo_public void
// cairo_set_source_surface (cairo_t *cr,
//               cairo_surface_t *surface,
//               double x,
//               double y);
static int l_cairo_set_source_surface(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_surface_t *surface = get_cairo_surface_t (L, 2);
    double x = luaL_checknumber(L, 3);
    double y = luaL_checknumber(L, 4);
    cairo_set_source_surface(cr, surface, x, y);
    return 0;
}


// cairo_public void
// cairo_set_tolerance (cairo_t *cr,
//              double tolerance);
static int l_cairo_set_tolerance(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double tolerance = luaL_checknumber(L, 2);
    cairo_set_tolerance(cr, tolerance);
    return 0;
}


// cairo_public void
// cairo_set_antialias (cairo_t *cr,
//              cairo_antialias_t antialias);
static int l_cairo_set_antialias(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_antialias_t antialias = (cairo_antialias_t) luaL_checkinteger(L, 2);
    cairo_set_antialias(cr, antialias);
    return 0;
}


// cairo_public void
// cairo_set_fill_rule (cairo_t *cr,
//              cairo_fill_rule_t fill_rule);
static int l_cairo_set_fill_rule(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_fill_rule_t fill_rule = (cairo_fill_rule_t) luaL_checkinteger(L, 2);
    cairo_set_fill_rule(cr, fill_rule);
    return 0;
}


// cairo_public void
// cairo_set_line_width (cairo_t *cr,
//              double width);
static int l_cairo_set_line_width(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double width = luaL_checknumber(L, 2);
    cairo_set_line_width(cr, width);
    return 0;
}


// cairo_public void
// cairo_set_line_cap (cairo_t *cr,
//              cairo_line_cap_t line_cap);
static int l_cairo_set_line_cap(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_line_cap_t line_cap = (cairo_line_cap_t) luaL_checkinteger(L, 2);
    cairo_set_line_cap(cr, line_cap);
    return 0;
}


// cairo_public void
// cairo_set_line_join (cairo_t *cr,
//              cairo_line_join_t line_join);
static int l_cairo_set_line_join(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_line_join_t line_join = (cairo_line_join_t) luaL_checkinteger(L, 2);
    cairo_set_line_join(cr, line_join);
    return 0;
}


// cairo_public void
// cairo_set_dash (cairo_t *cr,
//         const double *dashes,
//         int num_dashes,
//         double offset);
static int l_cairo_set_dash(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);

    switch (lua_type(L, 2)) {
    case LUA_TNUMBER: {
            double dash = luaL_checknumber(L, 2);
            int	num_dashes = 1; //luaL_checkinteger(L, 3);
            double offset = luaL_checknumber(L, 4);
            cairo_set_dash(cr, &dash, num_dashes, offset);
        }
        break;
    case LUA_TTABLE: { // Borrowed from Chris Osgood's binding
            int num_dashes = (int)lua_objlen(L, 2);

            double *dashes = malloc(num_dashes * sizeof(double));
            if (dashes == NULL) luaL_error(L, "memory error");

            int i;
            for (i = 0; i < num_dashes; i++) {
                lua_rawgeti(L, 2, i+1);
                dashes[i] = lua_tonumber(L, -1);
                lua_pop(L, 1);
            }

            double offset = luaL_checknumber(L, 4);
            cairo_set_dash(cr, dashes, num_dashes, offset);
            free(dashes);
        }
        break;
    default:
        luaL_error(L, "expected a number or number array(table)!");
    }
    return 0;
}


// cairo_public void
// cairo_set_miter_limit (cairo_t *cr,
//              double limit);
static int l_cairo_set_miter_limit(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double limit = luaL_checknumber(L, 2);
    cairo_set_miter_limit(cr, limit);
    return 0;
}


// cairo_public void
// cairo_translate (cairo_t *cr,
//              double tx, double ty);
static int l_cairo_translate(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double tx = luaL_checknumber(L, 2);
    double ty = luaL_checknumber(L, 3);
    cairo_translate(cr, tx, ty);
    return 0;
}


// cairo_public void
// cairo_scale (cairo_t *cr,
//              double sx, double sy);
static int l_cairo_scale(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double sx = luaL_checknumber(L, 2);
    double sy = luaL_checknumber(L, 3);
    cairo_scale(cr, sx, sy);
    return 0;
}


// cairo_public void
// cairo_rotate (cairo_t *cr,
//              double angle);
static int l_cairo_rotate(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double angle = luaL_checknumber(L, 2);
    cairo_rotate(cr, angle);
    return 0;
}


// cairo_public void
// cairo_transform (cairo_t *cr,
//          const cairo_matrix_t *matrix);
static int l_cairo_transform(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_transform (cr, matrix);
    return 0;
}


// cairo_public void
// cairo_set_matrix (cairo_t *cr,
//           const cairo_matrix_t *matrix);
static int l_cairo_set_matrix(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_set_matrix (cr, matrix);
    return 0;
}


// cairo_public void
// cairo_identity_matrix (cairo_t *cr);
static int l_cairo_identity_matrix(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_identity_matrix(cr);
    return 0;
}


// cairo_public void
// cairo_user_to_device (cairo_t *cr,
//              double *x, double *y);
static int l_cairo_user_to_device(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    cairo_user_to_device(cr, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}


// cairo_public void
// cairo_user_to_device_distance (cairo_t *cr,
//              double *dx, double *dy);
static int l_cairo_user_to_device_distance(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double dx = luaL_checknumber(L, 2);
    double dy = luaL_checknumber(L, 3);
    cairo_user_to_device_distance(cr, &dx, &dy);
    lua_pushnumber(L, dx);
    lua_pushnumber(L, dy);
    return 2;
}


// cairo_public void
// cairo_device_to_user (cairo_t *cr,
//              double *x, double *y);
static int l_cairo_device_to_user(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    cairo_device_to_user(cr, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}


// cairo_public void
// cairo_device_to_user_distance (cairo_t *cr,
//              double *dx, double *dy);
static int l_cairo_device_to_user_distance(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double dx = luaL_checknumber(L, 2);
    double dy = luaL_checknumber(L, 3);
    cairo_device_to_user_distance(cr, &dx, &dy);
    lua_pushnumber(L, dx);
    lua_pushnumber(L, dy);
    return 2;
}


/* Path creation functions */

// cairo_public void
// cairo_new_path (cairo_t *cr);
static int l_cairo_new_path(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_new_path(cr);
    return 0;
}


// cairo_public void
// cairo_move_to (cairo_t *cr,
//              double x, double y);
static int l_cairo_move_to(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    cairo_move_to(cr, x, y);
    return 0;
}


// cairo_public void
// cairo_new_sub_path (cairo_t *cr);
static int l_cairo_new_sub_path(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_new_sub_path(cr);
    return 0;
}


// cairo_public void
// cairo_line_to (cairo_t *cr,
//              double x, double y);
static int l_cairo_line_to(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    cairo_line_to(cr, x, y);
    return 0;
}


// cairo_public void
// cairo_curve_to (cairo_t *cr,
//         double x1, double y1,
//         double x2, double y2,
//         double x3, double y3);
static int l_cairo_curve_to(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double x1 = luaL_checknumber(L, 2);
    double y1 = luaL_checknumber(L, 3);
    double x2 = luaL_checknumber(L, 4);
    double y2 = luaL_checknumber(L, 5);
    double x3 = luaL_checknumber(L, 6);
    double y3 = luaL_checknumber(L, 7);
    cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);
    return 0;
}


// cairo_public void
// cairo_arc (cairo_t *cr,
//        double xc, double yc,
//        double radius,
//        double angle1, double angle2);
static int l_cairo_arc(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double xc = luaL_checknumber(L, 2);
    double yc = luaL_checknumber(L, 3);
    double radius = luaL_checknumber(L, 4);
    double angle1 = luaL_checknumber(L, 5);
    double angle2 = luaL_checknumber(L, 6);
    cairo_arc(cr, xc, yc, radius, angle1, angle2);
    return 0;
}


// cairo_public void
// cairo_arc_negative (cairo_t *cr,
//             double xc, double yc,
//             double radius,
//             double angle1, double angle2);
static int l_cairo_arc_negative(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double xc = luaL_checknumber(L, 2);
    double yc = luaL_checknumber(L, 3);
    double radius = luaL_checknumber(L, 4);
    double angle1 = luaL_checknumber(L, 5);
    double angle2 = luaL_checknumber(L, 6);
    cairo_arc_negative(cr, xc, yc, radius, angle1, angle2);
    return 0;
}


// cairo_public void
// cairo_rel_move_to (cairo_t *cr,
//              double dx, double dy);
static int l_cairo_rel_move_to(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double dx = luaL_checknumber(L, 2);
    double dy = luaL_checknumber(L, 3);
    cairo_rel_move_to(cr, dx, dy);
    return 0;
}


// cairo_public void
// cairo_rel_line_to (cairo_t *cr,
//              double dx, double dy);
static int l_cairo_rel_line_to(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double dx = luaL_checknumber(L, 2);
    double dy = luaL_checknumber(L, 3);
    cairo_rel_line_to(cr, dx, dy);
    return 0;
}


// cairo_public void
// cairo_rel_curve_to (cairo_t *cr,
//             double dx1, double dy1,
//             double dx2, double dy2,
//             double dx3, double dy3);
static int l_cairo_rel_curve_to(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double dx1 = luaL_checknumber(L, 2);
    double dy1 = luaL_checknumber(L, 3);
    double dx2 = luaL_checknumber(L, 4);
    double dy2 = luaL_checknumber(L, 5);
    double dx3 = luaL_checknumber(L, 6);
    double dy3 = luaL_checknumber(L, 7);
    cairo_rel_curve_to(cr, dx1, dy1, dx2, dy2, dx3, dy3);
    return 0;
}


// cairo_public void
// cairo_rectangle (cairo_t *cr,
//          double x, double y,
//          double width, double height);
static int l_cairo_rectangle(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    double width = luaL_checknumber(L, 4);
    double height = luaL_checknumber(L, 5);
    cairo_rectangle(cr, x, y, width, height);
    return 0;
}


// cairo_public void
// cairo_close_path (cairo_t *cr);
static int l_cairo_close_path(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_close_path(cr);
    return 0;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public void
// cairo_path_extents (cairo_t *cr,
//             double *x1, double *y1,
//             double *x2, double *y2);
static int l_cairo_path_extents(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x1 = luaL_optnumber(L, 2, 0.0);
    double y1 = luaL_optnumber(L, 3, 0.0);
    double x2 = luaL_optnumber(L, 4, 0.0);
    double y2 = luaL_optnumber(L, 5, 0.0);
    cairo_path_extents (cr, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}
#endif


/* Painting functions */

// cairo_public void
// cairo_paint (cairo_t *cr);
static int l_cairo_paint(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_paint(cr);
    return 0;
}


// cairo_public void
// cairo_paint_with_alpha (cairo_t *cr,
//             double   alpha);
static int l_cairo_paint_with_alpha(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double alpha = luaL_checknumber(L, 2);
    cairo_paint_with_alpha(cr, alpha);
    return 0;
}


// cairo_public void
// cairo_mask (cairo_t *cr,
//         cairo_pattern_t *pattern);
static int l_cairo_mask(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_pattern_t *pattern = get_cairo_pattern_t (L, 2);
    cairo_mask(cr, pattern);
    return 0;
}


// cairo_public void
// cairo_mask_surface (cairo_t *cr,
//             cairo_surface_t *surface,
//             double           surface_x,
//             double           surface_y);
static int l_cairo_mask_surface(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_surface_t *surface = get_cairo_surface_t (L, 2);
    double surface_x = luaL_checknumber(L, 3);
    double surface_y = luaL_checknumber(L, 4);
    cairo_mask_surface(cr, surface, surface_x, surface_y);
    return 0;
}


// cairo_public void
// cairo_stroke (cairo_t *cr);
static int l_cairo_stroke(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_stroke(cr);
    return 0;
}


// cairo_public void
// cairo_stroke_preserve (cairo_t *cr);
static int l_cairo_stroke_preserve(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_stroke_preserve(cr);
    return 0;
}


// cairo_public void
// cairo_fill (cairo_t *cr);
static int l_cairo_fill(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_fill(cr);
    return 0;
}


// cairo_public void
// cairo_fill_preserve (cairo_t *cr);
static int l_cairo_fill_preserve(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_fill_preserve(cr);
    return 0;
}


// cairo_public void
// cairo_copy_page (cairo_t *cr);
static int l_cairo_copy_page(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_copy_page(cr);
    return 0;
}


// cairo_public void
// cairo_show_page (cairo_t *cr);
static int l_cairo_show_page(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_show_page(cr);
    return 0;
}


/* Insideness testing */

// cairo_public cairo_bool_t
// cairo_in_stroke (cairo_t *cr,
//              double x, double y);
static int l_cairo_in_stroke(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    cairo_bool_t v = cairo_in_stroke(cr, x, y);
    lua_pushboolean(L, v);
    return 1;
}


// cairo_public cairo_bool_t
// cairo_in_fill (cairo_t *cr,
//              double x, double y);
static int l_cairo_in_fill(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_checknumber(L, 2);
    double y = luaL_checknumber(L, 3);
    cairo_bool_t v = cairo_in_fill(cr, x, y);
    lua_pushboolean(L, v);
    return 1;
}


/* Rectangular extents */

// cairo_public void
// cairo_stroke_extents (cairo_t *cr,
//               double *x1, double *y1,
//               double *x2, double *y2);
static int l_cairo_stroke_extents(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x1 = luaL_optnumber(L, 2, 0.0);
    double y1 = luaL_optnumber(L, 3, 0.0);
    double x2 = luaL_optnumber(L, 4, 0.0);
    double y2 = luaL_optnumber(L, 5, 0.0);
    cairo_stroke_extents(cr, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}


// cairo_public void
// cairo_fill_extents (cairo_t *cr,
//             double *x1, double *y1,
//             double *x2, double *y2);
static int l_cairo_fill_extents(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x1 = luaL_optnumber(L, 2, 0.0);
    double y1 = luaL_optnumber(L, 3, 0.0);
    double x2 = luaL_optnumber(L, 4, 0.0);
    double y2 = luaL_optnumber(L, 5, 0.0);
    cairo_fill_extents(cr, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}


/* Clipping */

// cairo_public void
// cairo_reset_clip (cairo_t *cr);
static int l_cairo_reset_clip(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_reset_clip(cr);
    return 0;
}


// cairo_public void
// cairo_clip (cairo_t *cr);
static int l_cairo_clip(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_clip(cr);
    return 0;
}


// cairo_public void
// cairo_clip_preserve (cairo_t *cr);
static int l_cairo_clip_preserve(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_clip_preserve(cr);
    return 0;
}


// cairo_public void
// cairo_clip_extents (cairo_t *cr,
//             double *x1, double *y1,
//             double *x2, double *y2);
static int l_cairo_clip_extents(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x1 = luaL_optnumber(L, 2, 0.0);
    double y1 = luaL_optnumber(L, 3, 0.0);
    double x2 = luaL_optnumber(L, 4, 0.0);
    double y2 = luaL_optnumber(L, 5, 0.0);
    cairo_clip_extents(cr, &x1, &y1, &x2, &y2);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, x2);
    lua_pushnumber(L, y2);
    return 4;
}


// cairo_public cairo_rectangle_list_t *
// cairo_copy_clip_rectangle_list (cairo_t *cr);
static int l_cairo_copy_clip_rectangle_list(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_rectangle_list_t *v = cairo_copy_clip_rectangle_list(cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_rectangle_list_destroy (cairo_rectangle_list_t *rectangle_list);
static int l_cairo_rectangle_list_destroy(lua_State* L)
{
    cairo_rectangle_list_t *rectangle_list = (cairo_rectangle_list_t *) check_lightuserdata(L, 1);
    cairo_rectangle_list_destroy(rectangle_list);
    return 0;
}


/* Font/Text functions */

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_glyph_t *
// cairo_glyph_allocate (int num_glyphs);
static int l_cairo_glyph_allocate(lua_State* L)
{
    //remove_Context(L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_glyph_free (cairo_glyph_t *glyphs);
static int l_cairo_glyph_free(lua_State* L)
{
    //remove_Context(L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_text_cluster_t *
// cairo_text_cluster_allocate (int num_clusters);
static int l_cairo_text_cluster_allocate(lua_State* L)
{
    //remove_Context(L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_text_cluster_free (cairo_text_cluster_t *clusters);
static int l_cairo_text_cluster_free(lua_State* L)
{
    //remove_Context(L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


// cairo_public cairo_font_options_t *
// cairo_font_options_create (void);
static int l_cairo_font_options_create(lua_State* L)
{
    cairo_font_options_t *v = cairo_font_options_create();
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_font_options_t *
// cairo_font_options_copy (const cairo_font_options_t *original);
static int l_cairo_font_options_copy(lua_State* L)
{
    const cairo_font_options_t *original = get_cairo_font_options_t (L, 1);
    cairo_font_options_t *v = cairo_font_options_copy (original);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_font_options_destroy (cairo_font_options_t *options);
static int l_cairo_font_options_destroy(lua_State* L)
{
    cairo_font_options_t *options = (cairo_font_options_t *) check_lightuserdata(L, 1);
    cairo_font_options_destroy(options);
    return 0;
}


// cairo_public cairo_status_t
// cairo_font_options_status (cairo_font_options_t *options);
static int l_cairo_font_options_status(lua_State* L)
{
    cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_status_t v = cairo_font_options_status(options);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_font_options_merge (cairo_font_options_t *options,
//           const cairo_font_options_t *other);
static int l_cairo_font_options_merge(lua_State* L)
{
    cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    const cairo_font_options_t *other = get_cairo_font_options_t (L, 2);
    cairo_font_options_merge(options, other);
    return 0;
}


// cairo_public cairo_bool_t
// cairo_font_options_equal (const cairo_font_options_t *options,
//           const cairo_font_options_t *other);
static int l_cairo_font_options_equal(lua_State* L)
{
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    const cairo_font_options_t *other = get_cairo_font_options_t (L, 2);
    cairo_bool_t v = cairo_font_options_equal(options, other);
    lua_pushboolean(L, v);
    return 1;
}


// cairo_public unsigned long
// cairo_font_options_hash (const cairo_font_options_t *options);
static int l_cairo_font_options_hash(lua_State* L)
{
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    unsigned long v = cairo_font_options_hash(options);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public void
// cairo_font_options_set_antialias (cairo_font_options_t *options,
//                   cairo_antialias_t     antialias);
static int l_cairo_font_options_set_antialias(lua_State* L)
{
    cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_antialias_t antialias = (cairo_antialias_t) luaL_checkinteger(L, 2);
    cairo_font_options_set_antialias(options, antialias);
    return 0;
}


// cairo_public cairo_antialias_t
// cairo_font_options_get_antialias (const cairo_font_options_t *options);
static int l_cairo_font_options_get_antialias(lua_State* L)
{
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_antialias_t v = cairo_font_options_get_antialias(options);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_font_options_set_subpixel_order (cairo_font_options_t *options,
//                        cairo_subpixel_order_t  subpixel_order);
static int l_cairo_font_options_set_subpixel_order(lua_State* L)
{
    cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_subpixel_order_t subpixel_order = (cairo_subpixel_order_t) luaL_checkinteger(L, 2);
    cairo_font_options_set_subpixel_order(options, subpixel_order);
    return 0;
}


// cairo_public cairo_subpixel_order_t
// cairo_font_options_get_subpixel_order (const cairo_font_options_t *options);
static int l_cairo_font_options_get_subpixel_order(lua_State* L)
{
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_subpixel_order_t v = cairo_font_options_get_subpixel_order(options);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_font_options_set_hint_style (cairo_font_options_t *options,
//                    cairo_hint_style_t     hint_style);
static int l_cairo_font_options_set_hint_style(lua_State* L)
{
    cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_hint_style_t hint_style = (cairo_hint_style_t) luaL_checkinteger(L, 2);
    cairo_font_options_set_hint_style(options, hint_style);
    return 0;
}


// cairo_public cairo_hint_style_t
// cairo_font_options_get_hint_style (const cairo_font_options_t *options);
static int l_cairo_font_options_get_hint_style(lua_State* L)
{
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_hint_style_t v = cairo_font_options_get_hint_style(options);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_font_options_set_hint_metrics (cairo_font_options_t *options,
//                      cairo_hint_metrics_t  hint_metrics);
static int l_cairo_font_options_set_hint_metrics(lua_State* L)
{
    cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_hint_metrics_t hint_metrics = (cairo_hint_metrics_t) luaL_checkinteger(L, 2);
    cairo_font_options_set_hint_metrics(options, hint_metrics);
    return 0;
}


// cairo_public cairo_hint_metrics_t
// cairo_font_options_get_hint_metrics (const cairo_font_options_t *options);
static int l_cairo_font_options_get_hint_metrics(lua_State* L)
{
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    cairo_hint_metrics_t v = cairo_font_options_get_hint_metrics(options);
    lua_pushinteger(L, v);
    return 1;
}


/* This interface is for dealing with text as text, not caring about the
   font object inside the the cairo_t. */

// cairo_public void
// cairo_select_font_face (cairo_t *cr,
//             const char           *family,
//             cairo_font_slant_t   slant,
//             cairo_font_weight_t  weight);
static int l_cairo_select_font_face(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const char *family = luaL_checkstring(L, 2);
    cairo_font_slant_t slant = (cairo_font_slant_t) luaL_checkinteger(L, 3);
    cairo_font_weight_t weight = (cairo_font_weight_t) luaL_checkinteger(L, 4);
    cairo_select_font_face(cr, family, slant, weight);
    return 0;
}


// cairo_public void
// cairo_set_font_size (cairo_t *cr, double size);
static int l_cairo_set_font_size(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double size = luaL_checknumber(L, 2);
    cairo_set_font_size(cr, size);
    return 0;
}


// cairo_public void
// cairo_set_font_matrix (cairo_t *cr,
//                const cairo_matrix_t *matrix);
static int l_cairo_set_font_matrix(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_set_font_matrix (cr, matrix);
    return 0;
}


// cairo_public void
// cairo_get_font_matrix (cairo_t *cr,
//                cairo_matrix_t *matrix);
static int l_cairo_get_font_matrix(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_get_font_matrix (cr, matrix);
    return 0;
}


// cairo_public void
// cairo_set_font_options (cairo_t *cr,
//             const cairo_font_options_t *options);
static int l_cairo_set_font_options(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 2);
    cairo_set_font_options(cr, options);
    return 0;
}


// cairo_public void
// cairo_get_font_options (cairo_t *cr,
//             cairo_font_options_t *options);
static int l_cairo_get_font_options(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_font_options_t *options = get_cairo_font_options_t (L, 2);
    cairo_get_font_options (cr, options);
    return 0;
}


// cairo_public void
// cairo_set_font_face (cairo_t *cr,
//              cairo_font_face_t *font_face);
static int l_cairo_set_font_face(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 2);
    cairo_set_font_face (cr, font_face);
    return 0;
}


// cairo_public cairo_font_face_t *
// cairo_get_font_face (cairo_t *cr);
static int l_cairo_get_font_face(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_font_face_t *v = cairo_get_font_face (cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_set_scaled_font (cairo_t *cr,
//                const cairo_scaled_font_t *scaled_font);
static int l_cairo_set_scaled_font(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 2);
    cairo_set_scaled_font (cr, scaled_font);
    return 0;
}


// cairo_public cairo_scaled_font_t *
// cairo_get_scaled_font (cairo_t *cr);
static int l_cairo_get_scaled_font(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_scaled_font_t *v = cairo_get_scaled_font (cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_show_text (cairo_t *cr,
//              const char *utf8);
static int l_cairo_show_text(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const char *utf8 = luaL_checkstring(L, 2);
    cairo_show_text (cr, utf8);
    return 0;
}


// cairo_public void
// cairo_show_glyphs (cairo_t *cr,
//              const cairo_glyph_t *glyphs,
//              int num_glyphs);
static int l_cairo_show_glyphs(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_show_text_glyphs (cairo_t *cr,
//             const char *utf8,
//             int utf8_len,
//             const cairo_glyph_t *glyphs,
//             int num_glyphs,
//             const cairo_text_cluster_t *clusters,
//             int num_clusters,
//             cairo_text_cluster_flags_t cluster_flags);
static int l_cairo_show_text_glyphs(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


// cairo_public void
// cairo_text_path  (cairo_t *cr,
//              const char *utf8);
static int l_cairo_text_path(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const char *utf8 = luaL_checkstring(L, 2);
    cairo_text_path (cr, utf8);
    return 0;
}


// cairo_public void
// cairo_glyph_path (cairo_t *cr,
//              const cairo_glyph_t *glyphs,
//              int num_glyphs);
static int l_cairo_glyph_path(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_text_extents (cairo_t *cr,
//             const char *utf8,
//             cairo_text_extents_t *extents);
static int l_cairo_text_extents(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const char *utf8 = luaL_checkstring(L, 2);
    cairo_text_extents_t *extents = get_cairo_text_extents_t (L, 3);
    cairo_text_extents (cr, utf8, extents);
    return 0;
}


// cairo_public void
// cairo_glyph_extents (cairo_t *cr,
//              const cairo_glyph_t *glyphs,
//              int num_glyphs,
//              cairo_text_extents_t *extents);
static int l_cairo_glyph_extents(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_font_extents (cairo_t *cr,
//             cairo_font_extents_t *extents);
static int l_cairo_font_extents(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_font_extents_t *extents = get_cairo_font_extents_t (L, 2);
    cairo_font_extents (cr, extents);
    return 0;
}


/* Generic identifier for a font style */

//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public cairo_font_face_t *
// cairo_font_face_reference (cairo_font_face_t *font_face);
static int l_cairo_font_face_reference(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    cairo_font_face_t *v = cairo_font_face_reference (font_face);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public void
// cairo_font_face_destroy (cairo_font_face_t *font_face);
static int l_cairo_font_face_destroy(lua_State* L)
{
    cairo_font_face_t *font_face = (cairo_font_face_t *) check_lightuserdata(L, 1);
    cairo_font_face_destroy (font_face);
    return 0;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public unsigned int
// cairo_font_face_get_reference_count (cairo_font_face_t *font_face);
static int l_cairo_font_face_get_reference_count(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    unsigned int v = cairo_font_face_get_reference_count (font_face);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public cairo_status_t
// cairo_font_face_status (cairo_font_face_t *font_face);
static int l_cairo_font_face_status(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    cairo_status_t v = cairo_font_face_status (font_face);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_font_type_t
// cairo_font_face_get_type (cairo_font_face_t *font_face);
static int l_cairo_font_face_get_type(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    cairo_font_type_t v = cairo_font_face_get_type (font_face);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void *
// cairo_font_face_get_user_data (cairo_font_face_t *font_face,
//                    const cairo_user_data_key_t *key);
static int l_cairo_font_face_get_user_data(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_status_t
// cairo_font_face_set_user_data (cairo_font_face_t *font_face,
//                    const cairo_user_data_key_t *key,
//                    void *user_data,
//                    cairo_destroy_func_t destroy);
static int l_cairo_font_face_set_user_data(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


/* Portable interface to general font features. */

// cairo_public cairo_scaled_font_t *
// cairo_scaled_font_create (cairo_font_face_t *font_face,
//               const cairo_matrix_t *font_matrix,
//               const cairo_matrix_t *ctm,
//               const cairo_font_options_t *options);
static int l_cairo_scaled_font_create(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    const cairo_matrix_t *font_matrix = get_cairo_matrix_t (L, 2);
    const cairo_matrix_t *ctm = get_cairo_matrix_t (L, 3);
    const cairo_font_options_t *options = get_cairo_font_options_t (L, 4);
    cairo_scaled_font_t *v = cairo_scaled_font_create (font_face, font_matrix, ctm, options);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public cairo_scaled_font_t *
// cairo_scaled_font_reference (cairo_scaled_font_t *scaled_font);
static int l_cairo_scaled_font_reference(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_scaled_font_t *v = cairo_scaled_font_reference (scaled_font);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public void
// cairo_scaled_font_destroy (cairo_scaled_font_t *scaled_font);
static int l_cairo_scaled_font_destroy(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = (cairo_scaled_font_t *) check_lightuserdata(L, 1);
    cairo_scaled_font_destroy (scaled_font);
    return 0;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public unsigned int
// cairo_scaled_font_get_reference_count (cairo_scaled_font_t *scaled_font);
static int l_cairo_scaled_font_get_reference_count(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    unsigned int v = cairo_scaled_font_get_reference_count (scaled_font);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public cairo_status_t
// cairo_scaled_font_status (cairo_scaled_font_t *scaled_font);
static int l_cairo_scaled_font_status(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_status_t v = cairo_scaled_font_status (scaled_font);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_font_type_t
// cairo_scaled_font_get_type (cairo_scaled_font_t *scaled_font);
static int l_cairo_scaled_font_get_type(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_font_type_t v = cairo_scaled_font_get_type (scaled_font);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void *
// cairo_scaled_font_get_user_data (cairo_scaled_font_t *scaled_font,
//                  const cairo_user_data_key_t *key);
static int l_cairo_scaled_font_get_user_data(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_status_t
// cairo_scaled_font_set_user_data (cairo_scaled_font_t *scaled_font,
//                  const cairo_user_data_key_t *key,
//                  void *user_data,
//                  cairo_destroy_func_t destroy);
static int l_cairo_scaled_font_set_user_data(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_scaled_font_extents (cairo_scaled_font_t *scaled_font,
//                cairo_font_extents_t *extents);
static int l_cairo_scaled_font_extents(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_font_extents_t *extents = get_cairo_font_extents_t (L, 2);
    cairo_scaled_font_extents (scaled_font, extents);
    return 0;
}


// cairo_public void
// cairo_scaled_font_text_extents (cairo_scaled_font_t *scaled_font,
//                 const char *utf8,
//                 cairo_text_extents_t *extents);
static int l_cairo_scaled_font_text_extents(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    const char *utf8 = luaL_checkstring(L, 2);
    cairo_text_extents_t *extents = get_cairo_text_extents_t (L, 3);
    cairo_scaled_font_text_extents (scaled_font, utf8, extents);
    return 0;
}


// cairo_public void
// cairo_scaled_font_glyph_extents (cairo_scaled_font_t *scaled_font,
//                  const cairo_glyph_t *glyphs,
//                  int num_glyphs,
//                  cairo_text_extents_t *extents);
static int l_cairo_scaled_font_glyph_extents(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_status_t
// cairo_scaled_font_text_to_glyphs (cairo_scaled_font_t *scaled_font,
//                   double x,
//                   double y,
//                   const char *utf8,
//                   int utf8_len,
//                   cairo_glyph_t **glyphs,
//                   int *num_glyphs,
//                   cairo_text_cluster_t **clusters,
//                   int *num_clusters,
//                   cairo_text_cluster_flags_t *cluster_flags);
static int l_cairo_scaled_font_text_to_glyphs(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


// cairo_public cairo_font_face_t *
// cairo_scaled_font_get_font_face (cairo_scaled_font_t *scaled_font);
static int l_cairo_scaled_font_get_font_face(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_font_face_t *v = cairo_scaled_font_get_font_face (scaled_font);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_scaled_font_get_font_matrix (cairo_scaled_font_t *scaled_font,
//                    cairo_matrix_t *font_matrix);
static int l_cairo_scaled_font_get_font_matrix(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_matrix_t *font_matrix = get_cairo_matrix_t (L, 2);
    cairo_scaled_font_get_font_matrix (scaled_font, font_matrix);
    return 0;
}


// cairo_public void
// cairo_scaled_font_get_ctm (cairo_scaled_font_t *scaled_font,
//                cairo_matrix_t *ctm);
static int l_cairo_scaled_font_get_ctm(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_matrix_t *ctm = get_cairo_matrix_t (L, 2);
    cairo_scaled_font_get_ctm (scaled_font, ctm);
    return 0;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_scaled_font_get_scale_matrix (cairo_scaled_font_t *scaled_font,
//                     cairo_matrix_t *scale_matrix);
static int l_cairo_scaled_font_get_scale_matrix(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


// cairo_public void
// cairo_scaled_font_get_font_options (cairo_scaled_font_t *scaled_font,
//                     cairo_font_options_t	*options);
static int l_cairo_scaled_font_get_font_options(lua_State* L)
{
    cairo_scaled_font_t	*scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_font_options_t *options = get_cairo_font_options_t (L, 2);
    cairo_scaled_font_get_font_options (scaled_font, options);
    return 0;
}


/* Toy fonts */

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_font_face_t *
// cairo_toy_font_face_create (const char *family,
//                 cairo_font_slant_t slant,
//                 cairo_font_weight_t weight);
static int l_cairo_toy_font_face_create(lua_State* L)
{
    const char *family = luaL_checkstring(L, 1);
    cairo_font_slant_t slant = (cairo_font_slant_t) luaL_checkinteger(L, 2);
    cairo_font_weight_t weight = (cairo_font_weight_t) luaL_checkinteger(L, 3);
    cairo_font_face_t *v = cairo_toy_font_face_create (family, slant, weight);
    lua_pushlightuserdata(L, v);
    return 1;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public const char *
// cairo_toy_font_face_get_family (cairo_font_face_t *font_face);
static int l_cairo_toy_font_face_get_family(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    const char *v = cairo_toy_font_face_get_family (font_face);
    lua_pushstring(L, v);
    return 1;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_font_slant_t
// cairo_toy_font_face_get_slant (cairo_font_face_t *font_face);
static int l_cairo_toy_font_face_get_slant(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    cairo_font_slant_t v = cairo_toy_font_face_get_slant (font_face);
    lua_pushinteger(L, v);
    return 1;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_font_weight_t
// cairo_toy_font_face_get_weight (cairo_font_face_t *font_face);
static int l_cairo_toy_font_face_get_weight(lua_State* L)
{
    cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    cairo_font_weight_t v = cairo_toy_font_face_get_weight (font_face);
    lua_pushinteger(L, v);
    return 1;
}
#endif



/* User fonts */

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_font_face_t *
// cairo_user_font_face_create (void);
static int l_cairo_user_font_face_create(lua_State* L)
{
    cairo_font_face_t *v = cairo_user_font_face_create ();
    lua_pushlightuserdata(L, v);
    return 1;
}
#endif


/* User-font method signatures */

/* User-font method setters */

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_user_font_face_set_init_func (cairo_font_face_t *font_face,
//                     cairo_user_scaled_font_init_func_t  init_func);
static int l_cairo_user_font_face_set_init_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_user_font_face_set_render_glyph_func (cairo_font_face_t *font_face,
//                         cairo_user_scaled_font_render_glyph_func_t  render_glyph_func);
static int l_cairo_user_font_face_set_render_glyph_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_user_font_face_set_text_to_glyphs_func (cairo_font_face_t *font_face,
//                           cairo_user_scaled_font_text_to_glyphs_func_t  text_to_glyphs_func);
static int l_cairo_user_font_face_set_text_to_glyphs_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_user_font_face_set_unicode_to_glyph_func (cairo_font_face_t *font_face,
//                             cairo_user_scaled_font_unicode_to_glyph_func_t  unicode_to_glyph_func);
static int l_cairo_user_font_face_set_unicode_to_glyph_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


/* User-font method getters */

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_user_scaled_font_init_func_t
// cairo_user_font_face_get_init_func (cairo_font_face_t *font_face);
static int l_cairo_user_font_face_get_init_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_user_scaled_font_render_glyph_func_t
// cairo_user_font_face_get_render_glyph_func (cairo_font_face_t *font_face);
static int l_cairo_user_font_face_get_render_glyph_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_user_scaled_font_text_to_glyphs_func_t
// cairo_user_font_face_get_text_to_glyphs_func (cairo_font_face_t *font_face);
static int l_cairo_user_font_face_get_text_to_glyphs_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_user_scaled_font_unicode_to_glyph_func_t
// cairo_user_font_face_get_unicode_to_glyph_func (cairo_font_face_t *font_face);
static int l_cairo_user_font_face_get_unicode_to_glyph_func(lua_State* L)
{
    //cairo_font_face_t *font_face = get_cairo_font_face_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


/* Query functions */

// cairo_public cairo_operator_t
// cairo_get_operator (cairo_t *cr);
static int l_cairo_get_operator(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_operator_t v = cairo_get_operator(cr);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_pattern_t *
// cairo_get_source (cairo_t *cr);
static int l_cairo_get_source(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_pattern_t *v = cairo_get_source(cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public double
// cairo_get_tolerance (cairo_t *cr);
static int l_cairo_get_tolerance(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double v = cairo_get_tolerance(cr);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public cairo_antialias_t
// cairo_get_antialias (cairo_t *cr);
static int l_cairo_get_antialias(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_antialias_t v = cairo_get_antialias(cr);
    lua_pushinteger(L, v);
    return 1;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_bool_t
// cairo_has_current_point (cairo_t *cr);
static int l_cairo_has_current_point(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_bool_t v = cairo_has_current_point (cr);
    lua_pushboolean(L, v);
    return 1;
}
#endif


// cairo_public void
// cairo_get_current_point (cairo_t *cr,
//              double *x, double *y);
static int l_cairo_get_current_point(lua_State* L)
{
    /// WARN:different usage
    cairo_t *cr = get_cairo_t (L, 1);
    double x = luaL_optnumber(L, 2, 0.0);
    double y = luaL_optnumber(L, 3, 0.0);
    cairo_get_current_point (cr, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}


// cairo_public cairo_fill_rule_t
// cairo_get_fill_rule (cairo_t *cr);
static int l_cairo_get_fill_rule(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_fill_rule_t v = cairo_get_fill_rule(cr);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public double
// cairo_get_line_width (cairo_t *cr);
static int l_cairo_get_line_width(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double v = cairo_get_line_width(cr);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public cairo_line_cap_t
// cairo_get_line_cap (cairo_t *cr);
static int l_cairo_get_line_cap(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_line_cap_t v = cairo_get_line_cap(cr);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_line_join_t
// cairo_get_line_join (cairo_t *cr);
static int l_cairo_get_line_join(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_line_join_t v = cairo_get_line_join(cr);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public double
// cairo_get_miter_limit (cairo_t *cr);
static int l_cairo_get_miter_limit(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    double v = cairo_get_miter_limit(cr);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public int
// cairo_get_dash_count (cairo_t *cr);
static int l_cairo_get_dash_count(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    int v = cairo_get_dash_count(cr);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_get_dash (cairo_t *cr,
//              double *dashes, double *offset);
static int l_cairo_get_dash(lua_State* L)
{
    //cairo_t *cr = get_cairo_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_get_matrix (cairo_t *cr,
//              cairo_matrix_t *matrix);
static int l_cairo_get_matrix(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_get_matrix (cr, matrix);
    return 0;
}


// cairo_public cairo_surface_t *
// cairo_get_target (cairo_t *cr);
static int l_cairo_get_target(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_surface_t *v = cairo_get_target(cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_get_group_target (cairo_t *cr);
static int l_cairo_get_group_target(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_surface_t *v = cairo_get_group_target(cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_path_t *
// cairo_copy_path (cairo_t *cr);
static int l_cairo_copy_path(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_path_t *v = cairo_copy_path (cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_path_t *
// cairo_copy_path_flat (cairo_t *cr);
static int l_cairo_copy_path_flat(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_path_t *v = cairo_copy_path_flat (cr);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public void
// cairo_append_path (cairo_t		*cr,
//            const cairo_path_t	*path);
static int l_cairo_append_path(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    const cairo_path_t *path = get_cairo_path_t (L, 2);
    cairo_append_path (cr, path);
    return 0;
}


// cairo_public void
// cairo_path_destroy (cairo_path_t *path);
static int l_cairo_path_destroy(lua_State* L)
{
    remove_Context(L, 1); // if called via Context userdata
    cairo_path_t *path = (cairo_path_t *) check_lightuserdata(L, 1);
    cairo_path_destroy (path);
    return 0;
}


/* Error status queries */

// cairo_public cairo_status_t
// cairo_status (cairo_t *cr);
static int l_cairo_status(lua_State* L)
{
    cairo_t *cr = get_cairo_t (L, 1);
    cairo_status_t v = cairo_status(cr);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public const char *
// cairo_status_to_string (cairo_status_t status);
static int l_cairo_status_to_string(lua_State* L)
{
    remove_Context(L, 1); // if called via Context userdata
    cairo_status_t status = (cairo_status_t) luaL_checkinteger(L, 1);
    const char *v = cairo_status_to_string(status);
    lua_pushstring(L, v);
    return 1;
}


/* Surface manipulation */

// cairo_public cairo_surface_t *
// cairo_surface_create_similar (cairo_surface_t  *other,
//                   cairo_content_t	content,
//                   int		width,
//                   int		height);
static int l_cairo_surface_create_similar(lua_State* L)
{
    cairo_surface_t *other = get_cairo_surface_t (L, 1);
    cairo_content_t content = (cairo_content_t) luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    cairo_surface_t *v = cairo_surface_create_similar(other, content, width, height);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public cairo_surface_t *
// cairo_surface_reference (cairo_surface_t *surface);
static int l_cairo_surface_reference(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_t *v = cairo_surface_reference(surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public void
// cairo_surface_finish (cairo_surface_t *surface);
static int l_cairo_surface_finish(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_finish(surface);
    return 0;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public void
// cairo_surface_destroy (cairo_surface_t *surface);
static int l_cairo_surface_destroy(lua_State* L)
{
    cairo_surface_t *surface = (cairo_surface_t *) check_lightuserdata(L, 1);
    cairo_surface_destroy(surface);
    return 0;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public unsigned int
// cairo_surface_get_reference_count (cairo_surface_t *surface);
static int l_cairo_surface_get_reference_count(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    unsigned int v = cairo_surface_get_reference_count(surface);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public cairo_status_t
// cairo_surface_status (cairo_surface_t *surface);
static int l_cairo_surface_status(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_status_t v = cairo_surface_status(surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_surface_type_t
// cairo_surface_get_type (cairo_surface_t *surface);
static int l_cairo_surface_get_type(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_type_t v = cairo_surface_get_type(surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_content_t
// cairo_surface_get_content (cairo_surface_t *surface);
static int l_cairo_surface_get_content(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_content_t v = cairo_surface_get_content (surface);
    lua_pushinteger(L, v);
    return 1;
}


#if CAIRO_HAS_PNG_FUNCTIONS

// cairo_public cairo_status_t
// cairo_surface_write_to_png (cairo_surface_t	*surface,
//                 const char		*filename);
static int l_cairo_surface_write_to_png(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    const char *filename = luaL_checkstring(L, 2);
    cairo_status_t v = cairo_surface_write_to_png (surface, filename);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_status_t
// cairo_surface_write_to_png_stream (cairo_surface_t *surface,
//                    cairo_write_func_t	write_func,
//                    void			*closure);
static int l_cairo_surface_write_to_png_stream(lua_State* L)
{
    //cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}

#endif /* CAIRO_HAS_PNG_FUNCTIONS */


// cairo_public void *
// cairo_surface_get_user_data (cairo_surface_t *surface,
//                  const cairo_user_data_key_t *key);
static int l_cairo_surface_get_user_data(lua_State* L)
{
    //cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_status_t
// cairo_surface_set_user_data (cairo_surface_t *surface,
//                  const cairo_user_data_key_t *key,
//                  void			 *user_data,
//                  cairo_destroy_func_t	 destroy);
static int l_cairo_surface_set_user_data(lua_State* L)
{
    //cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_surface_get_font_options (cairo_surface_t *surface,
//                 cairo_font_options_t *options);
static int l_cairo_surface_get_font_options(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_font_options_t *options = get_cairo_font_options_t (L, 2);
    cairo_surface_get_font_options (surface, options);
    return 0;
}


// cairo_public void
// cairo_surface_flush (cairo_surface_t *surface);
static int l_cairo_surface_flush(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_flush(surface);
    return 0;
}


// cairo_public void
// cairo_surface_mark_dirty (cairo_surface_t *surface);
static int l_cairo_surface_mark_dirty(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_mark_dirty (surface);
    return 0;
}


// cairo_public void
// cairo_surface_mark_dirty_rectangle (cairo_surface_t *surface,
//                     int              x,
//                     int              y,
//                     int              width,
//                     int              height);
static int l_cairo_surface_mark_dirty_rectangle(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int width = luaL_checkinteger(L, 4);
    int height = luaL_checkinteger(L, 5);
    cairo_surface_mark_dirty_rectangle (surface, x, y, width, height);
    return 0;
}


// cairo_public void
// cairo_surface_set_device_offset (cairo_surface_t *surface,
//                  double           x_offset,
//                  double           y_offset);
static int l_cairo_surface_set_device_offset(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    double x_offset = luaL_checknumber(L, 2);
    double y_offset = luaL_checknumber(L, 3);
    cairo_surface_set_device_offset (surface, x_offset, y_offset);
    return 0;
}


// cairo_public void
// cairo_surface_get_device_offset (cairo_surface_t *surface,
//                  double          *x_offset,
//                  double          *y_offset);
static int l_cairo_surface_get_device_offset(lua_State* L)
{
    /// WARN:different usage
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    double x_offset = luaL_optnumber(L, 2, 0.0);
    double y_offset = luaL_optnumber(L, 3, 0.0);
    cairo_surface_get_device_offset (surface, &x_offset, &y_offset);
    lua_pushnumber(L, x_offset);
    lua_pushnumber(L, y_offset);
    return 2;
}


// cairo_public void
// cairo_surface_set_fallback_resolution (cairo_surface_t	*surface,
//                        double		 x_pixels_per_inch,
//                        double		 y_pixels_per_inch);
static int l_cairo_surface_set_fallback_resolution(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    double x_pixels_per_inch = luaL_checknumber(L, 2);
    double y_pixels_per_inch = luaL_checknumber(L, 3);
    cairo_surface_set_fallback_resolution (surface, x_pixels_per_inch, y_pixels_per_inch);
    return 0;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public void
// cairo_surface_get_fallback_resolution (cairo_surface_t	*surface,
//                        double		*x_pixels_per_inch,
//                        double		*y_pixels_per_inch);
static int l_cairo_surface_get_fallback_resolution(lua_State* L)
{
    /// WARN:different usage
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    double x_pixels_per_inch = luaL_checknumber(L, 2);
    double y_pixels_per_inch = luaL_checknumber(L, 3);
    cairo_surface_get_fallback_resolution (surface, &x_pixels_per_inch, &y_pixels_per_inch);
    lua_pushnumber(L, x_pixels_per_inch);
    lua_pushnumber(L, y_pixels_per_inch);
    return 2;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public void
// cairo_surface_copy_page (cairo_surface_t *surface);
static int l_cairo_surface_copy_page(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_copy_page (surface);
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public void
// cairo_surface_show_page (cairo_surface_t *surface);
static int l_cairo_surface_show_page(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_show_page (surface);
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
// cairo_public cairo_bool_t
// cairo_surface_has_show_text_glyphs (cairo_surface_t *surface);
static int l_cairo_surface_has_show_text_glyphs(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_bool_t v = cairo_surface_has_show_text_glyphs (surface);
    lua_pushboolean(L, v);
    return 1;
}
#endif


/* Image-surface functions */

// cairo_public cairo_surface_t *
// cairo_image_surface_create (cairo_format_t	format,
//                 int			width,
//                 int			height);
static int l_cairo_image_surface_create(lua_State* L)
{
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    cairo_surface_t *v = cairo_image_surface_create (format, width, height);
    lua_pushlightuserdata(L, v);
    return 1;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public int
// cairo_format_stride_for_width (cairo_format_t	format,
//                 int		width);
static int l_cairo_format_stride_for_width(lua_State* L)
{
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 1);
    int width = luaL_checkinteger(L, 2);
    int v = cairo_format_stride_for_width (format, width);
    lua_pushinteger(L, v);
    return 1;
}
#endif


// cairo_public cairo_surface_t *
// cairo_image_surface_create_for_data (unsigned char *data,
//                      cairo_format_t format,
//                      int			width,
//                      int			height,
//                      int			stride);
static int l_cairo_image_surface_create_for_data(lua_State* L)
{
    // Gerald Franz (gerald.franz@googlemail.com) reported that
    // this is not working.
    /*
    const char* data = luaL_checkstring(L, 1);
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    int stride = luaL_checkinteger(L, 5);
    // WARN: (unsigned char*)data ?
    cairo_surface_t *v = cairo_image_surface_create_for_data ((unsigned char*)data, format, width, height, stride);
    lua_pushlightuserdata(L, v);
    return 1;
    */

    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public unsigned char *
// cairo_image_surface_get_data (cairo_surface_t *surface);
static int l_cairo_image_surface_get_data(lua_State* L)
{
    // Courtesy of Gerald Franz (gerald.franz@googlemail.com)

    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    unsigned char *data = cairo_image_surface_get_data (surface);
    int stride = cairo_image_surface_get_stride (surface);
    int height = cairo_image_surface_get_height (surface);
    size_t size = stride * height;
    lua_pushlstring(L, data, size);
    return 1;
}


// cairo_public cairo_format_t
// cairo_image_surface_get_format (cairo_surface_t *surface);
static int l_cairo_image_surface_get_format(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_format_t v = cairo_image_surface_get_format (surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public int
// cairo_image_surface_get_width (cairo_surface_t *surface);
static int l_cairo_image_surface_get_width(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int v = cairo_image_surface_get_width(surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public int
// cairo_image_surface_get_height (cairo_surface_t *surface);
static int l_cairo_image_surface_get_height(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int v = cairo_image_surface_get_height(surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public int
// cairo_image_surface_get_stride (cairo_surface_t *surface);
static int l_cairo_image_surface_get_stride(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int v = cairo_image_surface_get_stride(surface);
    lua_pushinteger(L, v);
    return 1;
}


#if CAIRO_HAS_PNG_FUNCTIONS

// cairo_public cairo_surface_t *
// cairo_image_surface_create_from_png (const char	*filename);
static int l_cairo_image_surface_create_from_png(lua_State* L)
{
    const char *filename = luaL_checkstring(L, 1);
    cairo_surface_t *v = cairo_image_surface_create_from_png(filename);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_image_surface_create_from_png_stream (cairo_read_func_t	read_func,
//                         void		*closure);
static int l_cairo_image_surface_create_from_png_stream(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}

#endif /* CAIRO_HAS_PNG_FUNCTIONS */


/* Pattern creation functions */

// cairo_public cairo_pattern_t *
// cairo_pattern_create_rgb (double red, double green, double blue);
static int l_cairo_pattern_create_rgb(lua_State* L)
{
    double red = luaL_checknumber(L, 1);
    double green = luaL_checknumber(L, 2);
    double blue = luaL_checknumber(L, 3);
    cairo_pattern_t *v = cairo_pattern_create_rgb (red, green, blue);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_pattern_t *
// cairo_pattern_create_rgba (double red, double green, double blue,
//                double alpha);
static int l_cairo_pattern_create_rgba(lua_State* L)
{
    double red = luaL_checknumber(L, 1);
    double green = luaL_checknumber(L, 2);
    double blue = luaL_checknumber(L, 3);
    double alpha = luaL_checknumber(L, 4);
    cairo_pattern_t *v = cairo_pattern_create_rgba (red, green, blue, alpha);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_pattern_t *
// cairo_pattern_create_for_surface (cairo_surface_t *surface);
static int l_cairo_pattern_create_for_surface(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_pattern_t *v = cairo_pattern_create_for_surface (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_pattern_t *
// cairo_pattern_create_linear (double x0, double y0,
//                  double x1, double y1);
static int l_cairo_pattern_create_linear(lua_State* L)
{
    double x0 = luaL_checknumber(L, 1);
    double y0 = luaL_checknumber(L, 2);
    double x1 = luaL_checknumber(L, 3);
    double y1 = luaL_checknumber(L, 4);
    cairo_pattern_t *v = cairo_pattern_create_linear(x0, y0, x1, y1);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_pattern_t *
// cairo_pattern_create_radial (double cx0, double cy0, double radius0,
//                  double cx1, double cy1, double radius1);
static int l_cairo_pattern_create_radial(lua_State* L)
{
    double cx0 = luaL_checknumber(L, 1);
    double cy0 = luaL_checknumber(L, 2);
    double radius0 = luaL_checknumber(L, 3);
    double cx1 = luaL_checknumber(L, 4);
    double cy1 = luaL_checknumber(L, 5);
    double radius1 = luaL_checknumber(L, 6);
    cairo_pattern_t *v = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public cairo_pattern_t *
// cairo_pattern_reference (cairo_pattern_t *pattern);
static int l_cairo_pattern_reference(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_pattern_t *v = cairo_pattern_reference(pattern);
    lua_pushlightuserdata(L, v);
    return 1;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public void
// cairo_pattern_destroy (cairo_pattern_t *pattern);
static int l_cairo_pattern_destroy(lua_State* L)
{
    cairo_pattern_t *pattern = (cairo_pattern_t *) check_lightuserdata(L, 1);
    cairo_pattern_destroy(pattern);
    return 0;
}


//FIXME should not be exposed to the user of the language binding,
//FIXME but rather used to implement memory management within the language binding
// cairo_public unsigned int
// cairo_pattern_get_reference_count (cairo_pattern_t *pattern);
static int l_cairo_pattern_get_reference_count(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    unsigned int v = cairo_pattern_get_reference_count(pattern);
    lua_pushnumber(L, v);
    return 1;
}


// cairo_public cairo_status_t
// cairo_pattern_status (cairo_pattern_t *pattern);
static int l_cairo_pattern_status(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_status_t v = cairo_pattern_status(pattern);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void *
// cairo_pattern_get_user_data (cairo_pattern_t		 *pattern,
//                  const cairo_user_data_key_t *key);
static int l_cairo_pattern_get_user_data(lua_State* L)
{
    //cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_status_t
// cairo_pattern_set_user_data (cairo_pattern_t		 *pattern,
//                  const cairo_user_data_key_t *key,
//                  void			 *user_data,
//                  cairo_destroy_func_t	  destroy);
static int l_cairo_pattern_set_user_data(lua_State* L)
{
    //cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_pattern_type_t
// cairo_pattern_get_type (cairo_pattern_t *pattern);
static int l_cairo_pattern_get_type(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_pattern_type_t v = cairo_pattern_get_type(pattern);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_pattern_add_color_stop_rgb (cairo_pattern_t *pattern,
//                   double offset,
//                   double red, double green, double blue);
static int l_cairo_pattern_add_color_stop_rgb(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    double offset = luaL_checknumber(L, 2);
    double red = luaL_checknumber(L, 3);
    double green = luaL_checknumber(L, 4);
    double blue = luaL_checknumber(L, 5);
    cairo_pattern_add_color_stop_rgb(pattern, offset, red, green, blue);
    return 0;
}


// cairo_public void
// cairo_pattern_add_color_stop_rgba (cairo_pattern_t *pattern,
//                    double offset,
//                    double red, double green, double blue,
//                    double alpha);
static int l_cairo_pattern_add_color_stop_rgba(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    double offset = luaL_checknumber(L, 2);
    double red = luaL_checknumber(L, 3);
    double green = luaL_checknumber(L, 4);
    double blue = luaL_checknumber(L, 5);
    double alpha = luaL_checknumber(L, 6);
    cairo_pattern_add_color_stop_rgba(pattern, offset, red, green, blue, alpha);
    return 0;
}


// cairo_public void
// cairo_pattern_set_matrix (cairo_pattern_t      *pattern,
//               const cairo_matrix_t *matrix);
static int l_cairo_pattern_set_matrix(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t (L, 1);
    const cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_pattern_set_matrix (pattern, matrix);
    return 0;
}


// cairo_public void
// cairo_pattern_get_matrix (cairo_pattern_t *pattern,
//               cairo_matrix_t  *matrix);
static int l_cairo_pattern_get_matrix(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t (L, 1);
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 2);
    cairo_pattern_get_matrix (pattern, matrix);
    return 0;
}


// cairo_public void
// cairo_pattern_set_extend (cairo_pattern_t *pattern, cairo_extend_t extend);
static int l_cairo_pattern_set_extend(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_extend_t extend = (cairo_extend_t) luaL_checkinteger(L, 2);
    cairo_pattern_set_extend (pattern, extend);
    return 0;
}


// cairo_public cairo_extend_t
// cairo_pattern_get_extend (cairo_pattern_t *pattern);
static int l_cairo_pattern_get_extend(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_extend_t v = cairo_pattern_get_extend (pattern);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_pattern_set_filter (cairo_pattern_t *pattern, cairo_filter_t filter);
static int l_cairo_pattern_set_filter(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_filter_t filter = (cairo_filter_t) luaL_checkinteger(L, 2);
    cairo_pattern_set_filter (pattern, filter);
    return 0;
}


// cairo_public cairo_filter_t
// cairo_pattern_get_filter (cairo_pattern_t *pattern);
static int l_cairo_pattern_get_filter(lua_State* L)
{
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_filter_t v = cairo_pattern_get_filter (pattern);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public cairo_status_t
// cairo_pattern_get_rgba (cairo_pattern_t *pattern,
//             double *red, double *green,
//             double *blue, double *alpha);
static int l_cairo_pattern_get_rgba(lua_State* L)
{
    /// WARN:different usage
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    double red   = luaL_optnumber(L, 2, 0.0);
    double green = luaL_optnumber(L, 3, 0.0);
    double blue  = luaL_optnumber(L, 4, 0.0);
    double alpha = luaL_optnumber(L, 5, 0.0);
    cairo_status_t v = cairo_pattern_get_rgba (pattern, &red, &green, &blue, &alpha);
    lua_pushinteger(L, v);
    lua_pushnumber(L, red);
    lua_pushnumber(L, green);
    lua_pushnumber(L, blue);
    lua_pushnumber(L, alpha);
    return 5;
}


// cairo_public cairo_status_t
// cairo_pattern_get_surface (cairo_pattern_t *pattern,
//                cairo_surface_t **surface);
static int l_cairo_pattern_get_surface(lua_State* L)
{
    /// WARN:different usage
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    cairo_surface_t *surface = (cairo_surface_t *) check_lightuserdata(L, 2);
    cairo_status_t v = cairo_pattern_get_surface (pattern, &surface);
    lua_pushinteger(L, v);
    lua_pushlightuserdata(L, surface);
    return 2;
}


// cairo_public cairo_status_t
// cairo_pattern_get_color_stop_rgba (cairo_pattern_t *pattern,
//                    int index, double *offset,
//                    double *red, double *green,
//                    double *blue, double *alpha);
static int l_cairo_pattern_get_color_stop_rgba(lua_State* L)
{
    /// WARN:different usage
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    int index     = luaL_checkinteger(L, 2);
    double offset = luaL_optnumber(L, 3, 0.0);
    double red    = luaL_optnumber(L, 4, 0.0);
    double green  = luaL_optnumber(L, 5, 0.0);
    double blue   = luaL_optnumber(L, 6, 0.0);
    double alpha  = luaL_optnumber(L, 7, 0.0);
    cairo_status_t v = cairo_pattern_get_color_stop_rgba (pattern, index, &offset, &red, &green, &blue, &alpha);
    lua_pushinteger(L, v);
    lua_pushnumber(L, offset);
    lua_pushnumber(L, red);
    lua_pushnumber(L, green);
    lua_pushnumber(L, blue);
    lua_pushnumber(L, alpha);
    return 6;
}


// cairo_public cairo_status_t
// cairo_pattern_get_color_stop_count (cairo_pattern_t *pattern,
//                     int *count);
static int l_cairo_pattern_get_color_stop_count(lua_State* L)
{
    /// WARN:different usage
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    int count = luaL_optinteger(L, 2, 0);
    cairo_status_t v = cairo_pattern_get_color_stop_count (pattern, &count);
    lua_pushinteger(L, v);
    lua_pushinteger(L, count);
    return 2;
}


// cairo_public cairo_status_t
// cairo_pattern_get_linear_points (cairo_pattern_t *pattern,
//                  double *x0, double *y0,
//                  double *x1, double *y1);
static int l_cairo_pattern_get_linear_points(lua_State* L)
{
    /// WARN:different usage
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    double x0 = luaL_optnumber(L, 2, 0.0);
    double y0 = luaL_optnumber(L, 3, 0.0);
    double x1 = luaL_optnumber(L, 4, 0.0);
    double y1 = luaL_optnumber(L, 5, 0.0);
    cairo_status_t v = cairo_pattern_get_linear_points (pattern, &x0, &y0, &x1, &y1);
    lua_pushinteger(L, v);
    lua_pushnumber(L, x0);
    lua_pushnumber(L, y0);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    return 5;
}


// cairo_public cairo_status_t
// cairo_pattern_get_radial_circles (cairo_pattern_t *pattern,
//                   double *x0, double *y0, double *r0,
//                   double *x1, double *y1, double *r1);
static int l_cairo_pattern_get_radial_circles(lua_State* L)
{
    /// WARN:different usage
    cairo_pattern_t *pattern = get_cairo_pattern_t(L, 1);
    double x0 = luaL_optnumber(L, 2, 0.0);
    double y0 = luaL_optnumber(L, 3, 0.0);
    double r0 = luaL_optnumber(L, 4, 0.0);
    double x1 = luaL_optnumber(L, 5, 0.0);
    double y1 = luaL_optnumber(L, 6, 0.0);
    double r1 = luaL_optnumber(L, 7, 0.0);
    cairo_status_t v = cairo_pattern_get_radial_circles (pattern, &x0, &y0, &r0, &x1, &y1, &r1);
    lua_pushinteger(L, v);
    lua_pushnumber(L, x0);
    lua_pushnumber(L, y0);
    lua_pushnumber(L, r0);
    lua_pushnumber(L, x1);
    lua_pushnumber(L, y1);
    lua_pushnumber(L, r1);
    return 7;
}


/* Matrix functions */

// cairo_public void
// cairo_matrix_init (cairo_matrix_t *matrix,
//            double  xx, double  yx,
//            double  xy, double  yy,
//            double  x0, double  y0);
static int l_cairo_matrix_init(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  xx = luaL_checknumber(L, 2);
    double  yx = luaL_checknumber(L, 3);
    double  xy = luaL_checknumber(L, 4);
    double  yy = luaL_checknumber(L, 5);
    double  x0 = luaL_checknumber(L, 6);
    double  y0 = luaL_checknumber(L, 7);
    cairo_matrix_init (matrix, xx, yx, xy, yy, x0, y0);
    return 0;
}


// cairo_public void
// cairo_matrix_init_identity (cairo_matrix_t *matrix);
static int l_cairo_matrix_init_identity(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    cairo_matrix_init_identity (matrix);
    return 0;
}


// cairo_public void
// cairo_matrix_init_translate (cairo_matrix_t *matrix,
//                  double tx, double ty);
static int l_cairo_matrix_init_translate(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  tx = luaL_checknumber(L, 2);
    double  ty = luaL_checknumber(L, 3);
    cairo_matrix_init_translate (matrix, tx, ty);
    return 0;
}


// cairo_public void
// cairo_matrix_init_scale (cairo_matrix_t *matrix,
//              double sx, double sy);
static int l_cairo_matrix_init_scale(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  sx = luaL_checknumber(L, 2);
    double  sy = luaL_checknumber(L, 3);
    cairo_matrix_init_scale (matrix, sx, sy);
    return 0;
}


// cairo_public void
// cairo_matrix_init_rotate (cairo_matrix_t *matrix,
//               double radians);
static int l_cairo_matrix_init_rotate(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double radians = luaL_checknumber(L, 2);
    cairo_matrix_init_rotate (matrix, radians);
    return 0;
}


// cairo_public void
// cairo_matrix_translate (cairo_matrix_t *matrix, double tx, double ty);
static int l_cairo_matrix_translate(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  tx = luaL_checknumber(L, 2);
    double  ty = luaL_checknumber(L, 3);
    cairo_matrix_translate (matrix, tx, ty);
    return 0;
}


// cairo_public void
// cairo_matrix_scale (cairo_matrix_t *matrix, double sx, double sy);
static int l_cairo_matrix_scale(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  sx = luaL_checknumber(L, 2);
    double  sy = luaL_checknumber(L, 3);
    cairo_matrix_scale (matrix, sx, sy);
    return 0;
}


// cairo_public void
// cairo_matrix_rotate (cairo_matrix_t *matrix, double radians);
static int l_cairo_matrix_rotate(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double radians = luaL_checknumber(L, 2);
    cairo_matrix_rotate (matrix, radians);
    return 0;
}


// cairo_public cairo_status_t
// cairo_matrix_invert (cairo_matrix_t *matrix);
static int l_cairo_matrix_invert(lua_State* L)
{
    cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    cairo_status_t v = cairo_matrix_invert (matrix);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_matrix_multiply (cairo_matrix_t *result,
//                const cairo_matrix_t *a,
//                const cairo_matrix_t *b);
static int l_cairo_matrix_multiply(lua_State* L)
{
    cairo_matrix_t *result = get_cairo_matrix_t (L, 1);
    const cairo_matrix_t *a = get_cairo_matrix_t (L, 2);
    const cairo_matrix_t *b = get_cairo_matrix_t (L, 3);
    cairo_matrix_multiply (result, a, b);
    return 0;
}


// cairo_public void
// cairo_matrix_transform_distance (const cairo_matrix_t *matrix,
//                  double *dx, double *dy);
static int l_cairo_matrix_transform_distance(lua_State* L)
{
    /// WARN:different usage
    const cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  dx = luaL_checknumber(L, 2);
    double  dy = luaL_checknumber(L, 3);
    cairo_matrix_transform_distance (matrix, &dx, &dy);
    lua_pushnumber(L, dx);
    lua_pushnumber(L, dy);
    return 2;
}


// cairo_public void
// cairo_matrix_transform_point (const cairo_matrix_t *matrix,
//                   double *x, double *y);
static int l_cairo_matrix_transform_point(lua_State* L)
{
    /// WARN:different usage
    const cairo_matrix_t *matrix = get_cairo_matrix_t (L, 1);
    double  x = luaL_checknumber(L, 2);
    double  y = luaL_checknumber(L, 3);
    cairo_matrix_transform_point (matrix, &x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}


/* Functions to be used while debugging (not intended for use in production code) */
// cairo_public void
// cairo_debug_reset_static_data (void);
static int l_cairo_debug_reset_static_data(lua_State* L)
{
    //remove_Context(L, 1); // if called via Context userdata
    cairo_debug_reset_static_data ();
    return 0;
}


#if CAIRO_HAS_SVG_SURFACE

// cairo_public cairo_surface_t *
// cairo_svg_surface_create (const char   *filename,
//               double	width_in_points,
//               double	height_in_points);
static int l_cairo_svg_surface_create(lua_State* L)
{
    const char *filename = luaL_checkstring(L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_surface_t *v = cairo_svg_surface_create(filename, width_in_points, height_in_points);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_svg_surface_create_for_stream (cairo_write_func_t	write_func,
//                      void	       *closure,
//                      double		width_in_points,
//                      double		height_in_points);
static int l_cairo_svg_surface_create_for_stream(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_svg_surface_restrict_to_version (cairo_surface_t *surface,
//                        cairo_svg_version_t  	 version);
static int l_cairo_svg_surface_restrict_to_version(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_svg_version_t version = (cairo_svg_version_t) luaL_checkinteger(L, 2);
    cairo_svg_surface_restrict_to_version (surface, version);
    return 0;
}


// cairo_public void
// cairo_svg_get_versions (cairo_svg_version_t const	**versions,
//                         int                      	 *num_versions);
static int l_cairo_svg_get_versions(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public const char *
// cairo_svg_version_to_string (cairo_svg_version_t version);
static int l_cairo_svg_version_to_string(lua_State* L)
{
    cairo_svg_version_t version = (cairo_svg_version_t) luaL_checkinteger(L, 1);
    const char *v = cairo_svg_version_to_string (version);
    lua_pushstring(L, v);
    return 1;
}

#endif /* CAIRO_HAS_SVG_SURFACE */


#if CAIRO_HAS_PDF_SURFACE

// cairo_public cairo_surface_t *
// cairo_pdf_surface_create (const char		*filename,
//               double		 width_in_points,
//               double		 height_in_points);
static int l_cairo_pdf_surface_create(lua_State* L)
{
    const char *filename = luaL_checkstring(L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_surface_t *v = cairo_pdf_surface_create(filename, width_in_points, height_in_points);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_pdf_surface_create_for_stream (cairo_write_func_t	write_func,
//                      void	       *closure,
//                      double		width_in_points,
//                      double		height_in_points);
static int l_cairo_pdf_surface_create_for_stream(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_pdf_surface_set_size (cairo_surface_t	*surface,
//                 double		 width_in_points,
//                 double		 height_in_points);
static int l_cairo_pdf_surface_set_size(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_pdf_surface_set_size (surface, width_in_points, height_in_points);
    return 0;
}

#endif /* CAIRO_HAS_PDF_SURFACE */


#if CAIRO_HAS_PS_SURFACE

/* PS-surface functions */

// cairo_public cairo_surface_t *
// cairo_ps_surface_create (const char		*filename,
//              double			 width_in_points,
//              double			 height_in_points);
static int l_cairo_ps_surface_create(lua_State* L)
{
    const char *filename = luaL_checkstring(L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_surface_t *v = cairo_ps_surface_create(filename, width_in_points, height_in_points);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_ps_surface_create_for_stream (cairo_write_func_t	write_func,
//                     void	       *closure,
//                     double		width_in_points,
//                     double		height_in_points);
static int l_cairo_ps_surface_create_for_stream(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public void
// cairo_ps_surface_restrict_to_level (cairo_surface_t    *surface,
//                                     cairo_ps_level_t    level);
static int l_cairo_ps_surface_restrict_to_level(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    cairo_ps_level_t level = (cairo_ps_level_t)luaL_checkinteger(L, 2);
    cairo_ps_surface_restrict_to_level (surface, level);
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public void
// cairo_ps_get_levels (cairo_ps_level_t const  **levels,
//                      int                      *num_levels);
static int l_cairo_ps_get_levels(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public const char *
// cairo_ps_level_to_string (cairo_ps_level_t level);
static int l_cairo_ps_level_to_string(lua_State* L)
{
    cairo_ps_level_t level = (cairo_ps_level_t)luaL_checkinteger(L, 1);
    const char *v = cairo_ps_level_to_string (level);
    lua_pushstring(L, v);
    return 1;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public void
// cairo_ps_surface_set_eps (cairo_surface_t	*surface,
//               cairo_bool_t           eps);
static int l_cairo_ps_surface_set_eps(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    cairo_bool_t eps = (cairo_bool_t)luaL_checkinteger(L, 2);
    cairo_ps_surface_set_eps (surface, eps);
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_bool_t
// cairo_ps_surface_get_eps (cairo_surface_t	*surface);
static int l_cairo_ps_surface_get_eps(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    cairo_bool_t v = cairo_ps_surface_get_eps (surface);
    lua_pushboolean(L, v);
    return 1;
}
#endif


// cairo_public void
// cairo_ps_surface_set_size (cairo_surface_t	*surface,
//                double		 width_in_points,
//                double		 height_in_points);
static int l_cairo_ps_surface_set_size(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    double width_in_points = luaL_checknumber(L, 2);
    double height_in_points = luaL_checknumber(L, 3);
    cairo_ps_surface_set_size (surface, width_in_points, height_in_points);
    return 0;
}


// cairo_public void
// cairo_ps_surface_dsc_comment (cairo_surface_t	*surface,
//                   const char	*comment);
static int l_cairo_ps_surface_dsc_comment(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    const char *comment = luaL_checkstring(L, 2);
    cairo_ps_surface_dsc_comment (surface, comment);
    return 0;
}


// cairo_public void
// cairo_ps_surface_dsc_begin_setup (cairo_surface_t *surface);
static int l_cairo_ps_surface_dsc_begin_setup(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    cairo_ps_surface_dsc_begin_setup (surface);
    return 0;
}


// cairo_public void
// cairo_ps_surface_dsc_begin_page_setup (cairo_surface_t *surface);
static int l_cairo_ps_surface_dsc_begin_page_setup(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    cairo_ps_surface_dsc_begin_page_setup (surface);
    return 0;
}

#endif /* CAIRO_HAS_PS_SURFACE */


#if CAIRO_HAS_XCB_SURFACE

// cairo_public cairo_surface_t *
// cairo_xcb_surface_create (xcb_connection_t *c,
//               xcb_drawable_t	 drawable,
//               xcb_visualtype_t *visual,
//               int		 width,
//               int		 height);
static int l_cairo_xcb_surface_create(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_surface_t *
// cairo_xcb_surface_create_for_bitmap (xcb_connection_t *c,
//                      xcb_pixmap_t	    bitmap,
//                      xcb_screen_t	   *screen,
//                      int	    width,
//                      int	    height);
static int l_cairo_xcb_surface_create_for_bitmap(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_xcb_surface_set_size (cairo_surface_t *surface,
//                 int		     width,
//                 int		     height);
static int l_cairo_xcb_surface_set_size(lua_State* L)
{
    cairo_surface_t	*surface = get_cairo_surface_t (L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    cairo_xcb_surface_set_size (surface, width, height);
    return 0;
}


#if CAIRO_HAS_XLIB_XRENDER_SURFACE
// cairo_public cairo_surface_t *
// cairo_xcb_surface_create_with_xrender_format (xcb_connection_t	    *c,
//                           xcb_drawable_t	     drawable,
//                           xcb_screen_t		    *screen,
//                           xcb_render_pictforminfo_t *format,
//                           int		     width,
//                           int		     height);
static int l_cairo_xcb_surface_create_with_xrender_format(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}
#endif

#endif /* CAIRO_HAS_XCB_SURFACE */


#if CAIRO_HAS_XLIB_SURFACE

// cairo_public cairo_surface_t *
// cairo_xlib_surface_create (Display     *dpy,
//                Drawable	drawable,
//                Visual      *visual,
//                int		width,
//                int		height);
static int l_cairo_xlib_surface_create(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_surface_t *
// cairo_xlib_surface_create_for_bitmap (Display  *dpy,
//                       Pixmap	bitmap,
//                       Screen	*screen,
//                       int	width,
//                       int	height);
static int l_cairo_xlib_surface_create_for_bitmap(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_xlib_surface_set_size (cairo_surface_t *surface,
//                  int              width,
//                  int              height);
static int l_cairo_xlib_surface_set_size(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    cairo_xlib_surface_set_size (surface, width, height);
    return 0;
}


// cairo_public void
// cairo_xlib_surface_set_drawable (cairo_surface_t *surface,
//                  Drawable	  drawable,
//                  int              width,
//                  int              height);
static int l_cairo_xlib_surface_set_drawable(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public Display *
// cairo_xlib_surface_get_display (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_display(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    Display *v = cairo_xlib_surface_get_display (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public Drawable
// cairo_xlib_surface_get_drawable (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_drawable(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public Screen *
// cairo_xlib_surface_get_screen (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_screen(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    Screen *v = cairo_xlib_surface_get_screen (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public Visual *
// cairo_xlib_surface_get_visual (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_visual(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    Visual *v = cairo_xlib_surface_get_visual (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public int
// cairo_xlib_surface_get_depth (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_depth(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int v = cairo_xlib_surface_get_depth (surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public int
// cairo_xlib_surface_get_width (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_width(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int v = cairo_xlib_surface_get_width (surface);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public int
// cairo_xlib_surface_get_height (cairo_surface_t *surface);
static int l_cairo_xlib_surface_get_height(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int v = cairo_xlib_surface_get_height (surface);
    lua_pushinteger(L, v);
    return 1;
}


#if CAIRO_HAS_XLIB_XRENDER_SURFACE
// cairo_public cairo_surface_t *
// cairo_xlib_surface_create_with_xrender_format (Display		 *dpy,
//                                                Drawable		  drawable,
//                            Screen		 *screen,
//                                                XRenderPictFormat *format,
//                                                int		  width,
//                                                int		  height);
static int l_cairo_xlib_surface_create_with_xrender_format(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}
#endif

#endif /* CAIRO_HAS_XLIB_SURFACE */


#if CAIRO_HAS_WIN32_SURFACE

// cairo_public cairo_surface_t *
// cairo_win32_surface_create (HDC hdc);
static int l_cairo_win32_surface_create(lua_State* L)
{
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_surface_t *v = cairo_win32_surface_create(hdc);
    lua_pushlightuserdata(L, v);
    return 1;
}


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_surface_t *
// cairo_win32_printing_surface_create (HDC hdc);
static int l_cairo_win32_printing_surface_create(lua_State* L)
{
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_surface_t *v = cairo_win32_printing_surface_create(hdc);
    lua_pushlightuserdata(L, v);
    return 1;
}
#endif


// cairo_public cairo_surface_t *
// cairo_win32_surface_create_with_ddb (HDC hdc,
//                                      cairo_format_t format,
//                                      int width,
//                                      int height);
static int l_cairo_win32_surface_create_with_ddb(lua_State* L)
{
    HDC hdc = (HDC) check_lightuserdata(L, 1);
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    cairo_surface_t *v = cairo_win32_surface_create_with_ddb (hdc, format, width, height);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_win32_surface_create_with_dib (cairo_format_t format,
//                                      int width,
//                                      int height);
static int l_cairo_win32_surface_create_with_dib(lua_State* L)
{
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 1);
    int width = luaL_checkinteger(L, 2);
    int height = luaL_checkinteger(L, 3);
    cairo_surface_t *v = cairo_win32_surface_create_with_dib (format, width, height);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public HDC
// cairo_win32_surface_get_dc (cairo_surface_t *surface);
static int l_cairo_win32_surface_get_dc(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    HDC v = cairo_win32_surface_get_dc (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_win32_surface_get_image (cairo_surface_t *surface);
static int l_cairo_win32_surface_get_image(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_surface_t *v = cairo_win32_surface_get_image (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}


#if CAIRO_HAS_WIN32_FONT
// cairo_public cairo_font_face_t *
// cairo_win32_font_face_create_for_logfontw (LOGFONTW *logfont);
static int l_cairo_win32_font_face_create_for_logfontw(lua_State* L)
{
    LOGFONTW *logfont = (LOGFONTW *) check_lightuserdata(L, 1);
    cairo_font_face_t *v = cairo_win32_font_face_create_for_logfontw (logfont);
    lua_pushlightuserdata(L, v);
    return 1;
}
#endif


#if CAIRO_HAS_WIN32_FONT
// cairo_public cairo_font_face_t *
// cairo_win32_font_face_create_for_hfont (HFONT font);
static int l_cairo_win32_font_face_create_for_hfont(lua_State* L)
{
    HFONT font = (HFONT) check_lightuserdata(L, 1);
    cairo_font_face_t *v = cairo_win32_font_face_create_for_hfont (font);
    lua_pushlightuserdata(L, v);
    return 1;
}
#endif


#if CAIRO_HAS_WIN32_FONT
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_font_face_t *
// cairo_win32_font_face_create_for_logfontw_hfont (LOGFONTW *logfont, HFONT font);
static int l_cairo_win32_font_face_create_for_logfontw_hfont(lua_State* L)
{
    LOGFONTW *logfont = (LOGFONTW *) check_lightuserdata(L, 1);
    HFONT font = (HFONT) check_lightuserdata(L, 2);
    cairo_font_face_t *v = cairo_win32_font_face_create_for_logfontw_hfont (logfont, font);
    lua_pushlightuserdata(L, v);
    return 1;
}
#endif
#endif


#if CAIRO_HAS_WIN32_FONT
// cairo_public cairo_status_t
// cairo_win32_scaled_font_select_font (cairo_scaled_font_t *scaled_font,
//                      HDC                  hdc);
static int l_cairo_win32_scaled_font_select_font(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    HDC hdc = (HDC) check_lightuserdata(L, 2);
    cairo_status_t v = cairo_win32_scaled_font_select_font (scaled_font, hdc);
    lua_pushinteger(L, v);
    return 1;
}
#endif


#if CAIRO_HAS_WIN32_FONT
// cairo_public void
// cairo_win32_scaled_font_done_font (cairo_scaled_font_t *scaled_font);
static int l_cairo_win32_scaled_font_done_font(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_win32_scaled_font_done_font (scaled_font);
    return 0;
}
#endif


#if CAIRO_HAS_WIN32_FONT
// cairo_public double
// cairo_win32_scaled_font_get_metrics_factor (cairo_scaled_font_t *scaled_font);
static int l_cairo_win32_scaled_font_get_metrics_factor(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    double v = cairo_win32_scaled_font_get_metrics_factor (scaled_font);
    lua_pushnumber(L, v);
    return 1;
}
#endif


#if CAIRO_HAS_WIN32_FONT
// cairo_public void
// cairo_win32_scaled_font_get_logical_to_device (cairo_scaled_font_t *scaled_font,
//                            cairo_matrix_t *logical_to_device);
static int l_cairo_win32_scaled_font_get_logical_to_device(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_matrix_t *logical_to_device = get_cairo_matrix_t (L, 2);
    cairo_win32_scaled_font_get_logical_to_device (scaled_font, logical_to_device);
    return 0;
}
#endif


#if CAIRO_HAS_WIN32_FONT
// cairo_public void
// cairo_win32_scaled_font_get_device_to_logical (cairo_scaled_font_t *scaled_font,
//                            cairo_matrix_t *device_to_logical);
static int l_cairo_win32_scaled_font_get_device_to_logical(lua_State* L)
{
    cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    cairo_matrix_t *device_to_logical = get_cairo_matrix_t (L, 2);
    cairo_win32_scaled_font_get_device_to_logical (scaled_font, device_to_logical);
    return 0;
}
#endif

#endif /* CAIRO_HAS_WIN32_SURFACE */


#if CAIRO_HAS_BEOS_SURFACE

// cairo_public cairo_surface_t *
// cairo_beos_surface_create (BView* view);
static int l_cairo_beos_surface_create(lua_State* L)
{
    BView* view = (BView *) check_lightuserdata(L, 1);
    cairo_surface_t *v = cairo_beos_surface_create (view);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_beos_surface_create_for_bitmap (BView*   view,
//                       BBitmap* bmp);
static int l_cairo_beos_surface_create_for_bitmap(lua_State* L)
{
    BView* view = (BView *) check_lightuserdata(L, 1);
    BBitmap* bmp = (BBitmap *) check_lightuserdata(L, 2);
    cairo_surface_t *v = cairo_beos_surface_create_for_bitmap (view, bmp);
    lua_pushlightuserdata(L, v);
    return 1;
}

#endif /* CAIRO_HAS_BEOS_SURFACE */


#if CAIRO_HAS_DIRECTFB_SURFACE

// cairo_public cairo_surface_t *
// cairo_directfb_surface_create (IDirectFB *dfb, IDirectFBSurface *surface);
static int l_cairo_directfb_surface_create(lua_State* L)
{
    IDirectFB *dfb = (IDirectFB *) check_lightuserdata(L, 1);
    IDirectFBSurface *surface = (IDirectFBSurface *) check_lightuserdata(L, 2);
    cairo_surface_t *v = cairo_directfb_surface_create (dfb, surface);
    lua_pushlightuserdata(L, v);
    return 1;
}

#endif /*CAIRO_HAS_DIRECTFB_SURFACE*/


#if CAIRO_HAS_OS2_SURFACE

/* The OS/2 Specific Cairo API */

// cairo_public void
// cairo_os2_init (void);
static int l_cairo_os2_init(lua_State* L)
{
    cairo_os2_init ();
    return 0;
}


// cairo_public void
// cairo_os2_fini (void);
static int l_cairo_os2_fini(lua_State* L)
{
    cairo_os2_fini ();
    return 0;
}


// cairo_public cairo_surface_t *
// cairo_os2_surface_create (HPS hps_client_window,
//                           int width,
//                           int height);
static int l_cairo_os2_surface_create(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_os2_surface_set_hwnd (cairo_surface_t *surface,
//                             HWND             hwnd_client_window);
static int l_cairo_os2_surface_set_hwnd(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public int
// cairo_os2_surface_set_size (cairo_surface_t *surface,
//                             int              new_width,
//                             int              new_height,
//                             int              timeout);
static int l_cairo_os2_surface_set_size(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    int new_width = luaL_checkinteger(L, 2);
    int new_height = luaL_checkinteger(L, 3);
    int timeout = luaL_checkinteger(L, 4);
    int v = cairo_os2_surface_set_size (surface, new_width, new_height, timeout);
    lua_pushinteger(L, v);
    return 1;
}


// cairo_public void
// cairo_os2_surface_refresh_window (cairo_surface_t *surface,
//                                   HPS              hps_begin_paint,
//                                   PRECTL           prcl_begin_paint_rect);
static int l_cairo_os2_surface_refresh_window(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_os2_surface_set_manual_window_refresh (cairo_surface_t *surface,
//                                              cairo_bool_t     manual_refresh);
static int l_cairo_os2_surface_set_manual_window_refresh(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_bool_t manual_refresh = (cairo_bool_t) luaL_checkinteger(L, 2);
    cairo_os2_surface_set_manual_window_refresh (surface, manual_refresh);
    return 0;
}


// cairo_public cairo_bool_t
// cairo_os2_surface_get_manual_window_refresh (cairo_surface_t *surface);
static int l_cairo_os2_surface_get_manual_window_refresh(lua_State* L)
{
    cairo_surface_t *surface = get_cairo_surface_t (L, 1);
    cairo_bool_t v = cairo_os2_surface_get_manual_window_refresh (surface);
    lua_pushboolean(L, v);
    return 1;
}

#endif /* CAIRO_HAS_OS2_SURFACE */


#if CAIRO_HAS_GLITZ_SURFACE

// cairo_public cairo_surface_t *
// cairo_glitz_surface_create (glitz_surface_t *surface);
static int l_cairo_glitz_surface_create(lua_State* L)
{
    glitz_surface_t *surface = (glitz_surface_t *) check_lightuserdata(L, 1);
    cairo_surface_t *v = cairo_glitz_surface_create (surface);
    lua_pushlightuserdata(L, v);
    return 1;
}

#endif /* CAIRO_HAS_GLITZ_SURFACE */


#if CAIRO_HAS_QUARTZ_SURFACE

// cairo_public cairo_surface_t *
// cairo_quartz_surface_create (cairo_format_t format,
//                              unsigned int width,
//                              unsigned int height);
static int l_cairo_quartz_surface_create(lua_State* L)
{
    cairo_format_t format = (cairo_format_t) luaL_checkinteger(L, 1);
    unsigned int width = luaL_checknumber(L, 2);
    unsigned int height = luaL_checknumber(L, 3);
    cairo_surface_t *v = cairo_quartz_surface_create (format, width, height);
    lua_pushlightuserdata(L, v);
    return 1;
}


// cairo_public cairo_surface_t *
// cairo_quartz_surface_create_for_cg_context (CGContextRef cgContext,
//                                             unsigned int width,
//                                             unsigned int height);
static int l_cairo_quartz_surface_create_for_cg_context(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public CGContextRef
// cairo_quartz_surface_get_cg_context (cairo_surface_t *surface);
static int l_cairo_quartz_surface_get_cg_context(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


/*
 * Quartz font support
 */

#ifdef CAIRO_HAS_QUARTZ_FONT

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_font_face_t *
// cairo_quartz_font_face_create_for_cgfont (CGFontRef font);
static int l_cairo_quartz_font_face_create_for_cgfont(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_font_face_t *
// cairo_quartz_font_face_create_for_atsu_font_id (ATSUFontID font_id);
static int l_cairo_quartz_font_face_create_for_atsu_font_id(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}
#endif


#endif /* CAIRO_HAS_QUARTZ_FONT */

#endif /* CAIRO_HAS_QUARTZ_SURFACE */


#if CAIRO_HAS_ATSUI_FONT

#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 6, 0)
// cairo_public cairo_font_face_t *
// cairo_atsui_font_face_create_for_atsu_font_id (ATSUFontID font_id);
static int l_cairo_atsui_font_face_create_for_atsu_font_id(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}
#endif

#endif /* CAIRO_HAS_ATSUI_FONT */


#if CAIRO_HAS_FT_FONT

/* Fontconfig/Freetype platform-specific font interface */

// cairo_public cairo_font_face_t *
// cairo_ft_font_face_create_for_pattern (FcPattern *pattern);
static int l_cairo_ft_font_face_create_for_pattern(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_ft_font_options_substitute (const cairo_font_options_t *options,
//                   FcPattern                  *pattern);
static int l_cairo_ft_font_options_substitute(lua_State* L)
{
    //const cairo_font_options_t *options = get_cairo_font_options_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public cairo_font_face_t *
// cairo_ft_font_face_create_for_ft_face (FT_Face         face,
//                        int             load_flags);
static int l_cairo_ft_font_face_create_for_ft_face(lua_State* L)
{
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public FT_Face
// cairo_ft_scaled_font_lock_face (cairo_scaled_font_t *scaled_font);
static int l_cairo_ft_scaled_font_lock_face(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}


// cairo_public void
// cairo_ft_scaled_font_unlock_face (cairo_scaled_font_t *scaled_font);
static int l_cairo_ft_scaled_font_unlock_face(lua_State* L)
{
    //cairo_scaled_font_t *scaled_font = get_cairo_scaled_font_t (L, 1);
    luaL_error(L, "not implemented!");
    return 0;
}

#endif /* CAIRO_HAS_FT_FONT */




///--------------------------------------------------------------------------
/// Registration
///--------------------------------------------------------------------------


static const struct luaL_Reg cairolib_f [] = {
    // not part of Cairo API
    {"CreateContext",                            l_CreateContext},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"CreatePrintingContext",                    l_CreatePrintingContext},
#endif

#if LUACAIRO_HAS_OO_INTERFACE
    {"ContextFromLUD",                           new_ContextFromLUD},
    {"SurfaceFromLUD",                           new_SurfaceFromLUD},
    {"PatternFromLUD",                           new_PatternFromLUD},
    {"FontOptionsFromLUD",                       new_FontOptionsFromLUD},
    {"ScaledFontFromLUD",                        new_ScaledFontFromLUD},
    {"FontFaceFromLUD",                          new_FontFaceFromLUD},
#endif

    // Cairo API
    {"version",                                  l_cairo_version},
    {"version_string",                           l_cairo_version_string},
    {"create",                                   l_cairo_create},
    {"reference",                                l_cairo_reference},
    {"destroy",                                  l_cairo_destroy},
    {"get_reference_count",                      l_cairo_get_reference_count},
    {"get_user_data",                            l_cairo_get_user_data},
    {"set_user_data",                            l_cairo_set_user_data},
    {"save",                                     l_cairo_save},
    {"restore",                                  l_cairo_restore},
    {"push_group",                               l_cairo_push_group},
    {"push_group_with_content",                  l_cairo_push_group_with_content},
    {"pop_group",                                l_cairo_pop_group},
    {"pop_group_to_source",                      l_cairo_pop_group_to_source},
    {"set_operator",                             l_cairo_set_operator},
    {"set_source",                               l_cairo_set_source},
    {"set_source_rgb",                           l_cairo_set_source_rgb},
    {"set_source_rgba",                          l_cairo_set_source_rgba},
    {"set_source_surface",                       l_cairo_set_source_surface},
    {"set_tolerance",                            l_cairo_set_tolerance},
    {"set_antialias",                            l_cairo_set_antialias},
    {"set_fill_rule",                            l_cairo_set_fill_rule},
    {"set_line_width",                           l_cairo_set_line_width},
    {"set_line_cap",                             l_cairo_set_line_cap},
    {"set_line_join",                            l_cairo_set_line_join},
    {"set_dash",                                 l_cairo_set_dash},
    {"set_miter_limit",                          l_cairo_set_miter_limit},
    {"translate",                                l_cairo_translate},
    {"scale",                                    l_cairo_scale},
    {"rotate",                                   l_cairo_rotate},
    {"transform",                                l_cairo_transform},
    {"set_matrix",                               l_cairo_set_matrix},
    {"identity_matrix",                          l_cairo_identity_matrix},
    {"user_to_device",                           l_cairo_user_to_device},
    {"user_to_device_distance",                  l_cairo_user_to_device_distance},
    {"device_to_user",                           l_cairo_device_to_user},
    {"device_to_user_distance",                  l_cairo_device_to_user_distance},
    {"new_path",                                 l_cairo_new_path},
    {"move_to",                                  l_cairo_move_to},
    {"new_sub_path",                             l_cairo_new_sub_path},
    {"line_to",                                  l_cairo_line_to},
    {"curve_to",                                 l_cairo_curve_to},
    {"arc",                                      l_cairo_arc},
    {"arc_negative",                             l_cairo_arc_negative},
    {"rel_move_to",                              l_cairo_rel_move_to},
    {"rel_line_to",                              l_cairo_rel_line_to},
    {"rel_curve_to",                             l_cairo_rel_curve_to},
    {"rectangle",                                l_cairo_rectangle},
    {"close_path",                               l_cairo_close_path},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"path_extents",                             l_cairo_path_extents},
#endif
    {"paint",                                    l_cairo_paint},
    {"paint_with_alpha",                         l_cairo_paint_with_alpha},
    {"mask",                                     l_cairo_mask},
    {"mask_surface",                             l_cairo_mask_surface},
    {"stroke",                                   l_cairo_stroke},
    {"stroke_preserve",                          l_cairo_stroke_preserve},
    {"fill",                                     l_cairo_fill},
    {"fill_preserve",                            l_cairo_fill_preserve},
    {"copy_page",                                l_cairo_copy_page},
    {"show_page",                                l_cairo_show_page},
    {"in_stroke",                                l_cairo_in_stroke},
    {"in_fill",                                  l_cairo_in_fill},
    {"stroke_extents",                           l_cairo_stroke_extents},
    {"fill_extents",                             l_cairo_fill_extents},
    {"reset_clip",                               l_cairo_reset_clip},
    {"clip",                                     l_cairo_clip},
    {"clip_preserve",                            l_cairo_clip_preserve},
    {"clip_extents",                             l_cairo_clip_extents},
    {"copy_clip_rectangle_list",                 l_cairo_copy_clip_rectangle_list},
    {"rectangle_list_destroy",                   l_cairo_rectangle_list_destroy},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"glyph_allocate",                           l_cairo_glyph_allocate},
    {"glyph_free",                               l_cairo_glyph_free},
    {"text_cluster_allocate",                    l_cairo_text_cluster_allocate},
    {"text_cluster_free",                        l_cairo_text_cluster_free},
#endif
    {"font_options_create",                      l_cairo_font_options_create},
    {"font_options_copy",                        l_cairo_font_options_copy},
    {"font_options_destroy",                     l_cairo_font_options_destroy},
    {"font_options_status",                      l_cairo_font_options_status},
    {"font_options_merge",                       l_cairo_font_options_merge},
    {"font_options_equal",                       l_cairo_font_options_equal},
    {"font_options_hash",                        l_cairo_font_options_hash},
    {"font_options_set_antialias",               l_cairo_font_options_set_antialias},
    {"font_options_get_antialias",               l_cairo_font_options_get_antialias},
    {"font_options_set_subpixel_order",          l_cairo_font_options_set_subpixel_order},
    {"font_options_get_subpixel_order",          l_cairo_font_options_get_subpixel_order},
    {"font_options_set_hint_style",              l_cairo_font_options_set_hint_style},
    {"font_options_get_hint_style",              l_cairo_font_options_get_hint_style},
    {"font_options_set_hint_metrics",            l_cairo_font_options_set_hint_metrics},
    {"font_options_get_hint_metrics",            l_cairo_font_options_get_hint_metrics},
    {"select_font_face",                         l_cairo_select_font_face},
    {"set_font_size",                            l_cairo_set_font_size},
    {"set_font_matrix",                          l_cairo_set_font_matrix},
    {"get_font_matrix",                          l_cairo_get_font_matrix},
    {"set_font_options",                         l_cairo_set_font_options},
    {"get_font_options",                         l_cairo_get_font_options},
    {"set_font_face",                            l_cairo_set_font_face},
    {"get_font_face",                            l_cairo_get_font_face},
    {"set_scaled_font",                          l_cairo_set_scaled_font},
    {"get_scaled_font",                          l_cairo_get_scaled_font},
    {"show_text",                                l_cairo_show_text},
    {"show_glyphs",                              l_cairo_show_glyphs},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"show_text_glyphs",                         l_cairo_show_text_glyphs},
#endif
    {"text_path",                                l_cairo_text_path},
    {"glyph_path",                               l_cairo_glyph_path},
    {"text_extents",                             l_cairo_text_extents},
    {"glyph_extents",                            l_cairo_glyph_extents},
    {"font_extents",                             l_cairo_font_extents},
    {"font_face_reference",                      l_cairo_font_face_reference},
    {"font_face_destroy",                        l_cairo_font_face_destroy},
    {"font_face_get_reference_count",            l_cairo_font_face_get_reference_count},
    {"font_face_status",                         l_cairo_font_face_status},
    {"font_face_get_type",                       l_cairo_font_face_get_type},
    {"font_face_get_user_data",                  l_cairo_font_face_get_user_data},
    {"font_face_set_user_data",                  l_cairo_font_face_set_user_data},
    {"scaled_font_create",                       l_cairo_scaled_font_create},
    {"scaled_font_reference",                    l_cairo_scaled_font_reference},
    {"scaled_font_destroy",                      l_cairo_scaled_font_destroy},
    {"scaled_font_get_reference_count",          l_cairo_scaled_font_get_reference_count},
    {"scaled_font_status",                       l_cairo_scaled_font_status},
    {"scaled_font_get_type",                     l_cairo_scaled_font_get_type},
    {"scaled_font_get_user_data",                l_cairo_scaled_font_get_user_data},
    {"scaled_font_set_user_data",                l_cairo_scaled_font_set_user_data},
    {"scaled_font_extents",                      l_cairo_scaled_font_extents},
    {"scaled_font_text_extents",                 l_cairo_scaled_font_text_extents},
    {"scaled_font_glyph_extents",                l_cairo_scaled_font_glyph_extents},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"scaled_font_text_to_glyphs",               l_cairo_scaled_font_text_to_glyphs},
#endif
    {"scaled_font_get_font_face",                l_cairo_scaled_font_get_font_face},
    {"scaled_font_get_font_matrix",              l_cairo_scaled_font_get_font_matrix},
    {"scaled_font_get_ctm",                      l_cairo_scaled_font_get_ctm},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"scaled_font_get_scale_matrix",             l_cairo_scaled_font_get_scale_matrix},
#endif
    {"scaled_font_get_font_options",             l_cairo_scaled_font_get_font_options},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"toy_font_face_create",                     l_cairo_toy_font_face_create},
    {"toy_font_face_get_family",                 l_cairo_toy_font_face_get_family},
    {"toy_font_face_get_slant",                  l_cairo_toy_font_face_get_slant},
    {"toy_font_face_get_weight",                 l_cairo_toy_font_face_get_weight},
    {"user_font_face_create",                    l_cairo_user_font_face_create},
    {"user_font_face_set_init_func",             l_cairo_user_font_face_set_init_func},
    {"user_font_face_set_render_glyph_func",     l_cairo_user_font_face_set_render_glyph_func},
    {"user_font_face_set_text_to_glyphs_func",   l_cairo_user_font_face_set_text_to_glyphs_func},
    {"user_font_face_set_unicode_to_glyph_func", l_cairo_user_font_face_set_unicode_to_glyph_func},
    {"user_font_face_get_init_func",             l_cairo_user_font_face_get_init_func},
    {"user_font_face_get_render_glyph_func",     l_cairo_user_font_face_get_render_glyph_func},
    {"user_font_face_get_text_to_glyphs_func",   l_cairo_user_font_face_get_text_to_glyphs_func},
    {"user_font_face_get_unicode_to_glyph_func", l_cairo_user_font_face_get_unicode_to_glyph_func},
#endif
    {"get_operator",                             l_cairo_get_operator},
    {"get_source",                               l_cairo_get_source},
    {"get_tolerance",                            l_cairo_get_tolerance},
    {"get_antialias",                            l_cairo_get_antialias},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"has_current_point",                        l_cairo_has_current_point},
#endif
    {"get_current_point",                        l_cairo_get_current_point},
    {"get_fill_rule",                            l_cairo_get_fill_rule},
    {"get_line_width",                           l_cairo_get_line_width},
    {"get_line_cap",                             l_cairo_get_line_cap},
    {"get_line_join",                            l_cairo_get_line_join},
    {"get_miter_limit",                          l_cairo_get_miter_limit},
    {"get_dash_count",                           l_cairo_get_dash_count},
    {"get_dash",                                 l_cairo_get_dash},
    {"get_matrix",                               l_cairo_get_matrix},
    {"get_target",                               l_cairo_get_target},
    {"get_group_target",                         l_cairo_get_group_target},
    {"copy_path",                                l_cairo_copy_path},
    {"copy_path_flat",                           l_cairo_copy_path_flat},
    {"append_path",                              l_cairo_append_path},
    {"path_destroy",                             l_cairo_path_destroy},
    {"status",                                   l_cairo_status},
    {"status_to_string",                         l_cairo_status_to_string},
    {"surface_create_similar",                   l_cairo_surface_create_similar},
    {"surface_reference",                        l_cairo_surface_reference},
    {"surface_finish",                           l_cairo_surface_finish},
    {"surface_destroy",                          l_cairo_surface_destroy},
    {"surface_get_reference_count",              l_cairo_surface_get_reference_count},
    {"surface_status",                           l_cairo_surface_status},
    {"surface_get_type",                         l_cairo_surface_get_type},
    {"surface_get_content",                      l_cairo_surface_get_content},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"surface_write_to_png",                     l_cairo_surface_write_to_png},
    {"surface_write_to_png_stream",              l_cairo_surface_write_to_png_stream},
#endif
    {"surface_get_user_data",                    l_cairo_surface_get_user_data},
    {"surface_set_user_data",                    l_cairo_surface_set_user_data},
    {"surface_get_font_options",                 l_cairo_surface_get_font_options},
    {"surface_flush",                            l_cairo_surface_flush},
    {"surface_mark_dirty",                       l_cairo_surface_mark_dirty},
    {"surface_mark_dirty_rectangle",             l_cairo_surface_mark_dirty_rectangle},
    {"surface_set_device_offset",                l_cairo_surface_set_device_offset},
    {"surface_get_device_offset",                l_cairo_surface_get_device_offset},
    {"surface_set_fallback_resolution",          l_cairo_surface_set_fallback_resolution},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"surface_get_fallback_resolution",          l_cairo_surface_get_fallback_resolution},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"surface_copy_page",                        l_cairo_surface_copy_page},
    {"surface_show_page",                        l_cairo_surface_show_page},
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    {"surface_has_show_text_glyphs",             l_cairo_surface_has_show_text_glyphs},
#endif
    {"image_surface_create",                     l_cairo_image_surface_create},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"format_stride_for_width",                  l_cairo_format_stride_for_width},
#endif
    {"image_surface_create_for_data",            l_cairo_image_surface_create_for_data},
    {"image_surface_get_data",                   l_cairo_image_surface_get_data},
    {"image_surface_get_format",                 l_cairo_image_surface_get_format},
    {"image_surface_get_width",                  l_cairo_image_surface_get_width},
    {"image_surface_get_height",                 l_cairo_image_surface_get_height},
    {"image_surface_get_stride",                 l_cairo_image_surface_get_stride},
#if CAIRO_HAS_PNG_FUNCTIONS
    {"image_surface_create_from_png",            l_cairo_image_surface_create_from_png},
    {"image_surface_create_from_png_stream",     l_cairo_image_surface_create_from_png_stream},
#endif
    {"pattern_create_rgb",                       l_cairo_pattern_create_rgb},
    {"pattern_create_rgba",                      l_cairo_pattern_create_rgba},
    {"pattern_create_for_surface",               l_cairo_pattern_create_for_surface},
    {"pattern_create_linear",                    l_cairo_pattern_create_linear},
    {"pattern_create_radial",                    l_cairo_pattern_create_radial},
    {"pattern_reference",                        l_cairo_pattern_reference},
    {"pattern_destroy",                          l_cairo_pattern_destroy},
    {"pattern_get_reference_count",              l_cairo_pattern_get_reference_count},
    {"pattern_status",                           l_cairo_pattern_status},
    {"pattern_get_user_data",                    l_cairo_pattern_get_user_data},
    {"pattern_set_user_data",                    l_cairo_pattern_set_user_data},
    {"pattern_get_type",                         l_cairo_pattern_get_type},
    {"pattern_add_color_stop_rgb",               l_cairo_pattern_add_color_stop_rgb},
    {"pattern_add_color_stop_rgba",              l_cairo_pattern_add_color_stop_rgba},
    {"pattern_set_matrix",                       l_cairo_pattern_set_matrix},
    {"pattern_get_matrix",                       l_cairo_pattern_get_matrix},
    {"pattern_set_extend",                       l_cairo_pattern_set_extend},
    {"pattern_get_extend",                       l_cairo_pattern_get_extend},
    {"pattern_set_filter",                       l_cairo_pattern_set_filter},
    {"pattern_get_filter",                       l_cairo_pattern_get_filter},
    {"pattern_get_rgba",                         l_cairo_pattern_get_rgba},
    {"pattern_get_surface",                      l_cairo_pattern_get_surface},
    {"pattern_get_color_stop_rgba",              l_cairo_pattern_get_color_stop_rgba},
    {"pattern_get_color_stop_count",             l_cairo_pattern_get_color_stop_count},
    {"pattern_get_linear_points",                l_cairo_pattern_get_linear_points},
    {"pattern_get_radial_circles",               l_cairo_pattern_get_radial_circles},
    {"matrix_init",                              l_cairo_matrix_init},
    {"matrix_init_identity",                     l_cairo_matrix_init_identity},
    {"matrix_init_translate",                    l_cairo_matrix_init_translate},
    {"matrix_init_scale",                        l_cairo_matrix_init_scale},
    {"matrix_init_rotate",                       l_cairo_matrix_init_rotate},
    {"matrix_translate",                         l_cairo_matrix_translate},
    {"matrix_scale",                             l_cairo_matrix_scale},
    {"matrix_rotate",                            l_cairo_matrix_rotate},
    {"matrix_invert",                            l_cairo_matrix_invert},
    {"matrix_multiply",                          l_cairo_matrix_multiply},
    {"matrix_transform_distance",                l_cairo_matrix_transform_distance},
    {"matrix_transform_point",                   l_cairo_matrix_transform_point},
    {"debug_reset_static_data",                  l_cairo_debug_reset_static_data},

#if CAIRO_HAS_SVG_SURFACE
    {"svg_surface_create",                       l_cairo_svg_surface_create},
    {"svg_surface_create_for_stream",            l_cairo_svg_surface_create_for_stream},
    {"svg_surface_restrict_to_version",          l_cairo_svg_surface_restrict_to_version},
    {"svg_get_versions",                         l_cairo_svg_get_versions},
    {"svg_version_to_string",                    l_cairo_svg_version_to_string},
#endif

#if CAIRO_HAS_PDF_SURFACE
    {"pdf_surface_create",                       l_cairo_pdf_surface_create},
    {"pdf_surface_create_for_stream",            l_cairo_pdf_surface_create_for_stream},
    {"pdf_surface_set_size",                     l_cairo_pdf_surface_set_size},
#endif

#if CAIRO_HAS_PS_SURFACE
    {"ps_surface_create",                        l_cairo_ps_surface_create},
    {"ps_surface_create_for_stream",             l_cairo_ps_surface_create_for_stream},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"ps_surface_restrict_to_level",             l_cairo_ps_surface_restrict_to_level},
    {"ps_get_levels",                            l_cairo_ps_get_levels},
    {"ps_level_to_string",                       l_cairo_ps_level_to_string},
    {"ps_surface_set_eps",                       l_cairo_ps_surface_set_eps},
    {"ps_surface_get_eps",                       l_cairo_ps_surface_get_eps},
#endif
    {"ps_surface_set_size",                      l_cairo_ps_surface_set_size},
    {"ps_surface_dsc_comment",                   l_cairo_ps_surface_dsc_comment},
    {"ps_surface_dsc_begin_setup",               l_cairo_ps_surface_dsc_begin_setup},
    {"ps_surface_dsc_begin_page_setup",          l_cairo_ps_surface_dsc_begin_page_setup},
#endif

#if CAIRO_HAS_XCB_SURFACE
    {"xcb_surface_create",                       l_cairo_xcb_surface_create},
    {"xcb_surface_create_for_bitmap",            l_cairo_xcb_surface_create_for_bitmap},
    {"xcb_surface_set_size",                     l_cairo_xcb_surface_set_size},
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
    {"xcb_surface_create_with_xrender_format",   l_cairo_xcb_surface_create_with_xrender_format},
#endif
#endif

#if CAIRO_HAS_XLIB_SURFACE
    {"xlib_surface_create",                      l_cairo_xlib_surface_create},
    {"xlib_surface_create_for_bitmap",           l_cairo_xlib_surface_create_for_bitmap},
    {"xlib_surface_set_size",                    l_cairo_xlib_surface_set_size},
    {"xlib_surface_set_drawable",                l_cairo_xlib_surface_set_drawable},
    {"xlib_surface_get_display",                 l_cairo_xlib_surface_get_display},
    {"xlib_surface_get_drawable",                l_cairo_xlib_surface_get_drawable},
    {"xlib_surface_get_screen",                  l_cairo_xlib_surface_get_screen},
    {"xlib_surface_get_visual",                  l_cairo_xlib_surface_get_visual},
    {"xlib_surface_get_depth",                   l_cairo_xlib_surface_get_depth},
    {"xlib_surface_get_width",                   l_cairo_xlib_surface_get_width},
    {"xlib_surface_get_height",                  l_cairo_xlib_surface_get_height},
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
    {"xlib_surface_create_with_xrender_format",  l_cairo_xlib_surface_create_with_xrender_format},
#endif
#endif

#if CAIRO_HAS_WIN32_SURFACE
    {"win32_surface_create",                     l_cairo_win32_surface_create},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"win32_printing_surface_create",            l_cairo_win32_printing_surface_create},
#endif
    {"win32_surface_create_with_ddb",            l_cairo_win32_surface_create_with_ddb},
    {"win32_surface_create_with_dib",            l_cairo_win32_surface_create_with_dib},
    {"win32_surface_get_dc",                     l_cairo_win32_surface_get_dc},
    {"win32_surface_get_image",                  l_cairo_win32_surface_get_image},
#if CAIRO_HAS_WIN32_FONT
    {"win32_font_face_create_for_logfontw",      l_cairo_win32_font_face_create_for_logfontw},
    {"win32_font_face_create_for_hfont",         l_cairo_win32_font_face_create_for_hfont},
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"win32_font_face_create_for_logfontw_hfont",l_cairo_win32_font_face_create_for_logfontw_hfont},
#endif
    {"win32_scaled_font_select_font",            l_cairo_win32_scaled_font_select_font},
    {"win32_scaled_font_done_font",              l_cairo_win32_scaled_font_done_font},
    {"win32_scaled_font_get_metrics_factor",     l_cairo_win32_scaled_font_get_metrics_factor},
    {"win32_scaled_font_get_logical_to_device",  l_cairo_win32_scaled_font_get_logical_to_device},
    {"win32_scaled_font_get_device_to_logical",  l_cairo_win32_scaled_font_get_device_to_logical},
#endif
#endif

#if CAIRO_HAS_BEOS_SURFACE
    {"beos_surface_create",                      l_cairo_beos_surface_create},
    {"beos_surface_create_for_bitmap",           l_cairo_beos_surface_create_for_bitmap},
#endif

#if CAIRO_HAS_DIRECTFB_SURFACE
    {"directfb_surface_create",                  l_cairo_directfb_surface_create},
#endif

#if CAIRO_HAS_OS2_SURFACE
    {"os2_init",                                 l_cairo_os2_init},
    {"os2_fini",                                 l_cairo_os2_fini},
    {"os2_surface_create",                       l_cairo_os2_surface_create},
    {"os2_surface_set_hwnd",                     l_cairo_os2_surface_set_hwnd},
    {"os2_surface_set_size",                     l_cairo_os2_surface_set_size},
    {"os2_surface_refresh_window",               l_cairo_os2_surface_refresh_window},
    {"os2_surface_set_manual_window_refresh",    l_cairo_os2_surface_set_manual_window_refresh},
    {"os2_surface_get_manual_window_refresh",    l_cairo_os2_surface_get_manual_window_refresh},
#endif

#if CAIRO_HAS_GLITZ_SURFACE
    {"glitz_surface_create",                     l_cairo_glitz_surface_create},
#endif

#if CAIRO_HAS_QUARTZ_SURFACE
    {"quartz_surface_create",                    l_cairo_quartz_surface_create},
    {"quartz_surface_create_for_cg_context",     l_cairo_quartz_surface_create_for_cg_context},
    {"quartz_surface_get_cg_context",            l_cairo_quartz_surface_get_cg_context},
#ifdef CAIRO_HAS_QUARTZ_FONT
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    {"quartz_font_face_create_for_cgfont",       l_cairo_quartz_font_face_create_for_cgfont},
    {"quartz_font_face_create_for_atsu_font_id", l_cairo_quartz_font_face_create_for_atsu_font_id},
#endif
#endif
#endif

#if CAIRO_HAS_ATSUI_FONT
#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 6, 0)
    {"atsui_font_face_create_for_atsu_font_id",  l_cairo_atsui_font_face_create_for_atsu_font_id},
#endif
#endif

#if CAIRO_HAS_FT_FONT
    {"ft_font_face_create_for_pattern",          l_cairo_ft_font_face_create_for_pattern},
    {"ft_font_options_substitute",               l_cairo_ft_font_options_substitute},
    {"ft_font_face_create_for_ft_face",          l_cairo_ft_font_face_create_for_ft_face},
    {"ft_scaled_font_lock_face",                 l_cairo_ft_scaled_font_lock_face},
    {"ft_scaled_font_unlock_face",               l_cairo_ft_scaled_font_unlock_face},
#endif

	{NULL, NULL} /* sentinel */
};



#ifndef CAIRO_HAS_WIN32_FONT
    #define CAIRO_HAS_WIN32_FONT 0
#endif
#ifndef CAIRO_HAS_WIN32_SURFACE
    #define CAIRO_HAS_WIN32_SURFACE 0
#endif
#ifndef CAIRO_HAS_SVG_SURFACE
    #define CAIRO_HAS_SVG_SURFACE 0
#endif
#ifndef CAIRO_HAS_PDF_SURFACE
    #define CAIRO_HAS_PDF_SURFACE 0
#endif
#ifndef CAIRO_HAS_PS_SURFACE
    #define CAIRO_HAS_PS_SURFACE 0
#endif
#ifndef CAIRO_HAS_PNG_FUNCTIONS
    #define CAIRO_HAS_PNG_FUNCTIONS 0
#endif
#ifndef CAIRO_HAS_FT_FONT
    #define CAIRO_HAS_FT_FONT 0
#endif
#ifndef CAIRO_HAS_XLIB_XRENDER_SURFACE
    #define CAIRO_HAS_XLIB_XRENDER_SURFACE 0
#endif
#ifndef CAIRO_HAS_XLIB_SURFACE
    #define CAIRO_HAS_XLIB_SURFACE 0
#endif
#ifndef CAIRO_HAS_QUARTZ_FONT
    #define CAIRO_HAS_QUARTZ_FONT 0
#endif
#ifndef CAIRO_HAS_XCB_SURFACE
    #define CAIRO_HAS_XCB_SURFACE 0
#endif
#ifndef CAIRO_HAS_QUARTZ_SURFACE
    #define CAIRO_HAS_QUARTZ_SURFACE 0
#endif
#ifndef CAIRO_HAS_QUARTZ_IMAGE_SURFACE
    #define CAIRO_HAS_QUARTZ_IMAGE_SURFACE 0
#endif
#ifndef CAIRO_HAS_OS2_SURFACE
    #define CAIRO_HAS_OS2_SURFACE 0
#endif
#ifndef CAIRO_HAS_BEOS_SURFACE
    #define CAIRO_HAS_BEOS_SURFACE 0
#endif
#ifndef CAIRO_HAS_GLITZ_SURFACE
    #define CAIRO_HAS_GLITZ_SURFACE 0
#endif
#ifndef CAIRO_HAS_DIRECTFB_SURFACE
    #define CAIRO_HAS_DIRECTFB_SURFACE 0
#endif
#ifndef CAIRO_HAS_TEST_SURFACES
    #define CAIRO_HAS_TEST_SURFACES 0
#endif
#ifndef CAIRO_HAS_IMAGE_SURFACE
    #define CAIRO_HAS_IMAGE_SURFACE 0
#endif
#ifndef CAIRO_HAS_USER_FONT
    #define CAIRO_HAS_USER_FONT 0
#endif


int luaopen_lcairo(lua_State* L)
{
#if LUACAIRO_HAS_OO_INTERFACE
    luaopen_lcontext (L);
    luaopen_lsurface (L);
    luaopen_lpattern (L);
    luaopen_lfontoptions (L);
    luaopen_lscaledfont (L);
    luaopen_lfontface (L);
#endif

    luaopen_lmatrix (L);
    luaopen_ltextextents (L);
    luaopen_lfontextents (L);

    // register functions
    luaL_register(L, LCAIRO, cairolib_f);

    // register constants
    //
    // trick by: Jerome Vuarand (jerome.vuarand@ubisoft.com)
    // local cairo = require 'lcario'
    // local CAIRO = cairo
    // cairo.set_operator (cr, CAIRO.OPERATOR_ADD)
    //

    set_strfield(L, "VERSION_STRING",                CAIRO_VERSION_STRING);
    set_intfield(L, "VERSION_MAJOR",                 CAIRO_VERSION_MAJOR);
    set_intfield(L, "VERSION_MINOR",                 CAIRO_VERSION_MINOR);
    set_intfield(L, "VERSION_MICRO",                 CAIRO_VERSION_MICRO);
    set_intfield(L, "VERSION",                       CAIRO_VERSION);

    set_intfield(L, "HAS_WIN32_FONT",                CAIRO_HAS_WIN32_FONT);
    set_intfield(L, "HAS_WIN32_SURFACE",             CAIRO_HAS_WIN32_SURFACE);
    set_intfield(L, "HAS_SVG_SURFACE",               CAIRO_HAS_SVG_SURFACE);
    set_intfield(L, "HAS_PDF_SURFACE",               CAIRO_HAS_PDF_SURFACE);
    set_intfield(L, "HAS_PS_SURFACE",                CAIRO_HAS_PS_SURFACE);
    set_intfield(L, "HAS_PNG_FUNCTIONS",             CAIRO_HAS_PNG_FUNCTIONS);
    set_intfield(L, "HAS_FT_FONT",                   CAIRO_HAS_FT_FONT);
    set_intfield(L, "HAS_XLIB_XRENDER_SURFACE",      CAIRO_HAS_XLIB_XRENDER_SURFACE);
    set_intfield(L, "HAS_XLIB_SURFACE",              CAIRO_HAS_XLIB_SURFACE);
    set_intfield(L, "HAS_QUARTZ_FONT",               CAIRO_HAS_QUARTZ_FONT);
    set_intfield(L, "HAS_XCB_SURFACE",               CAIRO_HAS_XCB_SURFACE);
    set_intfield(L, "HAS_QUARTZ_SURFACE",            CAIRO_HAS_QUARTZ_SURFACE);
    set_intfield(L, "HAS_QUARTZ_IMAGE_SURFACE",      CAIRO_HAS_QUARTZ_IMAGE_SURFACE);
    set_intfield(L, "HAS_OS2_SURFACE",               CAIRO_HAS_OS2_SURFACE);
    set_intfield(L, "HAS_BEOS_SURFACE",              CAIRO_HAS_BEOS_SURFACE);
    set_intfield(L, "HAS_GLITZ_SURFACE",             CAIRO_HAS_GLITZ_SURFACE);
    set_intfield(L, "HAS_DIRECTFB_SURFACE",          CAIRO_HAS_DIRECTFB_SURFACE);
    set_intfield(L, "HAS_TEST_SURFACES",             CAIRO_HAS_TEST_SURFACES);
    set_intfield(L, "HAS_IMAGE_SURFACE",             CAIRO_HAS_IMAGE_SURFACE);
    set_intfield(L, "HAS_USER_FONT",                 CAIRO_HAS_USER_FONT);

    set_intfield(L, "STATUS_SUCCESS",                CAIRO_STATUS_SUCCESS);
    set_intfield(L, "STATUS_NO_MEMORY",              CAIRO_STATUS_NO_MEMORY);
    set_intfield(L, "STATUS_INVALID_RESTORE",        CAIRO_STATUS_INVALID_RESTORE);
    set_intfield(L, "STATUS_INVALID_POP_GROUP",      CAIRO_STATUS_INVALID_POP_GROUP);
    set_intfield(L, "STATUS_NO_CURRENT_POINT",       CAIRO_STATUS_NO_CURRENT_POINT);
    set_intfield(L, "STATUS_INVALID_MATRIX",         CAIRO_STATUS_INVALID_MATRIX);
    set_intfield(L, "STATUS_INVALID_STATUS",         CAIRO_STATUS_INVALID_STATUS);
    set_intfield(L, "STATUS_NULL_POINTER",           CAIRO_STATUS_NULL_POINTER);
    set_intfield(L, "STATUS_INVALID_STRING",         CAIRO_STATUS_INVALID_STRING);
    set_intfield(L, "STATUS_INVALID_PATH_DATA",      CAIRO_STATUS_INVALID_PATH_DATA);
    set_intfield(L, "STATUS_READ_ERROR",             CAIRO_STATUS_READ_ERROR);
    set_intfield(L, "STATUS_WRITE_ERROR",            CAIRO_STATUS_WRITE_ERROR);
    set_intfield(L, "STATUS_SURFACE_FINISHED",       CAIRO_STATUS_SURFACE_FINISHED);
    set_intfield(L, "STATUS_SURFACE_TYPE_MISMATCH",  CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
    set_intfield(L, "STATUS_PATTERN_TYPE_MISMATCH",  CAIRO_STATUS_PATTERN_TYPE_MISMATCH);
    set_intfield(L, "STATUS_INVALID_CONTENT",        CAIRO_STATUS_INVALID_CONTENT);
    set_intfield(L, "STATUS_INVALID_FORMAT",         CAIRO_STATUS_INVALID_FORMAT);
    set_intfield(L, "STATUS_INVALID_VISUAL",         CAIRO_STATUS_INVALID_VISUAL);
    set_intfield(L, "STATUS_FILE_NOT_FOUND",         CAIRO_STATUS_FILE_NOT_FOUND);
    set_intfield(L, "STATUS_INVALID_DASH",           CAIRO_STATUS_INVALID_DASH);
    set_intfield(L, "STATUS_INVALID_DSC_COMMENT",    CAIRO_STATUS_INVALID_DSC_COMMENT);
    set_intfield(L, "STATUS_INVALID_INDEX",          CAIRO_STATUS_INVALID_INDEX);
    set_intfield(L, "STATUS_CLIP_NOT_REPRESENTABLE", CAIRO_STATUS_CLIP_NOT_REPRESENTABLE);
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    set_intfield(L, "STATUS_TEMP_FILE_ERROR",        CAIRO_STATUS_CLIP_NOT_REPRESENTABLE);
    set_intfield(L, "STATUS_INVALID_STRIDE",         CAIRO_STATUS_INVALID_STRIDE);
#endif
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
    set_intfield(L, "STATUS_INVALID_STRIDE",         CAIRO_STATUS_INVALID_STRIDE);
    set_intfield(L, "STATUS_FONT_TYPE_MISMATCH",     CAIRO_STATUS_FONT_TYPE_MISMATCH);
    set_intfield(L, "STATUS_USER_FONT_IMMUTABLE",    CAIRO_STATUS_USER_FONT_IMMUTABLE);
    set_intfield(L, "STATUS_USER_FONT_ERROR",        CAIRO_STATUS_USER_FONT_ERROR);
    set_intfield(L, "STATUS_NEGATIVE_COUNT",         CAIRO_STATUS_NEGATIVE_COUNT);
    set_intfield(L, "STATUS_INVALID_CLUSTERS",       CAIRO_STATUS_INVALID_CLUSTERS);
    set_intfield(L, "STATUS_INVALID_SLANT",          CAIRO_STATUS_INVALID_SLANT);
    set_intfield(L, "STATUS_INVALID_WEIGHT",         CAIRO_STATUS_INVALID_WEIGHT);
#endif

    set_intfield(L, "CONTENT_COLOR",                 CAIRO_CONTENT_COLOR);
    set_intfield(L, "CONTENT_ALPHA",                 CAIRO_CONTENT_ALPHA);
    set_intfield(L, "CONTENT_COLOR_ALPHA",           CAIRO_CONTENT_COLOR_ALPHA);

    set_intfield(L, "OPERATOR_CLEAR",                CAIRO_OPERATOR_CLEAR);
    set_intfield(L, "OPERATOR_SOURCE",               CAIRO_OPERATOR_SOURCE);
    set_intfield(L, "OPERATOR_OVER",                 CAIRO_OPERATOR_OVER);
    set_intfield(L, "OPERATOR_IN",                   CAIRO_OPERATOR_IN);
    set_intfield(L, "OPERATOR_OUT",                  CAIRO_OPERATOR_OUT);
    set_intfield(L, "OPERATOR_ATOP",                 CAIRO_OPERATOR_ATOP);
    set_intfield(L, "OPERATOR_DEST",                 CAIRO_OPERATOR_DEST);
    set_intfield(L, "OPERATOR_DEST_OVER",            CAIRO_OPERATOR_DEST_OVER);
    set_intfield(L, "OPERATOR_DEST_IN",              CAIRO_OPERATOR_DEST_IN);
    set_intfield(L, "OPERATOR_DEST_OUT",             CAIRO_OPERATOR_DEST_OUT);
    set_intfield(L, "OPERATOR_DEST_ATOP",            CAIRO_OPERATOR_DEST_ATOP);
    set_intfield(L, "OPERATOR_XOR",                  CAIRO_OPERATOR_XOR);
    set_intfield(L, "OPERATOR_ADD",                  CAIRO_OPERATOR_ADD);
    set_intfield(L, "OPERATOR_SATURATE",             CAIRO_OPERATOR_SATURATE);

    set_intfield(L, "ANTIALIAS_DEFAULT",             CAIRO_ANTIALIAS_DEFAULT);
    set_intfield(L, "ANTIALIAS_NONE",                CAIRO_ANTIALIAS_NONE);
    set_intfield(L, "ANTIALIAS_GRAY",                CAIRO_ANTIALIAS_GRAY);
    set_intfield(L, "ANTIALIAS_SUBPIXEL",            CAIRO_ANTIALIAS_SUBPIXEL);

    set_intfield(L, "FILL_RULE_WINDING",             CAIRO_FILL_RULE_WINDING);
    set_intfield(L, "FILL_RULE_EVEN_ODD",            CAIRO_FILL_RULE_EVEN_ODD);

    set_intfield(L, "LINE_CAP_BUTT",                 CAIRO_LINE_CAP_BUTT);
    set_intfield(L, "LINE_CAP_ROUND",                CAIRO_LINE_CAP_ROUND);
    set_intfield(L, "LINE_CAP_SQUARE",               CAIRO_LINE_CAP_SQUARE);
    set_intfield(L, "LINE_JOIN_MITER",               CAIRO_LINE_JOIN_MITER);
    set_intfield(L, "LINE_JOIN_ROUND",               CAIRO_LINE_JOIN_ROUND);
    set_intfield(L, "LINE_JOIN_BEVEL",               CAIRO_LINE_JOIN_BEVEL);

    set_intfield(L, "FONT_SLANT_NORMAL",             CAIRO_FONT_SLANT_NORMAL);
    set_intfield(L, "FONT_SLANT_ITALIC",             CAIRO_FONT_SLANT_ITALIC);
    set_intfield(L, "FONT_SLANT_OBLIQUE",            CAIRO_FONT_SLANT_OBLIQUE);
    set_intfield(L, "FONT_WEIGHT_NORMAL",            CAIRO_FONT_WEIGHT_NORMAL);
    set_intfield(L, "FONT_WEIGHT_BOLD",              CAIRO_FONT_WEIGHT_BOLD);

    set_intfield(L, "SUBPIXEL_ORDER_DEFAULT",        CAIRO_SUBPIXEL_ORDER_DEFAULT);
    set_intfield(L, "SUBPIXEL_ORDER_RGB",            CAIRO_SUBPIXEL_ORDER_RGB);
    set_intfield(L, "SUBPIXEL_ORDER_BGR",            CAIRO_SUBPIXEL_ORDER_BGR);
    set_intfield(L, "SUBPIXEL_ORDER_VRGB",           CAIRO_SUBPIXEL_ORDER_VRGB);
    set_intfield(L, "SUBPIXEL_ORDER_VBGR",           CAIRO_SUBPIXEL_ORDER_VBGR);

    set_intfield(L, "HINT_STYLE_DEFAULT",            CAIRO_HINT_STYLE_DEFAULT);
    set_intfield(L, "HINT_STYLE_NONE",               CAIRO_HINT_STYLE_NONE);
    set_intfield(L, "HINT_STYLE_SLIGHT",             CAIRO_HINT_STYLE_SLIGHT);
    set_intfield(L, "HINT_STYLE_MEDIUM",             CAIRO_HINT_STYLE_MEDIUM);
    set_intfield(L, "HINT_STYLE_FULL",               CAIRO_HINT_STYLE_FULL);
    set_intfield(L, "HINT_METRICS_DEFAULT",          CAIRO_HINT_METRICS_DEFAULT);
    set_intfield(L, "HINT_METRICS_OFF",              CAIRO_HINT_METRICS_OFF);
    set_intfield(L, "HINT_METRICS_ON",               CAIRO_HINT_METRICS_ON);

    set_intfield(L, "FONT_TYPE_TOY",                 CAIRO_FONT_TYPE_TOY);
    set_intfield(L, "FONT_TYPE_FT",                  CAIRO_FONT_TYPE_FT);
    set_intfield(L, "FONT_TYPE_WIN32",               CAIRO_FONT_TYPE_WIN32);
#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1, 6, 0)
    set_intfield(L, "FONT_TYPE_ATSUI",               CAIRO_FONT_TYPE_ATSUI);
#else
    set_intfield(L, "FONT_TYPE_QUARTZ",              CAIRO_FONT_TYPE_QUARTZ);
#endif
#if CAIRO_VERSION > CAIRO_VERSION_ENCODE(1, 8, 0)
    set_intfield(L, "FONT_TYPE_USER",                CAIRO_FONT_TYPE_USER);
#endif

    set_intfield(L, "PATH_MOVE_TO",                  CAIRO_PATH_MOVE_TO);
    set_intfield(L, "PATH_LINE_TO",                  CAIRO_PATH_LINE_TO);
    set_intfield(L, "PATH_CURVE_TO",                 CAIRO_PATH_CURVE_TO);
    set_intfield(L, "PATH_CLOSE_PATH",               CAIRO_PATH_CLOSE_PATH);

    set_intfield(L, "SURFACE_TYPE_IMAGE",            CAIRO_SURFACE_TYPE_IMAGE);
    set_intfield(L, "SURFACE_TYPE_PDF",              CAIRO_SURFACE_TYPE_PDF);
    set_intfield(L, "SURFACE_TYPE_PS",               CAIRO_SURFACE_TYPE_PS);
    set_intfield(L, "SURFACE_TYPE_XLIB",             CAIRO_SURFACE_TYPE_XLIB);
    set_intfield(L, "SURFACE_TYPE_XCB",              CAIRO_SURFACE_TYPE_XCB);
    set_intfield(L, "SURFACE_TYPE_GLITZ",            CAIRO_SURFACE_TYPE_GLITZ);
    set_intfield(L, "SURFACE_TYPE_QUARTZ",           CAIRO_SURFACE_TYPE_QUARTZ);
    set_intfield(L, "SURFACE_TYPE_WIN32",            CAIRO_SURFACE_TYPE_WIN32);
    set_intfield(L, "SURFACE_TYPE_BEOS",             CAIRO_SURFACE_TYPE_BEOS);
    set_intfield(L, "SURFACE_TYPE_DIRECTFB",         CAIRO_SURFACE_TYPE_DIRECTFB);
    set_intfield(L, "SURFACE_TYPE_SVG",              CAIRO_SURFACE_TYPE_SVG);
    set_intfield(L, "SURFACE_TYPE_OS2",              CAIRO_SURFACE_TYPE_OS2);
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
    set_intfield(L, "SURFACE_TYPE_WIN32_PRINTING",   CAIRO_SURFACE_TYPE_WIN32_PRINTING);
    set_intfield(L, "SURFACE_TYPE_QUARTZ_IMAGE",     CAIRO_SURFACE_TYPE_QUARTZ_IMAGE);
#endif

    set_intfield(L, "FORMAT_ARGB32",                 CAIRO_FORMAT_ARGB32);
    set_intfield(L, "FORMAT_RGB24",                  CAIRO_FORMAT_RGB24);
    set_intfield(L, "FORMAT_A8",                     CAIRO_FORMAT_A8);
    set_intfield(L, "FORMAT_A1",                     CAIRO_FORMAT_A1);

    set_intfield(L, "PATTERN_TYPE_SOLID",            CAIRO_PATTERN_TYPE_SOLID);
    set_intfield(L, "PATTERN_TYPE_SURFACE",          CAIRO_PATTERN_TYPE_SURFACE);
    set_intfield(L, "PATTERN_TYPE_LINEAR",           CAIRO_PATTERN_TYPE_LINEAR);
    set_intfield(L, "PATTERN_TYPE_RADIAL",           CAIRO_PATTERN_TYPE_RADIAL);

    set_intfield(L, "EXTEND_NONE",                   CAIRO_EXTEND_NONE);
    set_intfield(L, "EXTEND_REPEAT",                 CAIRO_EXTEND_REPEAT);
    set_intfield(L, "EXTEND_REFLECT",                CAIRO_EXTEND_REFLECT);
    set_intfield(L, "EXTEND_PAD",                    CAIRO_EXTEND_PAD);

    set_intfield(L, "FILTER_FAST",                   CAIRO_FILTER_FAST);
    set_intfield(L, "FILTER_GOOD",                   CAIRO_FILTER_GOOD);
    set_intfield(L, "FILTER_BEST",                   CAIRO_FILTER_BEST);
    set_intfield(L, "FILTER_NEAREST",                CAIRO_FILTER_NEAREST);
    set_intfield(L, "FILTER_BILINEAR",               CAIRO_FILTER_BILINEAR);
    set_intfield(L, "FILTER_GAUSSIAN",               CAIRO_FILTER_GAUSSIAN);

    return 1;
}



