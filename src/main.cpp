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
#include "cpu_model/cpu_model.h"
#include "cpu_params/cpu_params.h"

//RAM HEADERS
#include "ram_params.h"

//FAN HEADERS
#include "fan_params.h"


void gpu_info(const std::string& gpu_model_name, nvmlReturn_t initialize_result) {
  
  if (initialize_result == NVML_SUCCESS) {
    printf("Result code : (%d)\n", static_cast<int>(initialize_result));
    printf("Result info : '%s'\n", nvmlErrorString(initialize_result));
    printf("NVML has been properly initialized\n\n"); 


    std::vector<unsigned int>             gpu_VRAM_memory_info                       = get_gpu_VRAM_info();
    unsigned int                          current_gpu_temp                           = get_current_gpu_temperature();
    unsigned int                          current_gpu_core_usage                     = get_core_utilization_percentage_rate();
    unsigned int                          current_gpu_clock_freq                     = get_gpu_clock_frequency();
    unsigned int                          current_gpu_fan_speed                      = get_gpu_fan_speed();
    unsigned int                          current_gpu_power_usage                    = get_gpu_power_usage();

    unsigned int                          total_VRAM_installed_MiB                   = gpu_VRAM_memory_info.at(0);
    unsigned int                          current_VRAM_used_MiB                      = gpu_VRAM_memory_info.at(1);
    unsigned int                          current_VRAM_free_MiB                      = gpu_VRAM_memory_info.at(2);
    float                                 current_VRAM_memory_usage                  = gpu_VRAM_memory_info.at(3);
    
//                                  TEST PRINT
// <------------------------------------------------------------------------------>
    //for (int i = 0; i < gpu_device_name.length(); i++) {
    //  printf("%c", gpu_device_name[i]);
    //}
    printf("%s\n", gpu_model_name.c_str());
    printf("\n");
    printf("Temperature            : %d C\n", current_gpu_temp);
    printf("GPU Core usage         : %d %%\n", current_gpu_core_usage);
    printf("VRAM memory usage      : %0.f %%\n", current_VRAM_memory_usage);
    printf("Total VRAM installed   : %d MiB\n", total_VRAM_installed_MiB);
    printf("Total VRAM used        : %d MiB\n", current_VRAM_used_MiB);
    printf("Total VRAM free        : %d MiB\n", current_VRAM_free_MiB);
    printf("GPU clock frequency    : %d MHz\n", current_gpu_clock_freq);
    printf("GPU fan speed          : %d RPM\n", current_gpu_fan_speed);
    printf("GPU power usage        : %d W\n", current_gpu_power_usage);
    
    gpu_VRAM_memory_info.clear();
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
}

void cpu_info(const std::string& cpu_model_name) {

  std::vector<double>                     cpu_cores_temperatures                     = get_cpu_cores_temperatures();
  size_t                                  number_of_cores                            = cpu_cores_temperatures.size();
  std::vector<unsigned int>               cpu_cores_frequencies                      = get_cpu_cores_frequencies(number_of_cores);
  double                                  cpu_utilization_percentage_value           = get_cpu_utilization();

  if (cpu_cores_temperatures.empty()) {

    printf("Problem with reading cpu cores temperatures !\n");
  }
  //                                    TEST PRINT
  // <------------------------------------------------------------------------>
  printf("\n%s\n", cpu_model_name.c_str());
  printf("\nCPU utilization        : %0.f%%\n", cpu_utilization_percentage_value);

  for (int core = 0; core < cpu_cores_temperatures.size(); core++) {

    printf("Core %d                 : %0.f C   %d MHz\n", core, cpu_cores_temperatures.at(core), cpu_cores_frequencies.at(core));
  }

  cpu_cores_temperatures.clear();
  cpu_cores_frequencies.clear();
}

void ram_info() {

  unsigned int ram_usage = get_ram_memory_usage();

  printf("\nRAM usage              : %d %%\n", ram_usage);
}

void fan_info() {

  std::vector<double>                     fans_tach_info                             = get_available_fans_speed();

  if (fans_tach_info.size() == 0) { printf("\nEMPTY VECTOR\n") ;}

  for (int fan = 0; fan < fans_tach_info.size(); fan++) {
    if (fans_tach_info.at(fan) == 0) { continue; }
    printf("\nFan %d                  : %0.f RPM", fan, fans_tach_info.at(fan));
  }
  printf("\n");
}

int main() {
  
  nvmlReturn_t initialize_result;
  initialize_result = nvmlInit_v2();
  if (sensors_init(NULL) != 0) {
    printf("\nERROR code : libsensors initialization problem\n");
  }

  std::string gpu_model_name = get_accessible_device_name();
  std::string cpu_model_name = get_cpu_model_name();
  
  gpu_info(gpu_model_name, initialize_result);
  cpu_info(cpu_model_name);
  ram_info();
  fan_info();
  
  nvmlShutdown();
  sensors_cleanup();

  return 0;
}

