#pragma once

#include <string>
#include <vector>

#include <tuple>
#include <stdexcept>

#include "lua.hpp"

template<typename T, typename Enable = void>
struct LuaFieldReader;



template<typename T>
struct LuaFieldReader<T, std::enable_if_t<std::is_integral_v<T>>>
{
	static const char* expected() { return "integer"; }
	static bool check(lua_State* L, int idx) { return lua_isinteger(L, idx); }
	static T read(lua_State* L, int idx) { return static_cast<T>(lua_tointeger(L, idx)); }
};



template<typename T>
struct LuaFieldReader<T, std::enable_if_t<std::is_floating_point_v<T>>>
{
	static const char* expected() { return "number"; }
	static bool check(lua_State* L, int idx) { return lua_isnumber(L, idx); }
	static T read(lua_State* L, int idx) { return static_cast<T>(lua_tonumber(L, idx)); }
};



template<>
struct LuaFieldReader<std::string, void>
{
	static const char* expected() { return "string"; }
	static bool check(lua_State* L, int idx) { return lua_isstring(L, idx); }
	static std::string read(lua_State* L, int idx) { return std::string(lua_tostring(L, idx)); }
};



template<typename T>
struct LuaFieldReader<std::vector<T>, void>
{
	static const char* expected() { return "array"; }
	static bool check(lua_State* L, int idx) { return lua_istable(L, idx); }
	static std::vector<T> read(lua_State* L, int idx)
	{
		std::vector<T> out;

		int abs = (idx < 0) ? lua_gettop(L) + 1 + idx : idx;
		int len = lua_rawlen(L, abs);

		out.reserve(len);
		for (int i = 1; i <= len; i++)
		{
			lua_rawgeti(L, abs, i);
			out.push_back(LuaFieldReader<T>::read(L, -1));
			lua_pop(L, 1);
		}
		return out;
	}
};



// Struct & Field lua parser
template<typename Struct, typename Field>
struct LuaFieldInfo
{
	const char* name;
	Field Struct::* member;
	bool optional = false;
};

template<typename Struct, typename Field>
static void lua_read_one_field(
	lua_State* L, int tableIndex,
	Struct& obj,
	const LuaFieldInfo<Struct, Field>& info
)
{
	lua_getfield(L, tableIndex, info.name); // ... t value

	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		if (!info.optional) {
			throw std::runtime_error(
				std::string("missing required field: ") + info.name
			);
		}
		return;
	}

	if (!LuaFieldReader<Field>::check(L, -1)) {
		std::string err = "field '";
		err += info.name;
		err += "': expected ";
		err += LuaFieldReader<Field>::expected();
		lua_pop(L, 1);
		throw std::runtime_error(err);
	}

	obj.*(info.member) = LuaFieldReader<Field>::read(L, -1);
	lua_pop(L, 1);
}

template<typename T>
struct LuaStructDescriptor;

template<typename, typename = void>
struct IsLuaStruct : std::false_type {};

template<typename T>
struct IsLuaStruct<T, std::void_t<decltype(LuaStructDescriptor<T>::fields())>>
	: std::true_type {};

template<typename T>
inline constexpr bool IsLuaStruct_v = IsLuaStruct<T>::value;

template<typename T>
struct LuaFieldReader<T, std::enable_if_t<IsLuaStruct_v<T>>>
{
	static const char* expected() { return "table"; }
	static bool check(lua_State* L, int idx) { return lua_istable(L, idx); }

	static T read(lua_State* L, int idx)
	{
		T out{};
		int abs = (idx < 0) ? lua_gettop(L) + 1 + idx : idx;

		auto fields = LuaStructDescriptor<T>::fields();
		std::apply([&](auto&&... finfo) {
			(lua_read_one_field(L, abs, out, finfo), ...);
		}, fields);

		return out;
	}
};

#define LUA_STRUCT_BEGIN(Type)							\
template<>												\
struct LuaStructDescriptor<Type> {						\
	static constexpr auto fields() {					\
		using S = Type;									\
		return std::make_tuple(

#define LUA_FIELD(name) \
	LuaFieldInfo<S, decltype(S::name)>{#name, &S::name, false}

#define LUA_FIELD_OPT(name) \
	LuaFieldInfo<S, decltype(S::name)>{#name, &S::name, true}

#define LUA_STRUCT_END()								\
		);												\
	}													\
};
