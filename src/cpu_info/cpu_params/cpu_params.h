#pragma once

#include <vector>
#include <string>


std::vector<double> get_cpu_cores_temperatures();
std::vector<unsigned int> get_cpu_cores_frequencies(const size_t& number_of_cores);

//double calculate_avarage_cpu_temperature(const std::vector<double> &per_core_temperatures) {
//  double avg = 0;
//  fo

double get_cpu_utilization();

std::string get_cpu_model_name();
