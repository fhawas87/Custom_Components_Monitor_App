#include <stdio.h>
#include <vector>


std::vector<unsigned int> get_cpu_cores_frequencies(size_t& number_of_cores) {
  std::vector<unsigned int> cpu_cores_freq;
  char path_buffer[128];

  for (size_t core = 0; core < number_of_cores; core++) {

    int number_of_chars = snprintf(path_buffer, sizeof(path_buffer),
    "/sys/devices/system/cpu/cpu%zu/cpufreq/scaling_cur_freq", core);

    FILE *scaling_cur_freq_info = fopen(path_buffer, "r");
    if (!scaling_cur_freq_info) {
      printf("Something wrong with the path !\n");
      cpu_cores_freq.emplace_back(0); // in case of a problem with specific core maybe?
      printf("Core stop : %zu\n", core);
      continue;
    }

    unsigned int current_cpu_core_freq_kHz = 0;
    fscanf(scaling_cur_freq_info, "%d", &current_cpu_core_freq_kHz);
    
    unsigned int current_cpu_core_freq_mHz = (current_cpu_core_freq_kHz / 1000);
    cpu_cores_freq.emplace_back(current_cpu_core_freq_mHz);

    fclose(scaling_cur_freq_info);
  }

  return cpu_cores_freq;
}
