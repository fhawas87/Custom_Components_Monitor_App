#include <stdio.h>

#include <nvml.h>

#include "gpu_model/gpu_model.h"



unsigned int get_gpu_clock_frequency() {

  nvmlDevice_t device = get_device();
  unsigned int core_clock_freq;
  nvmlReturn_t core_clock_freq_result = nvmlDeviceGetClockInfo(device, NVML_CLOCK_GRAPHICS, &core_clock_freq);

  if (core_clock_freq_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", static_cast<int>(core_clock_freq_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(core_clock_freq_result));
    return 1;
  }

  return core_clock_freq;
}

// Didn't test if this function retrieves and calculate fan speed in RPM 
// In my case I would have to overload GPU since it is always 0 at 38-45 degrees
//
// TODO maybe ?!!!

unsigned int get_gpu_fan_speed() {
  
  nvmlDevice_t device = get_device();
  unsigned int fan_speed_percentage_value;
  nvmlReturn_t fan_speed_result = nvmlDeviceGetFanSpeed_v2(device, 0, &fan_speed_percentage_value);
  unsigned int fan_speed_max;
  unsigned int fan_speed_min;
  nvmlReturn_t fan_speed_max_min_result = nvmlDeviceGetMinMaxFanSpeed(device, &fan_speed_min, &fan_speed_max);

  if (fan_speed_result != NVML_SUCCESS || fan_speed_max_min_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", static_cast<int>(fan_speed_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(fan_speed_result));
    return 1;
  }

  float fan_speed_RPM = fan_speed_percentage_value * fan_speed_max;

  return static_cast<unsigned int>(fan_speed_RPM);
}

unsigned int get_gpu_power_usage() {
  
  nvmlDevice_t device = get_device();
  unsigned int power_usage;
  nvmlReturn_t power_usage_result = nvmlDeviceGetPowerUsage(device, &power_usage);

  if (power_usage_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", static_cast<int>(power_usage_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(power_usage_result));
    return 1;
  }

  power_usage /= 1000;

  return power_usage;
}
