#include <iostream>
#include <sstream>
#include <exception>
#include <flecs.h>
#include "sol/sol.hpp"


// Component types
struct Position {
    float x; 
    float y;
    std::string to_string() const {
        std::stringstream ss;
        ss<<"Position<x="<<std::to_string(x)<<", y="<<std::to_string(y)<<">";
        return ss.str();
    }
};
struct Velocity {
    float x; 
    float y;
    std::string to_string() const {
        std::stringstream ss;
        ss<<"Velocity<x="<<std::to_string(x)<<", y="<<std::to_string(y)<<">";
        return ss.str();
    }
};
// Tag types
struct Eats {};
struct Fruit{};
struct Meat {};


int main() {
    std::cout<<"Hello Flecs and Lua"<<std::endl;

    // ECS систем
    flecs::world ecs;

    // Sol2, Lua скрипт холбогч системийг ачааллах
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);

    // ecs-г lua дотор ашиглах боломжтой болгох
    lua["ecs"] = std::ref(ecs);


    // Зарим нэг entity C++ дээр үүсгэх
    auto hero = ecs.entity("Hero")
        .set(Position{2, 4})
        .set(Velocity{1, 2});
        //.add<Eats, Fruit>();
    std::cout<<hero.name()<<"'s got: ["<<hero.type().str()<<"]"<< std::endl;
    ecs.progress();
    ecs.progress();
    const Position *p = hero.get<Position>();
    printf("Hero's position is {%f, %f}\n", p->x, p->y);


    // Lua хэл дотроос C++ дээр үүсгэгдсэн ecs ажиллаж үзэх
    lua.new_usertype<Position>("Position",
        sol::call_constructor,
        sol::factories([](float x, float y) {
            return Position{x, y};
        }),
        "x", &Position::x,
        "y", &Position::y
    );
    lua.new_usertype<Velocity>("Velocity",
        sol::call_constructor,
        sol::factories([](float x, float y) {
            return Velocity{x, y};
        }),
        "x", &Velocity::x,
        "y", &Velocity::y
    );
    lua.new_usertype<Eats>    ("Eats"    );
    lua.new_usertype<Fruit>   ("Fruit"   );
    lua.new_usertype<Meat>    ("Meat"    );

    auto result = lua.script(R"(
        pos = Position(1., 2.)
        vel = Velocity(3., 3.)
        print('inspecting components into Lua')
        print(pos)
        print(vel)
        --local monster = ecs:entity('Monster'):set(Position.new(2,3))
        --monster::set(Position(5,6))
        --monster::set(Velocity(1,1))
        --monster::add(Eats)
        --monster::add(Meat)
        print('created monster entity from Lua')
    )");
    if (!result.valid()) {
        sol::error err = result;
        std::cout<<"Error in Lua execution : "<<err.what()<<std::endl;
    }

    std::cout<<"inspecting components from C++"<<std::endl;
    Position& pos_lua = lua["pos"];
    std::cout<<pos_lua.to_string()<<std::endl;
    Velocity& vel_lua = lua["vel"];
    std::cout<<vel_lua.to_string()<<std::endl;


    return 0;
}