// Tank Game (@kennedyengineering)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "engine.hpp"

namespace py = pybind11;

PYBIND11_MODULE(tank_game, handle)
{
    handle.doc() = "Tank Game Python Bindings";

    py::class_<TankGame::TankConfig>(handle, "TankConfig")
        .def(py::init<>())
        ;

    py::class_<TankGame::Config>(handle, "Config")
        .def(py::init<>())
        .def_readwrite("arenaWidth", &TankGame::Config::arenaWidth)
        .def_readwrite("arenaHeight", &TankGame::Config::arenaHeight)
        .def_readwrite("tankConfigs", &TankGame::Config::tankConfigs)
        ;

    py::class_<TankGame::Engine>(handle, "Engine")
        .def(py::init<const TankGame::Config&>())
        .def("step", &TankGame::Engine::step)
        .def("destroy", &TankGame::Engine::destroy)
        ;
}