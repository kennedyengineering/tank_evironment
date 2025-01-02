// Tank Game (@kennedyengineering)

#include <pybind11/pybind11.h>

#include "engine.hpp"

namespace py = pybind11;

PYBIND11_MODULE(tank_game, handle)
{
    handle.doc() = "Tank Game Python Bindings";

    py::class_<TankGame::Engine>(handle, "Engine")
        .def(py::init<>())
        .def("step", &TankGame::Engine::step)
        .def("destroy", &TankGame::Engine::destroy)
        ;
}