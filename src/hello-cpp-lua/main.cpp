#include <iostream>
#ifdef _WIN32
extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
#else
extern "C" {
    #include "lua5.4/lua.h"
    #include "lua5.4/lualib.h"
    #include "lua5.4/lauxlib.h"
}
#endif

int main(int argc, char **argv) {
    std::cout << "Hello from Cpp" << std::endl;

    // Lua хөрвүүлэгч
    lua_State* L;
    L = luaL_newstate();
    // print, math гэх мэт суурь Lua сангуудыг ачаалах
    luaL_openlibs(L);

    // ###########################################
    // # энэ хэсгээс Lua хэлийг дуудах боломжтой #
    // ###########################################

    // string доторхи Lua код ажиллуулах
    luaL_dostring(L, "print('Hello from Lua')");
    luaL_dostring(L, "for x = 1, 5 do print(x) end");

    // файлаас Lua script дуудаж ажиллуулах
    luaL_dofile(L, "hello.lua");

    // Lua скрипт хөрвүүлэгчээр үүсгэгдсэн нөөцүүдийг чөлөөлөх
    lua_close(L);

    return 0;
}