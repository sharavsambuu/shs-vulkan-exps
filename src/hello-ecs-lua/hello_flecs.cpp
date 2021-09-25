#include <iostream>

#include <flecs.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif


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


int main() {
    std::cout<<"Hello Flecs ECS"<<std::endl;

    flecs::world ecs;

    auto Bob = ecs.entity("Bob")
        .set(Position{0, 0})
        .set(Velocity{1, 2})
        .add<Eats, Apples>();

    std::cout << Bob.name() << "'s got: [" << Bob.type().str() << "]" << std::endl;

    ecs.progress();
    ecs.progress();

    const Position *p = Bob.get<Position>();
    printf("Bob's position is {%f, %f}\n", p->x, p->y);

    return 0;
}