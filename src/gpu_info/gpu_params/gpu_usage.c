#include <stdio.h>

#include <nvml.h>

#include "gpu_model/gpu_model.h"


unsigned int get_core_utilization_percentage_rate(void) {

  nvmlDevice_t device = get_device();
  nvmlUtilization_t core_utilization_rate;
  nvmlReturn_t core_utilization_rate_result = nvmlDeviceGetUtilizationRates(device, &core_utilization_rate);

  if (core_utilization_rate_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)core_utilization_rate_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(core_utilization_rate_result));
    return 1;
  }

  return core_utilization_rate.gpu;
}
