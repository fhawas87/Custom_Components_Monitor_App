#pragma once

#ifdef __cplusplus
  #include <vector>
  extern "C" {
#endif

unsigned int                get_current_gpu_temperature(void);
unsigned int                get_core_utilization_percentage_rate(void);
unsigned int                get_gpu_clock_frequency(void);
unsigned int                get_gpu_fan_speed(void);
unsigned int                get_gpu_power_usage(void);

#ifdef __cplusplus
  }
  std::vector<unsigned int>   get_gpu_VRAM_info();
#endif


