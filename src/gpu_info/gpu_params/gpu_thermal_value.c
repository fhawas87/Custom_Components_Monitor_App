#include <stdio.h>

#include <nvml.h>

#include "gpu_model/gpu_model.h"



unsigned int get_current_gpu_temperature(void) {

  nvmlDevice_t device = get_device();
  unsigned int gpu_temp = 0;
  nvmlReturn_t gpu_temp_result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &gpu_temp);
  
  if (gpu_temp_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)gpu_temp_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(gpu_temp_result));
  }

  return gpu_temp;
}
