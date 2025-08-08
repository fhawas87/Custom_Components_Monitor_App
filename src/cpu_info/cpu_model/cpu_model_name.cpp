#include <cpuid.h> // ONLY WORKS FOR x86/x86-64 FROM WHAT I'VE READ
#include <string.h>
#include <string>


std::string get_cpu_model_name() {

  char cpu_brand_name[49];
  unsigned int cpu_info[4] = {0, 0, 0, 0};
  unsigned int limit_address = __get_cpuid_max(0x80000000, NULL);

  if (limit_address < 0x80000004) { return cpu_brand_name; }

  __cpuid(0x80000002, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
  memcpy(cpu_brand_name, cpu_info, 16);
  __cpuid(0x80000003, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
  memcpy(cpu_brand_name + 16, cpu_info, 16);
  __cpuid(0x80000004, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
  memcpy(cpu_brand_name + 32, cpu_info, 16);

  cpu_brand_name[48] = '\0';

  return std::string(cpu_brand_name);
}
