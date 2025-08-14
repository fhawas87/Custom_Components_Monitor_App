cmake_minimum_required(VERSION 3.5)
project(MonitorApp LANGUAGES C CXX)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_STANDARD 99)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(NVML_INCLUDE_DIR
  NAMES nvml.h 
  PATHS /usr/include 
)

find_library(NVML_LIBRARY
  NAMES nvidia-ml
  PATHS /usr/lib /usr/lib/x86_64-linux-gnu 
)

find_path(LIBSENSORS_INCLUDE_DIR
  NAMES sensors/sensors.h 
  PATHS /src/include /src/local/include 
)

find_library(LIBSENSORS_LIBRARY
  NAMES sensors 
  PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib 
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set(SRC
  src/main.cpp
  src/gpu_info/gpu_model/gpu_model.cpp
  src/gpu_info/gpu_model/gpu_device.c
  src/gpu_info/gpu_params/gpu_thermal_value.c
  src/gpu_info/gpu_params/gpu_usage.c
  src/gpu_info/gpu_params/gpu_memory_info.cpp
  src/gpu_info/gpu_params/gpu_other_params.c
  src/cpu_info/cpu_params/cpu_thermal_info.cpp
  src/cpu_info/cpu_params/cpu_freq_info.cpp
  src/cpu_info/cpu_params/cpu_utilization.cpp
  src/cpu_info/cpu_model/cpu_model_name.cpp
  src/ram_info/ram_usage.c
  src/fan_info/available_fans_tach_info.cpp
)

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(IMGUI_DIR ${EXTERNAL}/imgui)
set(IMPLOT_DIR ${EXTERNAL}/implot)

add_library(implot STATIC
  ${IMPLOT_DIR}/implot.cpp
  ${IMPLOT_DIR}/implot_items.cpp
)
add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(implot PUBLIC ${IMPLOT_DIR})
target_link_libraries(implot PUBLIC imgui)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${NVML_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gpu_info
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_info
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ram_info
  ${CMAKE_CURRENT_SOURCE_DIR}/src/fan_info
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  imgui
  implot
  glfw
  GLEW::GLEW
  OpenGL::GL
  ${NVML_LIBRARY}
  ${LIBSENSORS_LIBRARIES}
  ${LIBSENSORS_LIBRARY}
)
