#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <typeinfo>
#include <flecs.h>
#include "sol/sol.hpp"
#include "sol/types.hpp"

template<typename T> std::string type_name();

// Component types
struct BaseComponent {
    virtual void foo() {} // polymorphic
};
struct Position : BaseComponent {
    float x; 
    float y;
    Position() {}
    Position(float px, float py) {
        x = px;
        y = py;
    }
    void foo() override {}
    std::string to_string() const {
        std::stringstream ss;
        ss<<"Position<x="<<std::to_string(x)<<", y="<<std::to_string(y)<<">";
        return ss.str();
    }
};
struct Velocity : BaseComponent {
    float x; 
    float y;
    Velocity() {}
    Velocity(float px, float py) {
        x = px;
        y = py;
    }
    void foo() override {}
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
    auto hero = ecs.entity("Hero");
    Position pos_comp = Position(2, 1);
    hero.set((Position)pos_comp);
    Velocity vel_comp = Velocity(4, 1);
    hero.set((Velocity)vel_comp);
    hero.add<Eats, Fruit>();


    std::cout<<hero.name()<<"'s got: ["<<hero.type().str()<<"]"<< std::endl;
    ecs.progress();
    ecs.progress();
    const Position *p = hero.get<Position>();
    printf("Hero's position is {%f, %f}\n", p->x, p->y);


    // Lua хэл дотроос C++ дээр үүсгэгдсэн ecs ажиллаж үзэх
    lua.new_usertype<BaseComponent>("BaseComponent");
    lua.new_usertype<Position>("Position",
        sol::constructors<Position(float, float)>(),
        sol::call_constructor, sol::factories([](float x, float y) {
            return Position(x, y);
        }),
        "x", &Position::x,
        "y", &Position::y,
        sol::base_classes, sol::bases<BaseComponent>()
    );
    lua.new_usertype<Velocity>("Velocity",
        sol::constructors<Velocity(float, float)>(),
        sol::call_constructor, sol::factories([](float x, float y) {
            return Velocity(x, y);
        }),
        "x", &Velocity::x,
        "y", &Velocity::y,
        sol::base_classes, sol::bases<BaseComponent>()
    );
    lua.new_usertype<Eats> ("Eats" );
    lua.new_usertype<Fruit>("Fruit");
    lua.new_usertype<Meat> ("Meat" );

    // Lua дээр дуудаж болохоор API маягийн юм бүрдүүлэх
    lua["ecs_create_entity"] = [](const flecs::world& ecs, const std::string name) {
        return ecs.entity(name.c_str());
    };
    // BaseComponent-ээс удамшуулж интерфэйс үүсгэх байдлаар полиморфизмын дагуу
    // янз бүрийн төрлийн компонентүүдийг нэг функц ашиглан хаягаар нь хандаж оноох
    lua["ecs_set_component_to_entity"] = [](flecs::entity& entity, BaseComponent& comp_src) {
        if (typeid(comp_src)==typeid(Position)) {
            Position* comp_dest = dynamic_cast<Position*>(&comp_src);
            entity.set((Position)(*comp_dest));
        } else 
        if (typeid(comp_src)==typeid(Velocity)) {
            Velocity* comp_dest = static_cast<Velocity*>(&comp_src);
            entity.set((Velocity)(*comp_dest));
        }
    };

    auto result = lua.script(R"(
        pos = Position.new(1., 2.)
        vel = Velocity(3., 3.)
        print('inspecting components into Lua')
        print(pos)
        print(vel)

        monster = ecs_create_entity(ecs, "Monster Chan")
        ecs_set_component_to_entity(monster, pos)
        ecs_set_component_to_entity(monster, vel)
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
    flecs::entity& monster = lua["monster"];
    std::cout<<"monster's name created by Lua script in c++ : "<<monster.name()<<std::endl;
    std::cout<<monster.name()<<"'s got: ["<<monster.type().str()<<"]"<< std::endl;


    return 0;
}