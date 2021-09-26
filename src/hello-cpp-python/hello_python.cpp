#include <iostream>
#include <string>
#include <pybind11/embed.h>
namespace py=pybind11;
using namespace py::literals;


PYBIND11_EMBEDDED_MODULE(fast_calc, m) {
    m.def("add", [](int i, int j) {
        return i + j;
    });
}

struct IComponent {
    virtual ~IComponent() {};
};
struct Health {
    int value;
    Health() {}
    Health(int value) : value(value) {}
};
struct Position {
    float x;
    float y;
    Position() {}
    Position(float x, float y) : x(x), y(y) {}
};
PYBIND11_EMBEDDED_MODULE(components, m) {
    py::class_<Health>(m, "Health")
        .def(py::init<int>())
        .def("__str__", [](const Health &health) {
            return "Health<value="+std::to_string(health.value)+">";
        });
    py::class_<Position>(m, "Position")
        .def(py::init<float, float>())
        .def("__str__", [](const Position &position) {
            return "Position<x="+std::to_string(position.x)+", y="+std::to_string(position.y)+">";
        });
}

int main() {
    std::cout<<"Hello from C++"<<std::endl;

    py::scoped_interpreter guard{};
    py::print("Hello from Python embedding");
    
    py::exec(R"(
        args    = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**args)
        print(message)
    )");

    auto args    = py::dict("name"_a="World", "number"_a=42);
    auto message = "Hello, {name}! The answer is {number} from Python embedding"_s.format(**args);
    py::print(message);

    auto fast_calc = py::module_::import("fast_calc");
    auto result = fast_calc.attr("add")(1, 2).cast<int>();
    std::cout<<"calling python module in C++: "<<result<<std::endl;

    // Python хэл дээрхи файлаас код ажиллуулах
    py::exec(R"(
        from hello import *
        print("What is the meaning of the life? answer is : "+str(answer()))
    )");

    // C++ классыг Python хэлрүү оруулж ирэх
    py::exec(R"(
        import fast_calc
        print("fast_cal add result : ", fast_calc.add(21,21))

        import components
        health   = components.Health(100)
        position = components.Position(10.0, 200.0)

        print("health component :")
        print(health)

        print("position component :")
        print(position)
    )");

    return 0;
}