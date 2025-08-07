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

find_path(LIBSENSORS_INCLUDE_DIR
  NAMES sensors/sensors.h 
  PATHS
    /src/include 
    /src/local/include 
)

find_library(LIBSENSORS_LIBRARY
  NAMES sensors 
  PATHS 
    /usr/lib 
    /usr/lib/x86_64-linux-gnu 
    /usr/local/lib 
)


include_directories(
  ${NVML_INCLUDE_DIRS}
  ${LIBSENSORS_INCLUCDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu_info
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_info
)

set(SRC
  # int main() function localization
  src/main.cpp
  # gpu functions
  src/gpu_info/gpu_model/gpu_model.cpp
  src/gpu_info/gpu_model/gpu_device.cpp
  src/gpu_info/gpu_params/gpu_thermal_value.cpp
  src/gpu_info/gpu_params/gpu_usage.cpp
  src/gpu_info/gpu_params/gpu_memory_info.cpp
  src/gpu_info/gpu_params/gpu_other_params.cpp
  # cpu functions
  src/cpu_info/cpu_params/cpu_thermal_info.cpp
  src/cpu_info/cpu_params/cpu_freq_info.cpp
  src/cpu_info/cpu_params/cpu_utilization.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${NVML_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu_info
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_info
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ${NVML_LIBRARY}
  ${LIBSENSORS_LIBRARIES}
  ${LIBSENSORS_LIBRARY}
)
