#pragma once

#include <nvml.h>

#ifdef __cplusplus
  #include <string>
  extern "C" {
#endif

nvmlDevice_t get_device(void);
unsigned int get_current_gpu_temperature(void);

#ifdef __cplusplus
  }
  std::string get_accessible_device_name();
#endif
