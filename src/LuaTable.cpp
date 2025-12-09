#include "LuaTable.h"

const char* NULL_LUA = "Failed to create Lua state!";
// const std::string NOT_TABLE = "Config are not table!";

LuaTable::LuaTable(lua_State* L, int idx)
: _L(L)
, _index(idx)
{

}

bool LuaTable::initialize(const char *pConfigPath, std::string &pError)
{
	if (!_L)
	{
		pError = NULL_LUA;
		return false;
	}

	if (luaL_dofile(_L, pConfigPath) != LUA_OK)
	{
		pError = lua_tostring(_L, -1);
		lua_pop(_L, 1);
		return false;
	}

	return true;
}
