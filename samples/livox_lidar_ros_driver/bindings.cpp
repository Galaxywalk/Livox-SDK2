// bindings.cpp  —  exposes LivoxDriver to Python via pybind11
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>   // let py::function convert to std::function
#include "LivoxDriver.h"

namespace py = pybind11;

// Helper: wrap the C++ callback so we can keep the Python
// callable alive inside the C++ object.
struct PyDriver : public LivoxDriver {
    void register_pointcloud_callback(py::function py_cb) {
        // keep a copy of the Python function alive
        pc_py_cb_ = std::move(py_cb);

        // C++ -> Python thunk
        setPointcloudCallback(
            [this](uint32_t handle, uint8_t dev_type,
                   const LivoxLidarEthernetPacket* pkt)
            {
                if (!pc_py_cb_) return;
                py::gil_scoped_acquire gil;
                // reacquire GIL because SDK threads are native
                pc_py_cb_(handle,
                    dev_type,
                    reinterpret_cast<std::uintptr_t>(pkt->data),
                    pkt->dot_num,
                    pkt->data_type);
            });
    }

    void register_imu_callback(py::function py_cb) {
        imu_py_cb_ = std::move(py_cb);

        setImuCallback(
            [this](uint32_t handle, uint8_t dev_type,
                   const LivoxLidarEthernetPacket* pkt)
            {
                if (!imu_py_cb_) return;
                py::gil_scoped_acquire gil;
                imu_py_cb_(handle,
                    dev_type,
                    reinterpret_cast<std::uintptr_t>(pkt->data),
                    pkt->dot_num,
                    pkt->data_type);
            });
    }

  private:
    py::function pc_py_cb_;
    py::function imu_py_cb_;
};

PYBIND11_MODULE(_livox_core, m) {
    py::class_<PyDriver>(m, "LivoxDriver")
        .def(py::init<>())
        .def("init",     &PyDriver::init,
             py::arg("config_json"))
        .def("shutdown", &PyDriver::shutdown)
        .def("register_pointcloud_callback",
             &PyDriver::register_pointcloud_callback,
             py::arg("callable"),
             R"doc(
Python signature:  cb(handle:int, dev_type:int,
                      pkt_ptr:int, dot_num:int, data_type:int)
`pkt_ptr` is just the raw C pointer cast to int.  If you need the actual
bytes, use ctypes to cast it to a structure of your choice.
)doc")
        .def("register_imu_callback",
             &PyDriver::register_imu_callback,
             py::arg("callable"),
             R"doc(
Python signature:  cb(handle:int, dev_type:int,
                      pkt_ptr:int, dot_num:int, data_type:int)
Similar to pointcloud callback, pkt_ptr is a raw memory pointer.
)doc");

}
