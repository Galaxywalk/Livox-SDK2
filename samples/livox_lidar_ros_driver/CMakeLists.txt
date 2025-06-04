cmake_minimum_required(VERSION 3.10)
project(livox_lidar_ros_bridge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -------------------------------------------------------------------
#  include paths (absolute, so they work no matter where you build)
# -------------------------------------------------------------------
set(WRAPPER_INC ${CMAKE_CURRENT_LIST_DIR}/include)
set(SDK_INC     ${CMAKE_SOURCE_DIR}/include)   # adjust if needed

# -------------------------------------------------------------------
#  shared wrapper library
# -------------------------------------------------------------------
add_library(livox_core SHARED
            LivoxDriver.cpp)

target_include_directories(livox_core
    PUBLIC
        ${WRAPPER_INC}
        ${SDK_INC})

target_link_libraries(livox_core
    PRIVATE
        livox_lidar_sdk_shared)

find_package(pybind11 REQUIRED)
pybind11_add_module(_livox_core bindings.cpp)
target_include_directories(_livox_core
    PRIVATE
        ${WRAPPER_INC}
        ${SDK_INC})

target_link_libraries(_livox_core
    PRIVATE
        livox_core)                  # reuse the wrapper