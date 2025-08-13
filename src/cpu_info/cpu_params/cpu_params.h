#pragma once

#include <vector>
#include <string>


std::vector<float> get_cpu_cores_temperatures();
std::vector<float> get_cpu_cores_frequencies(size_t number_of_cores);

double get_cpu_utilization();
