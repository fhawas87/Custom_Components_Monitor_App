#pragma once

#include <nvml.h>
#include <vector>

unsigned int                get_current_gpu_temperature();
unsigned int                get_core_utilization_percentage_rate();
unsigned int                get_gpu_clock_frequency();
unsigned int                get_gpu_fan_speed();
unsigned int                get_gpu_power_usage();

std::vector<unsigned long long> get_gpu_VRAM_info();


