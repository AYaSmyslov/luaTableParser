#pragma once

#include "LuaFieldReader.h"

class LuaTable
{
public:
	LuaTable(lua_State* L, int idx=-1);
	// ~LuaTable();

	bool initialize(const char* pConfigPath, std::string &pError);

	template<typename T>
	bool getVal(const char* key, T& out, std::string& error) const
	{
		lua_getfield(_L, _index, key);
		if (lua_isnil(_L, -1))
		{
			error = std::string("missing required field: '") + key + "'";
			lua_pop(_L, 1);
			return false;
		}
		if (!LuaFieldReader<T>::check(_L, -1))
		{
			error = std::string("field '") + key + "' must be " + LuaFieldReader<T>::expected();
			lua_pop(_L, 1);
			return false;
		}
		out = LuaFieldReader<T>::read(_L, -1);
		lua_pop(_L, 1);
		return true;
	}


	lua_State* _L;
	int _index;
};
