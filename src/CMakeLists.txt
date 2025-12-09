project(luaTableParser)

add_library(luaTableParser
    LuaTable.h LuaTable.cpp
    LuaFieldReader.h
)

target_include_directories(luaTableParser PRIVATE ${LUA_INCLUDE_DIR})

target_link_libraries(luaTableParser PUBLIC
    ${LUA_LIBRARIES}
)


option(LUA_FIELD_READER_SFML "Enable LuaFieldReader for SFML types" OFF)
if (LUA_FIELD_READER_SFML)
    target_sources(luaTableParser PUBLIC
        LuaFieldReaderSFML.h
    )

    target_link_libraries(luaTableParser PUBLIC
	SFML::System
	SFML::Window
	SFML::Graphics
    )
endif()
