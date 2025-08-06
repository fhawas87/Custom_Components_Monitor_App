#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <nvml.h>

#include "gpu_model/gpu_model.h"
#include "gpu_params/gpu_params.h"

// trying to initialize nvml
// check for errors or success

bool has_initialization_suceed = false;

void try_to_initialize() {
  
  nvmlReturn_t initialize_result;
  initialize_result = nvmlInit_v2();

  if (initialize_result == NVML_SUCCESS) {
    has_initialization_suceed = true;
    printf("Result code : (%d)\n", static_cast<int>(initialize_result));
    printf("Result info : '%s'\n", nvmlErrorString(initialize_result));
    printf("NVML has been properly initialized\n\n"); 

    // RETRIEVING GPU PARAMETERS HERE !

    unsigned int current_gpu_temp                           = get_current_gpu_temperature();
    unsigned int current_gpu_core_usage                     = get_core_utilization_percentage_rate();
    std::string gpu_device_name                             = get_accessible_device_name();
    std::vector<unsigned long long> gpu_VRAM_memory_info    = get_gpu_VRAM_info();
    unsigned long long total_VRAM_installed_MiB             = gpu_VRAM_memory_info.at(0);
    unsigned long long total_VRAM_used_MiB                  = gpu_VRAM_memory_info.at(1);
    unsigned long long total_VRAM_free_MiB                  = gpu_VRAM_memory_info.at(2);
    float VRAM_memory_usage                                 = gpu_VRAM_memory_info.at(3);
    


    for (int i = 0; i < gpu_device_name.length(); i++) {
      printf("%c", gpu_device_name[i]);
    }
    printf("\n");
    printf("Temperature            : %d C\n", current_gpu_temp);
    printf("GPU Core usage         : %d %\n", current_gpu_core_usage);
    printf("VRAM memory usage      : %0.f %\n", VRAM_memory_usage);
    printf("Total VRAM installed   : %llu MiB\n", total_VRAM_installed_MiB);
    printf("Total VRAM used        : %llu MiB\n", total_VRAM_used_MiB);
    printf("Total VRAM free        : %llu MiB\n", total_VRAM_free_MiB);

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

int main() {
  
  try_to_initialize();

  if (!has_initialization_suceed) {
    return 1;
  }

  return 0;
}

