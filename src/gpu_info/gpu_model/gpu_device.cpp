#include <stdio.h>

#include <nvml.h>


nvmlDevice_t get_device() {
  nvmlDevice_t device;
  nvmlReturn_t device_result = nvmlDeviceGetHandleByIndex_v2(0, &device);

  if (device_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", static_cast<int>(device_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(device_result));
    return nullptr;
  }
  return device;
}
