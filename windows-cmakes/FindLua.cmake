set(LUA_DIR $ENV{LUA54_PATH})

message(STATUS "setting up Lua5.4 Path: $ENV{LUA54_PATH}")


# Support both 32 and 64 bit builds
# Download Lua5.4 Visual Studio 2019 version from
# - https://www.lua.org/download.html
# - http://luabinaries.sourceforge.net/
# - https://sourceforge.net/projects/luabinaries/files/5.4.2/Windows%20Libraries/Static/
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    set(LUA_INCLUDE_DIRS "$ENV{LUA54_PATH}/lua-5.4.2_Win64_vc16_lib/include")
    set(LUA_LIBRARIES "$ENV{LUA54_PATH}/lua-5.4.2_Win64_vc16_lib/lua54.lib")
else ()
    set(LUA_INCLUDE_DIRS "$ENV{LUA54_PATH}/lua-5.4.2_Win32_vc16_lib/include")
    set(LUA_LIBRARIES "$ENV{LUA54_PATH}/lua-5.4.2_Win32_vc16_lib/lua54.lib")
endif ()

string(STRIP "${LUA_LIBRARIES}" LUA_LIBRARIES)