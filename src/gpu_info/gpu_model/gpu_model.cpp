#include <stdio.h>
#include <string>

#include <nvml.h>

#include "gpu_model.h"



std::string get_accessible_device_name() {
  
  std::string device_name_string;
  unsigned int accessible_device_counter = 0;
  nvmlReturn_t device_count_result = nvmlDeviceGetCount_v2(&accessible_device_counter);

  if (accessible_device_counter == 0) {
    printf("ERROR code : (%d)\n", static_cast<int>(device_count_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(device_count_result));
    device_name_string = "1";
  }
  else if (accessible_device_counter != 1) {
    printf("ERROR code : (%d)\n", static_cast<int>(device_count_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(device_count_result));
    device_name_string = "1";
  }
  else {
    nvmlDevice_t device = get_device();

    char device_name[NVML_DEVICE_NAME_BUFFER_SIZE];
    device_name[NVML_DEVICE_NAME_BUFFER_SIZE - 1] = '\0';
    nvmlDeviceGetName(device, device_name, NVML_DEVICE_NAME_BUFFER_SIZE);
  
    char* char_pointer = device_name;
    while (*char_pointer != '\0') {
      device_name_string.push_back(*char_pointer);
      char_pointer++;
    }
  }

  return device_name_string;
}



