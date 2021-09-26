#include <iostream>
#include <pybind11/embed.h>
namespace py=pybind11;
using namespace py::literals;

PYBIND11_EMBEDDED_MODULE(fast_calc, m) {
    m.def("add", [](int i, int j) {
        return i + j;
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

    py::exec(R"(
        from hello import *
        print("What is the meaning of the life? answer is : "+str(answer()))
    )");

    return 0;
}