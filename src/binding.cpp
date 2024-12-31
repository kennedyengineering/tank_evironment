#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(tank_game, handle) {
    handle.doc() = "Tank Game Environment";
    handle.def("test", []() { return "Hello, World!"; });
}
