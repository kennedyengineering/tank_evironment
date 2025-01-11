// Tank Game (@kennedyengineering)

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "engine.hpp"

namespace py = pybind11;

PYBIND11_MODULE(python_bindings, handle) {
  handle.doc() = "Tank Game Python Bindings";

  py::class_<TankGame::TankConfig>(handle, "TankConfig")
      .def(py::init<>())
      .def_readwrite("positionX", &TankGame::TankConfig::positionX)
      .def_readwrite("positionY", &TankGame::TankConfig::positionY)
      .def_readwrite("angle", &TankGame::TankConfig::angle);

  py::class_<TankGame::Config>(handle, "Config")
      .def(py::init<>())
      .def_readwrite("arenaWidth", &TankGame::Config::arenaWidth)
      .def_readwrite("arenaHeight", &TankGame::Config::arenaHeight);

  py::class_<TankGame::Engine>(handle, "Engine")
      .def(py::init<const TankGame::Config &>())
      /* Add & Remove Tanks */
      .def("addTank", &TankGame::Engine::addTank)
      .def("removeTank", &TankGame::Engine::removeTank)
      /* Control Tanks */
      .def("rotateTankGun", &TankGame::Engine::rotateTankGun)
      .def("fireTankGun", &TankGame::Engine::rotateTankGun)
      .def("moveLeftTankTread", &TankGame::Engine::moveLeftTankTread)
      .def("moveRightTankTread", &TankGame::Engine::moveRightTankTread)
      /* Tank Sensors */
      .def("scanTankLidar",
           [](TankGame::Engine &self, TankGame::RegistryId tankId) {
             // Get scan data
             std::vector<float> scanData = self.scanTankLidar(tankId);

             // Return numpy array
             return py::array_t<float>(scanData.size(), scanData.data());
           })
      /* Tank Rendering */
      .def("renderProjectiles", &TankGame::Engine::renderProjectiles)
      .def("renderTank", &TankGame::Engine::renderTank)
      .def("renderTankLidar", &TankGame::Engine::renderTankLidar)
      /* Image Handling */
      .def("clearImage", &TankGame::Engine::clearImage)
      .def("getImageDimensions", &TankGame::Engine::getImageDimensions)
      .def("getImageChannels", &TankGame::Engine::getImageChannels)
      .def("getImageBuffer",
           [](TankGame::Engine &self) {
             // Get image dimensions
             auto [imageWidth, imageHeight] = self.getImageDimensions();
             int imageChannels = self.getImageChannels();

             // Get image buffer
             std::vector<unsigned char> imageBuffer = self.getImageBuffer();

             // Return numpy array
             return py::array_t<unsigned char>(
                 {imageHeight, imageWidth, imageChannels}, imageBuffer.data());
           })
      /* Simulation Step */
      .def("step", &TankGame::Engine::step);
}
