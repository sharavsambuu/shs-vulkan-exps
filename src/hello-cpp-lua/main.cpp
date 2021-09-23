#include <iostream>
#include <string>
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
    lua_getglobal(L, "lua_add");
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

// Lua хэл дээр дуудаж ажиллуулах боломжтой C++ функц тодорхойлох
int cpp_add(lua_State* L) {
    // Lua stack-аас параметрүүд задлах
    int n1  = lua_tonumber(L, 1);
    int n2  = lua_tonumber(L, 2);
    // C++ дээр тооцоолол хийх
    int sum = n1+n2;
    lua_pushnumber(L, sum);
    // stack-рүү хэдэн ширхэг утга хийсэн бэ гэдгийг буцаах
    return 1;
}

// Lua хэл дээр ажиллуулах боломжтой арай хэцүү C++ функц
// дурын тоотой параметрүүд бүхий функц
int cpp_average(lua_State* L) {
    // авах параметрүүдийн тоо
    int n = lua_gettop(L);
    // функцын аргументүүдээр нь давтах
    double sum = 0;
    for (int i=1; i<=n; i++) {
        sum += lua_tonumber(L, i);
    }
    // дундаж утгыг stack-руу хийх
    lua_pushnumber(L, sum/n);
    // нийлбэр утгыг stack-руу хийх
    lua_pushnumber(L, sum);
    // хэдэн гаралтын утгатай функц вэ гэдгийг тодорхойлох
    // энэ тохиолдолд дундаж болон нийлбэр гэсэн хоёр гаралттай
    return 2;
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


    // ####################################################
    // # C++ ээс Lua хэл дээр тодорхойлсон функцыг дуудах #
    // ####################################################

    // Lua хэл дээр тоо нэмэх функц тодорхойлох
    luaL_dostring(L, "function lua_add(x, y) return x+y end");
    // C++ ээс Lua дээр тодорхойлсон функцыг дуудаж ажиллуулах
    int input_a = 5;
    int input_b = 4;
    int result  = lua_add(L, input_a, input_b);
    std::cout<<"calling Lua function from C++: lua_add("<<input_a<<", "<<input_b<<")="<<result<<std::endl;


    // ####################################################
    // # Lua хэлнээс C++ дээр тодорхойлсон функцыг дуудах #
    // ####################################################

    // C++ функцыг Lua-д бүртгүүлэх
    lua_register(L, "cpp_add", cpp_add);
    // бүртгэсэн функцыг lua хэлнээс дуудах
    luaL_dostring(L, "print('calling C++ function from Lua: cpp_add(2, 4)='..cpp_add(2, 4))");

    // Арай хэцүү C++ функцыг Lua-д бүртгэх
    lua_register(L, "cpp_average", cpp_average);
    std::string lua_avg_code =
        "local avg, sum = cpp_average(1,2,3,4,5)"
        "print('calling arbitrary params C++ function from Lua: cpp_average(1,2,3,4,5) :')"
        "print('  average='..avg..', sum='..sum)";
    luaL_dostring(L, lua_avg_code.c_str());


    // ###################################
    // # Lua терминал циклдэх ажиллуулах #
    // ###################################

    std::cout<<"###########################"<<std::endl;
    std::cout<<"# Welcome to Lua terminal #"<<std::endl;
    std::cout<<"###########################"<<std::endl;
    while (true) {
        std::cout<<"lua > ";
        std::string input;
        std::getline(std::cin, input);
        if (input.compare("exit")==0) {
            break;
        }
        luaL_dostring(L, input.c_str());
    }

    std::cout<<"bye."<<std::endl;
    // Lua скрипт хөрвүүлэгчээр үүсгэгдсэн нөөцүүдийг чөлөөлөх
    lua_close(L);

    return 0;
}