#pragma once

#include <vector>
#include <string>


std::vector<double> get_cpu_cores_temperatures();
std::vector<unsigned int> get_cpu_cores_frequencies(size_t& number_of_cores);

double get_cpu_utilization();

std::string get_cpu_model_name();
