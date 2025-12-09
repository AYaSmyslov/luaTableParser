#include <iostream>

#include <LuaTable.h>

#define CONFIG_PATH "config.lua"

struct Config
{
	int count;
	std::string str;
	std::vector<int> arr;
};

LUA_STRUCT_BEGIN(Config)
	LUA_FIELD(count),
	LUA_FIELD(str),
	LUA_FIELD(arr)
LUA_STRUCT_END()

int main()
{
	Config cfg;

	lua_State* L = luaL_newstate();
	std::string err;

	if (L == nullptr)
	{
		std::cout << "cant open" << std::endl;
		lua_close(L);
		return -1;
	}

	luaL_openlibs(L);

	LuaTable luaConfig(L);
	if (!luaConfig.initialize(
		CONFIG_PATH,
		err
	))
	{
		std::cout << err;
		lua_close(L);
		return -1;
	}

	if (!luaConfig.getVal("config", cfg, err))
	{
		std::cout << err;
		lua_close(L);
		return -1;
	}



	std::cout << "Config:" << std::endl;
	std::cout << "count: " <<  cfg.count << std::endl;
	std::cout << "str: " << cfg.str << std::endl;

	std::cout << "arr " << "(size: " << cfg.arr.size() << "): { ";

	for (auto o : cfg.arr)
	{
		std::cout << o << " ";
	}

	std::cout << std::endl;

	lua_close(L);

	return 0;
}

