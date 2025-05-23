// Tank Game (@kennedyengineering)

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "engine.hpp"

namespace py = pybind11;

PYBIND11_MODULE(python_bindings, handle) {
  handle.doc() = "Tank Game Python Bindings";

  py::enum_<TankGame::CategoryBits>(handle, "CategoryBits")
      .value("OBSTACLE", TankGame::CategoryBits::OBSTACLE)
      .value("PROJECTILE", TankGame::CategoryBits::PROJECTILE)
      .value("WALL", TankGame::CategoryBits::WALL)
      .value("TANK_BODY", TankGame::CategoryBits::TANK_BODY)
      .value("TANK_GUN", TankGame::CategoryBits::TANK_GUN)
      .export_values();

  py::class_<TankGame::ObstacleConfig>(handle, "ObstacleConfig")
      .def(py::init<>())
      .def_readwrite("positionX", &TankGame::ObstacleConfig::positionX)
      .def_readwrite("positionY", &TankGame::ObstacleConfig::positionY)
      .def_readwrite("radius", &TankGame::ObstacleConfig::radius);

  py::class_<TankGame::TankConfig>(handle, "TankConfig")
      .def(py::init<>())
      .def_readwrite("positionX", &TankGame::TankConfig::positionX)
      .def_readwrite("positionY", &TankGame::TankConfig::positionY)
      .def_readwrite("angle", &TankGame::TankConfig::angle)
      .def_readwrite("treadMaxSpeed", &TankGame::TankConfig::treadMaxSpeed)
      .def_readwrite("lidarPoints", &TankGame::TankConfig::lidarPoints)
      .def_readwrite("lidarRange", &TankGame::TankConfig::lidarRange)
      .def_readwrite("lidarRadius", &TankGame::TankConfig::lidarRadius);

  py::class_<TankGame::Config>(handle, "Config")
      .def(py::init<>())
      .def_readwrite("arenaWidth", &TankGame::Config::arenaWidth)
      .def_readwrite("arenaHeight", &TankGame::Config::arenaHeight)
      .def_readwrite("pixelDensity", &TankGame::Config::pixelDensity)
      .def_readwrite("verboseOutput", &TankGame::Config::verboseOutput);

  py::class_<TankGame::Engine>(handle, "Engine")
      .def(py::init<const TankGame::Config &>())
      /* Add & Remove Obstacles */
      .def("addObstacle", &TankGame::Engine::addObstacle)
      .def("removeObstacle", &TankGame::Engine::removeObstacle)
      /* Add & Remove Tanks */
      .def("addTank", &TankGame::Engine::addTank)
      .def("removeTank", &TankGame::Engine::removeTank)
      /* Control Tanks */
      .def("rotateTankGun", &TankGame::Engine::rotateTankGun)
      .def("fireTankGun", &TankGame::Engine::fireTankGun)
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
      .def("getTankGunAngle", &TankGame::Engine::getTankGunAngle)
      .def("getTankPosition", &TankGame::Engine::getTankPosition)
      .def("getTankOrientation", &TankGame::Engine::getTankOrientation)
      .def("getTankWorldVelocity", &TankGame::Engine::getTankWorldVelocity)
      .def("getTankLocalVelocity", &TankGame::Engine::getTankLocalVelocity)
      .def("getTankAngularVelocity", &TankGame::Engine::getTankAngularVelocity)
      /* Obstacle Rendering */
      .def("renderObstacle", &TankGame::Engine::renderObstacle)
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
