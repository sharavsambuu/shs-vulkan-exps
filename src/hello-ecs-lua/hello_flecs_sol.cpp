#include <iostream>

#include <flecs.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#include "sol/sol.hpp"


// Component types
struct Position {
    double x;
    double y;
};

struct Velocity {
    double x;
    double y;
};

// Tag types
struct Eats   { };
struct Apples { };


int use_sol(lua_State* L) {
    sol::state_view lua(L);

    lua.script(R"(
        print('using sol from lua_State')
    )");

    return 0;
}


int main() {
    std::cout<<"Hello Flecs and Lua"<<std::endl;

    // Sol2, Lua скрипт холбогч системийг ачааллах
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcclosure(L, &use_sol, 0);
    lua_setglobal(L, "use_sol");
    if (luaL_dostring(L, "use_sol()")) {
        lua_error(L);
        return -1;
    }


    // ECS систем
    flecs::world ecs;

    auto Bob = ecs.entity("Bob")
        .set(Position{2, 4})
        .set(Velocity{1, 2})
        .add<Eats, Apples>();

    std::cout << Bob.name() << "'s got: [" << Bob.type().str() << "]" << std::endl;

    ecs.progress();
    ecs.progress();

    const Position *p = Bob.get<Position>();
    printf("Bob's position is {%f, %f}\n", p->x, p->y);




    lua_close(L);

    return 0;
}