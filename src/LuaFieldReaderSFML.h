#pragma once

#include "LuaFieldReader.h"
#include <SFML/Graphics.hpp>


template<typename T>
struct LuaFieldReader<sf::Vector2<T>, void> {
	static const char* expected() { return "array of 2 numbers"; }
	static bool check(lua_State* L, int idx) { return lua_istable(L, idx); }
	static sf::Vector2<T> read(lua_State* L, int idx) {
		int abs = (idx < 0) ? lua_gettop(L) + 1 + idx : idx;
		lua_rawgeti(L, abs, 1); T x = LuaFieldReader<T>::read(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, abs, 2); T y = LuaFieldReader<T>::read(L, -1); lua_pop(L, 1);
		return {x, y};
	}
};



template<typename T>
struct LuaFieldReader<sf::Rect<T>, void> {
	static const char* expected() { return "array of 4 numbers"; }
	static bool check(lua_State* L, int idx) { return lua_istable(L, idx); }
	static sf::Rect<T> read(lua_State* L, int idx) {
		int abs = (idx < 0) ? lua_gettop(L) + 1 + idx : idx;
		lua_rawgeti(L, abs, 1); T l = LuaFieldReader<T>::read(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, abs, 2); T t = LuaFieldReader<T>::read(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, abs, 3); T w = LuaFieldReader<T>::read(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, abs, 4); T h = LuaFieldReader<T>::read(L, -1); lua_pop(L, 1);
		return {{l, t}, {w, h}};
	}
};
