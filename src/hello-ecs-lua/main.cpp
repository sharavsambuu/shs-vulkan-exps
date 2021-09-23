//
//  Лавлагаанууд :
//  - https://skypjack.github.io/entt/md_docs_md_entity.html
//
//


#include <iostream>
#include <entt/entt.hpp>


// Компонентүүд
struct position {
    float x;
    float y;
};
struct velocity {
    float dx;
    float dy;
};

// Төсөөллийн системүүд
void system_movement(entt::registry &registry) {
    auto view = registry.view<position>();
    std::cout<<"movement system is working..."<<std::endl;
    view.each([](const auto entity, auto &pos) { 
        pos.x += 1.0f;
        pos.y += 1.0f;
        std::cout<<pos.x<<" "<<pos.y<<std::endl;
    });
}
void system_speed(entt::registry &registry) {
    auto view = registry.view<velocity>();
    std::cout<<"speed system is working..."<<std::endl;
    view.each([](const auto entity, auto &vel) {
        vel.dx += 1.0f;
        vel.dy += 1.0f;
        std::cout<<vel.dx<<" "<<vel.dy<<std::endl;
    });
}
void system_kinematic(entt::registry &registry) {
    auto view = registry.view<position, velocity>();
    std::cout<<"kinematics system is working..."<<std::endl;
    view.each([](const auto entity, auto &pos, auto &vel) {
        pos.x += vel.dx;
        pos.y += vel.dy;
        std::cout<<pos.x<<" "<<pos.y<<" "<<vel.dx<<" "<<vel.dy<<std::endl;
    });
}

int main() {
    std::cout<<"Hello ECS"<<std::endl;

    // ECS ачааллах
    entt::registry registry;
    
    // Туршилтын entity-нууд үүсгэх
    for(auto i=0; i<10; ++i) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i*1.0f, i*1.0f);
        if (i%2==0.0f) {
            auto &vel = registry.emplace<velocity>(entity);
            vel.dx = i*0.1f;
            vel.dy = i*0.1f;
        }
    }

    // Төсөөллийн цикл
    for (auto i=0; i<10; ++i) {
        system_movement (registry);
        system_speed    (registry);
        system_kinematic(registry);
    }

    return 0;
}