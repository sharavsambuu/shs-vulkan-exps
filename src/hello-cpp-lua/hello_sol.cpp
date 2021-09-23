#define SOL_ALL_SAFETIES_ON 1
#include <iostream>
#include "sol/sol.hpp"

int main() {
    std::cout<<"Hello Sol2 Library"<<std::endl;

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);

    lua.script("print('Hello from LUA')");
    lua.script_file("hello_sol.lua");


    std::cout<<std::endl;
    
    return 0;
}