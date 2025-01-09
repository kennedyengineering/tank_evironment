// Tank Game (@kennedyengineering)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "engine.hpp"

namespace py = pybind11;

// TODO: switch to _ notation for python names

PYBIND11_MODULE(tank_game, handle)
{
    handle.doc() = "Tank Game Python Bindings";

    py::class_<TankGame::TankConfig>(handle, "TankConfig")
        .def(py::init<>())
        .def_readwrite("positionX", &TankGame::TankConfig::positionX)
        .def_readwrite("positionY", &TankGame::TankConfig::positionY)
        .def_readwrite("angle", &TankGame::TankConfig::angle)
        ;

    py::class_<TankGame::Config>(handle, "Config")
        .def(py::init<>())
        .def_readwrite("arenaWidth", &TankGame::Config::arenaWidth)
        .def_readwrite("arenaHeight", &TankGame::Config::arenaHeight)
        ;

    py::class_<TankGame::Engine>(handle, "Engine")
        .def(py::init<const TankGame::Config&>())
        .def("addTank", &TankGame::Engine::addTank)
        .def("removeTank", &TankGame::Engine::removeTank)
        .def("renderTank", &TankGame::Engine::renderTank)
        .def("getImage", [](TankGame::Engine &self)
        {
            // Get image dimensions
            auto [imageWidth, imageHeight] = self.getImageDimensions();
            int imageChannels = self.getImageChannels();

            // Get image buffer
            std::vector<unsigned char> imageBuffer = self.getImageBuffer();

            // Return numpy array
            return py::array_t<unsigned char>({imageHeight, imageWidth, imageChannels}, imageBuffer.data());
        })
        .def("step", &TankGame::Engine::step)
        ;
}