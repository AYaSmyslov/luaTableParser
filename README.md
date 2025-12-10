# Lua table parser

C++ templates for reading fields from Lua tables

Includes:
- basic types (`int`, `float`, `std::string`, and etc.)
- containers (`std::vector<T>`)
- structures (Specialization can be described by a macro)
- SFML types (`sf::Vector2<T>`, `sf::Rect<T>`)

## Dependencies

- C++17
- CMake
- [Lua 5.4.x](https://www.lua.org)
- Optional [SFML 3.x](https://github.com/SFML/SFML) (Graphics, Window, System))

SFML templates are configuraated in the CMake configuration

The library **does not pull dependencies by itself**, they need to be intalled separately
It is a small addition to lua

---

## Exapmles

```cpp
#include <LuaTable.h> // Required header
```

Example of a decription of the structure and specialzation
```cpp
struct Config {
	int count;
	std::string str;
	std::vector<int> arr;
};
LUA_STRUCT_BEGIN(Config)
	LUA_FIELD(count),
	LUA_FIELD(str),
	LUA_FIELD(arr)
LUA_STRUCT_END()
```

You can read simple fields, but its more convenient to describe one large structure and get the data in one call
```cpp
#define CONFIG_PATH "config.lua"

int main()
{
	Config cfg; // Declaring data object

	// Lua initialization (lua.h)
	lua_State* L = luaL_newstate();
	std::string err;
	if (!L)
	{
		std::cout << "cant open" << std::endl;
		lua_close(L);
		return -1;
	}
	luaL_openlibs(L);

	// Initialize templated class (LuaTable.h)
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

	// Get data from lua and move to cfg object
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

	std::cout << "}" << std::endl;

	lua_close(L);

	return 0;
}
```

`LuaTable` does not initialize `lua_State*` and does not own. Initialization and closure takes place outise the `LuaTable`

Method `bool getVal(const char* key, T& out, std::string& error) const`:
- `key` name of the lua field to be read
- `out` variable that will be filled with data (Must have specialization)
- `error` has an error text if the method returns `false` / empty if the method returns `true`

```cpp
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
```

Full example [examples](https://github.com/AYaSmyslov/luaTableParser/tree/main/examples)

