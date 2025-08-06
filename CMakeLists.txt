cmake_minimum_required(VERSION 3.5)
project(MonitorApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(NVML_INCLUDE_DIR
  NAMES nvml.h 
  PATHS
    /usr/include 
)

find_library(NVML_LIBRARY
  NAMES nvidia-ml
  PATHS
    /usr/lib 
    /usr/lib/x86_64-linux-gnu 
)

include_directories(
  ${NVML_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu_info
)

set(SRC  
  src/gpu_info/main.cpp 
  src/gpu_info/gpu_model/gpu_model.cpp
  src/gpu_info/gpu_model/gpu_device.cpp
  src/gpu_info/gpu_params/gpu_thermal_value.cpp
  src/gpu_info/gpu_params/gpu_usage.cpp
  src/gpu_info/gpu_params/gpu_memory_info.cpp
  src/gpu_info/gpu_params/gpu_other_params.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${NVML_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu_info)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${NVML_LIBRARY}
)
