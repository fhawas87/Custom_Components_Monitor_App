#include <stdio.h>

#include <nvml.h>


nvmlDevice_t get_device(void) {
  nvmlDevice_t device;
  nvmlReturn_t device_result = nvmlDeviceGetHandleByIndex_v2(0, &device);

  if (device_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", (int)device_result);
    printf("ERROR info : '%s'\n", nvmlErrorString(device_result));
    return NULL;
  }
  return device;
}
