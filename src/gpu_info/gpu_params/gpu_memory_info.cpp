#include <stdio.h>
#include <vector>

#include <nvml.h>

#include "gpu_model/gpu_model.h"



std::vector<unsigned long long> get_gpu_VRAM_info() {

  std::vector<unsigned long long> memory_info;

  nvmlDevice_t device = get_device();
  nvmlMemory_t memory;
  nvmlReturn_t memory_result = nvmlDeviceGetMemoryInfo(device, &memory);

  if (memory_result != NVML_SUCCESS) {
    printf("ERROR code : ( %d )\n", static_cast<int>(memory_result));
    printf("ERROR info : '%s'\n", nvmlErrorString(memory_result));
    return {};
  }
  float VRAM_memory_percentage_usage             = 100 * ((float)memory.used / (float)memory.total);
  unsigned long long total_VRAM_installed_MiB    =     (memory.total / 1024 / 1024);
  unsigned long long total_VRAM_used_MiB         =     (memory.used / 1024 / 1024);
  unsigned long long total_VRAM_free_MiB         =     (memory.free / 1204 / 1024);
  memory_info.emplace_back(total_VRAM_installed_MiB);
  memory_info.emplace_back(total_VRAM_used_MiB);
  memory_info.emplace_back(total_VRAM_free_MiB);
  memory_info.emplace_back(VRAM_memory_percentage_usage);

  return memory_info;
}

