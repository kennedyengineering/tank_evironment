#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

extern "C" {
    #include "engine.h"
}

PYBIND11_MODULE(tank_game, handle) {
    handle.doc() = "Tank Game Environment";

    /* TankAction */
    py::class_<TankAction>(handle, "TankAction")
        .def(py::init<>())
        .def_readwrite("gun_angle", &TankAction::gun_angle)
        .def_readwrite("fire_gun", &TankAction::fire_gun)
        .def_readwrite("left_tread_force", &TankAction::left_tread_force)
        .def_readwrite("right_tread_force", &TankAction::right_tread_force);

    /* engineInit */
    handle.def("engineInit", &engineInit);

    /* engineStep */
    handle.def("engineStep", &engineStep);

    /* engineRender */
    handle.def("engineRender", []() {
        uint8_t* pixels = engineRender();
        auto free_pixels = [](void* p) { free(p); };
        py::capsule free_when_done(pixels, free_pixels);
        return py::array_t<uint8_t>({SCREEN_HEIGHT, SCREEN_WIDTH, 3}, pixels, free_when_done);
    });

    /* engineDestroy*/
    handle.def("engineDestroy", &engineDestroy);
}
