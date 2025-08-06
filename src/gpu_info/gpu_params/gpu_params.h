#pragma once

#include <nvml.h>
#include <vector>

unsigned int                get_current_gpu_temperature();
unsigned int                get_core_utilization_percentage_rate();

std::vector<unsigned long long> get_gpu_VRAM_info();


