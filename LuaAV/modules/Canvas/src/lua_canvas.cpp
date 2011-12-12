#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include <stdlib.h>


#ifdef __cplusplus
}
#endif

#include "lua_glue.h"
#include "lua_array.h"
#include "Canvas.h"


int lua_canvas_clear(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	float color[4];
	if(lua::to_vec<float>(L, 2, 4, color)) {
		s->clear(color[0], color[1], color[2], color[3]);
	}
	else {
		luaL_error(L, "Canvas.clear: invalid arguments");
	}	
	return 0;
}

int lua_canvas_line_width(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	if(lua::is<float>(L, 2)) {
		s->line_width(lua::to<float>(L, 2));
	}
	else {
		lua::push<float>(L, s->get_line_width());
		return 1;
	}
	return 0;
}

int lua_canvas_stroke_color(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	float color[4];
	if(lua::to_vec<float>(L, 2, 4, color)) {
		s->stroke_color(color[0], color[1], color[2], color[3]);
	}
	else {
		s->get_stroke_color(color[0], color[1], color[2], color[3]);
		lua::push_vec_t<float>(L, 4, color);
		return 1;
	}
	return 0;
}

int lua_canvas_fill_color(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	float color[4];
	if(lua::to_vec<float>(L, 2, 4, color)) {
		s->fill_color(color[0], color[1], color[2], color[3]);
	}
	else {
		s->get_fill_color(color[0], color[1], color[2], color[3]);
		lua::push_vec_t<float>(L, 4, color);
		return 1;
	}
	return 0;
}

int lua_canvas_stroke(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	s->stroke();
	return 0;
}

int lua_canvas_fill(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	s->fill();
	return 0;
}

int lua_canvas_move_to(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	Canvas::Point p(0, 0);
	if(lua::to_vec<float>(L, 2, 2, &p.x)) {
		s->move_to(p);
	}
	else {
		luaL_error(L, "Canvas.move_to: invalid arguments");
	}
	return 0;
}

int lua_canvas_line_to(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	Canvas::Point p(0, 0);
	if(lua::to_vec<float>(L, 2, 2, &p.x)) {
		s->line_to(p);
	}
	else {
		luaL_error(L, "Canvas.line_to: invalid arguments");
	}
	return 0;
}

int lua_canvas_rect(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	Canvas::Rect r(0, 0, 10, 10);
	if(lua::to_vec<float>(L, 2, 4, &r.x)) {
		s->rect(r);
	}
	else {
		luaL_error(L, "Canvas.rect: invalid arguments");
	}
	return 0;
}

int lua_canvas_oval(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	Canvas::Rect r(0, 0, 10, 10);
	if(lua::to_vec<float>(L, 2, 4, &r.x)) {
		s->oval(r);
	}
	else {
		luaL_error(L, "Canvas.oval: invalid arguments");
	}
	return 0;
}

int lua_canvas_set_font(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	if(lua::is<const char *>(L, 2) && lua::is<float>(L, 3)) {
		const char *name = lua::to<const char *>(L, 2);
		float size = lua::to<float>(L, 3);
		s->set_font(name, size);
	}
	else {
		luaL_error(L, "Canvas.set_font: invalid arguments");
	}
	return 0;
}

int lua_canvas_show_text(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	Canvas::Point p(0, 0);
	if(
		lua::to_vec<float>(L, 2, 2, &p.x) &&
		lua::is<const char *>(L, 3)
	) {
		const char *text = lua::to<const char *>(L, 3);
		s->show_text(p, text);
	}
	else {
		luaL_error(L, "Canvas.show_text: invalid arguments");
	}
	return 0;
}

int lua_canvas_text_size(lua_State *L) {
	Canvas *s = Glue<Canvas>::checkto(L, 1);
	if(lua::is<const char *>(L, 2)) {
		const char *text = lua::to<const char *>(L, 2);
		Canvas::Point p = s->text_size(text);
		lua::push_vec_t<float>(L, 2, &p.x);
	}
	else {
		luaL_error(L, "Canvas.text_size: invalid arguments");
	}
	return 1;
}


template<> const char * Glue<Canvas>::usr_name() { return "Canvas"; }
template<> bool Glue<Canvas>::usr_has_index() { return true; }

template<> void Glue<Canvas>::usr_push(lua_State * L, Canvas * u) {
	Glue<Canvas>::usr_attr_push(L);
}

template<> Canvas * Glue<Canvas>::usr_new(lua_State * L) {
	Canvas *canvas = 0;
	if(lua_type(L, 1) == LUA_TSTRING) {
		const char *filename = lua::to<const char *>(L, 1);
		Canvas::Rect rect(0, 0, 512, 512);
		lua::to_vec_t<float>(L, 2, 4, &rect.x);
		
		canvas = Canvas::create(filename, rect);
	}
	return canvas;
}

template<> void Glue<Canvas>::usr_index(lua_State * L, Canvas * u) {
	Glue<Canvas>::usr_attr_index(L);
}

template<> void Glue<Canvas>::usr_newindex(lua_State * L, Canvas * u) {
	Glue<Canvas>::usr_attr_newindex(L);
}

template<> int Glue<Canvas>::usr_tostring(lua_State * L, Canvas * u) {
	lua_pushfstring(L, "%s: %p", Glue<Canvas>::usr_name(), u); 
	return 1;
}

template<> void Glue<Canvas>::usr_gc(lua_State * L, Canvas * u) { 
	delete u;
}

template<> void Glue<Canvas>::usr_mt(lua_State * L) {
	static luaL_reg methods[] = {
		{ "clear", lua_canvas_clear },
		{ "stroke", lua_canvas_stroke },
		{ "fill", lua_canvas_fill },
		{ "move_to", lua_canvas_move_to },
		{ "line_to", lua_canvas_line_to },
		{ "rect", lua_canvas_rect },
		{ "oval", lua_canvas_oval },
		{ "set_font", lua_canvas_set_font },
		{ "show_text", lua_canvas_show_text },
		{ "text_size", lua_canvas_text_size },
		{ NULL, NULL}
	};
	static luaL_reg getters[] = {
		{ "line_width", lua_canvas_line_width },
		{ "stroke_color", lua_canvas_stroke_color },
		{ "fill_color", lua_canvas_fill_color },
		{ NULL, NULL}
	};
	static luaL_reg setters[] = {
		{ "line_width", lua_canvas_line_width },
		{ "stroke_color", lua_canvas_stroke_color },
		{ "fill_color", lua_canvas_fill_color },
		{ NULL, NULL}
	};
	Glue<Canvas>::usr_attr_mt(L, methods, getters, setters);
}



#ifdef __cplusplus
extern "C" {
#endif

int luaopen_Canvas(lua_State *L) {
	const char * libname = lua_tostring(L, 1);
	
	Glue<Canvas>::define(L);
	Glue<Canvas>::register_class(L);
	Glue<Canvas>::register_module(L, libname);

	return 1;
}


#ifdef __cplusplus
}
#endif
