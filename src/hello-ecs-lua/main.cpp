#include <iostream>
#include <entt/entt.hpp>
//#include <entt/entity/registry.hpp>


struct position {
    float x;
    float y;
};

struct velocity {
    float dx;
    float dy;
};

void update(entt::registry &registry) {
    auto view = registry.view<const position, velocity>();

    // use a callback
    std::cout<<"Callback based updating..."<<std::endl;
    view.each([](const auto &pos, auto &vel) { 
        std::cout<<"pos["<<pos.x<<" "<<pos.y<<"] vel["<<vel.dx<<" "<<vel.dy<<"]"<<std::endl;
    });

    // use an extended callback
    std::cout<<"Extended callback based updating..."<<std::endl;
    view.each([](const auto entity, const auto &pos, auto &vel) { 
        std::cout<<"pos["<<pos.x<<" "<<pos.y<<"] vel["<<vel.dx<<" "<<vel.dy<<"]"<<std::endl;
    });

    // use a range-for
    std::cout<<"Range-for based updating..."<<std::endl;
    for(auto [entity, pos, vel]: view.each()) {
        std::cout<<"pos["<<pos.x<<" "<<pos.y<<"] vel["<<vel.dx<<" "<<vel.dy<<"]"<<std::endl;
    }

    // use forward iterators and get only the components of interest
    // std::cout<<"forward iterators based updating..."<<std::endl;
    for(auto entity: view) {
        //auto &pos = view.get<position>(entity);
        //auto &vel = view.get<velocity>(entity);
        //std::cout<<"pos["<<pos.x<<" "<<pos.y<<"] vel["<<vel.dx<<" "<<vel.dy<<"]"<<std::endl;
    }
}

int main() {
    std::cout<<"Hello ECS"<<std::endl;

    entt::registry registry;
    
    for(auto i = 0u; i < 10u; ++i) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i*1.f, i*1.f);
        if(i % 2 == 0) { registry.emplace<velocity>(entity, i*.1f, i*.1f); }
    }

    update(registry);

    return 0;
}