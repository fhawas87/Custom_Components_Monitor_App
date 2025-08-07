#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <nvml.h>
#include <sensors/sensors.h>

// GPU HEADERS
#include "gpu_model/gpu_model.h"
#include "gpu_params/gpu_params.h"

//CPU HEADERS
#include "cpu_params/cpu_params.h"



bool has_initialization_suceed = false;


void gpu_info() {
  
  nvmlReturn_t initialize_result;
  initialize_result = nvmlInit_v2();

  if (initialize_result == NVML_SUCCESS) {
    has_initialization_suceed = true;
    printf("Result code : (%d)\n", static_cast<int>(initialize_result));
    printf("Result info : '%s'\n", nvmlErrorString(initialize_result));
    printf("NVML has been properly initialized\n\n"); 


    std::vector<unsigned long long>       gpu_VRAM_memory_info                       = get_gpu_VRAM_info();
    std::string                           gpu_device_name                            = get_accessible_device_name();

    unsigned int                          current_gpu_temp                           = get_current_gpu_temperature();
    unsigned int                          current_gpu_core_usage                     = get_core_utilization_percentage_rate();
    unsigned int                          current_gpu_clock_freq                     = get_gpu_clock_frequency();
    unsigned int                          current_gpu_fan_speed                      = get_gpu_fan_speed();
    unsigned int                          current_gpu_power_usage                    = get_gpu_power_usage();

    unsigned long long                    total_VRAM_installed_MiB                   = gpu_VRAM_memory_info.at(0);
    unsigned long long                    current_VRAM_used_MiB                      = gpu_VRAM_memory_info.at(1);
    unsigned long long                    current_VRAM_free_MiB                      = gpu_VRAM_memory_info.at(2);
    float                                 current_VRAM_memory_usage                  = gpu_VRAM_memory_info.at(3);
    
//                                  TEST PRINT
// <------------------------------------------------------------------------------>
    for (int i = 0; i < gpu_device_name.length(); i++) {
      printf("%c", gpu_device_name[i]);
    }

    printf("\n");
    printf("Temperature            : %d C\n", current_gpu_temp);
    printf("GPU Core usage         : %d %%\n", current_gpu_core_usage);
    printf("VRAM memory usage      : %0.f %%\n", current_VRAM_memory_usage);
    printf("Total VRAM installed   : %llu MiB\n", total_VRAM_installed_MiB);
    printf("Total VRAM used        : %llu MiB\n", current_VRAM_used_MiB);
    printf("Total VRAM free        : %llu MiB\n", current_VRAM_free_MiB);
    printf("GPU clock frequency    : %d MHz\n", current_gpu_clock_freq);
    printf("GPU fan speed          : %d RPM\n", current_gpu_fan_speed);
    printf("GPU power usage        : %d W\n", current_gpu_power_usage);

  }
  else {
    if (initialize_result == NVML_ERROR_DRIVER_NOT_LOADED) {
      printf("\nNvidia driver is not runnig\n");
      printf("ERROR code : (%d)\n", static_cast<int>(initialize_result));
      printf("ERROR info : '%s'\n", nvmlErrorString(initialize_result));
    }
    if (initialize_result == NVML_ERROR_NO_PERMISSION) {
      printf("\nNVML does not have permission to communicate to the driver\n");
      printf("ERROR code : (%d)\n", static_cast<int>(initialize_result));
      printf("ERROR info : '%s'\n", nvmlErrorString(initialize_result));
    }
    if (initialize_result == NVML_ERROR_UNKNOWN) {
      printf("\nUnknown error !\n");
      printf("ERROR code : (%d)\n", static_cast<int>(initialize_result));
      printf("ERROR info : '%s'\n", nvmlErrorString(initialize_result));
    }
  }
  nvmlShutdown();
}

void cpu_info() {
  
  if (sensors_init(NULL) != 0) {
    printf("INITIALIZING ERROR !\n");
  }

  std::vector<double> cpu_cores_temperatures = get_cpu_cores_temperatures();
  size_t number_of_cores = cpu_cores_temperatures.size();
  std::vector<unsigned int> cpu_cores_frequencies = get_cpu_cores_frequencies(number_of_cores);

  if (cpu_cores_temperatures.empty()) {

    printf("Problem with reading cpu cores temperatures !\n");
  }
  //                                    TEST PRINT
  // <------------------------------------------------------------------------>
  
  for (int core = 0; core < cpu_cores_temperatures.size(); core++) {

    printf("Core %d : %0.f C   %d mHz\n", core, cpu_cores_temperatures.at(core), cpu_cores_frequencies.at(core));
  }

  cpu_cores_temperatures.clear();
  sensors_cleanup();
}

int main() {
  
  gpu_info();
  cpu_info();

  if (!has_initialization_suceed) {
    return 1;
  }

  return 0;
}

