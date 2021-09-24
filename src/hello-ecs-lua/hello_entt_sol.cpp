#include <iostream>
#include <entt/entt.hpp>


struct position {
    float x;
    float y;
};
struct velocity {
    float dx;
    float dy;
};

int main() {
    std::cout<<"Integrating EnTT and Sol2 experiments"<<std::endl;

    // EnTT ECS систем
    entt::registry registry;

    // туршилтын entity-нууд үүсгэх
    const auto first_entity = registry.create();
    auto &first_pos = registry.emplace<position>(first_entity);
    first_pos.x     = 10.0f;
    first_pos.y     = 11.0f;
    auto &first_vel = registry.emplace<velocity>(first_entity);
    first_vel.dx    = 1.5f;
    first_vel.dy    = 1.0f;

    const auto second_entity = registry.create();
    auto &second_pos = registry.emplace<position>(second_entity);
    second_pos.x     = 20.0f;
    second_pos.y     = 31.0f;

    // үүсгэсэн entity-нүүдээ шүүж харах
    registry.view<position>().each([](const auto entity, auto &pos) {
        std::cout<<"<position> : "<<pos.x<<" "<<pos.y<<std::endl;
    });
    registry.view<position, velocity>().each([](const auto entity, auto &pos, auto &vel) {
        std::cout<<"<position, velocity> : "<<pos.x<<" "<<pos.y<<" "<<vel.dx<<" "<<vel.dy<<std::endl;
    });


    std::cout<<std::endl;
    return 0;
}