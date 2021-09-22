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

// Lua хэл дээр тодорхойлсон функцыг C++ ээс дуудах функц
int lua_add(lua_State* L, int a, int b) {
    // Lua stack-ийн орой дээр add функцыг хийх
    lua_getglobal(L, "add");
    // stack-ийн оройруу эхний утгыг хийх
    lua_pushnumber(L, a);
    // stack-ийн оройруу хоёр дахь утгыг хийх
    lua_pushnumber(L, b);
    // 2 оролт 1 гаралт бүхий функц дуудах
    lua_call(L, 2, 1);
    // функцын үр дүнг авах
    int result = (int)lua_tointeger(L, -1);
    // Үр дүнг lua stack-аас хасах хэрэгтэй.
    // Хэрвээ олон утга буцаавал тэрнийх нь тоогоор pop хийнэ
    lua_pop(L, 1);
    return result;
}

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


    // Lua хэл дээр тоо нэмэх функц тодорхойлох
    luaL_dostring(L, "function add(x, y) return x+y end");
    // C++ ээс Lua дээр тодорхойлсон функцыг дуудаж ажиллуулах
    int input_a = 5;
    int input_b = 4;
    int result  = lua_add(L, input_a, input_b);
    std::cout<<"Lua функц : add("<<input_a<<","<<input_b<<")="<<result<<std::endl;


    // Lua скрипт хөрвүүлэгчээр үүсгэгдсэн нөөцүүдийг чөлөөлөх
    lua_close(L);

    return 0;
}