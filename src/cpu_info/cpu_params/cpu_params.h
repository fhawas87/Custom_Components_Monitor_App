#pragma once

#include <vector>


std::vector<double> get_cpu_cores_temperatures();
std::vector<unsigned int> get_cpu_cores_frequencies(size_t& number_of_cores);
