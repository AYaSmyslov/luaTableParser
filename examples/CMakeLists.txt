project(luaTableParserExample)

add_executable(luaTableParserExample main.cpp
)

target_include_directories(luaTableParserExample PRIVATE ${LUA_INCLUDE_DIR})

target_link_libraries(luaTableParserExample
    ${LUA_LIBRARIES}
    luaTableParser
)

add_dependencies(luaTableParserExample
    lua_project
)

configure_file(config.lua config.lua COPYONLY)
