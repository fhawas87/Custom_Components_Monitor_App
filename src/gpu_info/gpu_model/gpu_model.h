#pragma once

#include <string>

#include <nvml.h>

nvmlDevice_t get_device();
std::string get_accessible_device_name();
unsigned int get_current_gpu_temperature();
