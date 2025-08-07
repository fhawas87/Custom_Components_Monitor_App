#include <stdio.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <vector>

#include <sensors/sensors.h>

std::vector<double> get_cpu_cores_temperatures() {

  if (sensors_init(NULL) != 0) {
    printf("INITIALIZING ERROR\n");
    return {};
  }

  std::vector<double> cpu_cores_temp;
  int chip_index = 0;
  const sensors_chip_name* chip_name;
  char chip_buffer[64];

  while ((chip_name = sensors_get_detected_chips(NULL, &chip_index)) != NULL) {
    sensors_snprintf_chip_name(chip_buffer, sizeof(chip_buffer), chip_name);
    if (std::strcmp(chip_buffer, "coretemp-isa-0000") != 0) {
      continue;
    }
    int feature_index = 0;
    const sensors_feature* feature;

    while ((feature = sensors_get_features(chip_name, &feature_index)) != NULL) {
      if (feature->type != SENSORS_FEATURE_TEMP) { continue; }

      char* label = sensors_get_label(chip_name, feature);
      bool is_core = label && std::strstr(label, "Core");
      free(label);
      if (!is_core) { continue; }
      //if (std::strstr(feature->name, "Core") == nullptr) { continue; }

      const sensors_subfeature* sub_feature = sensors_get_subfeature(chip_name, feature, SENSORS_SUBFEATURE_TEMP_INPUT);
      if (!sub_feature) { continue; }

      double current_core_temperature = 0.0;
      if (sensors_get_value(chip_name, sub_feature->number, &current_core_temperature) == 0) {
        cpu_cores_temp.emplace_back(current_core_temperature);
      }
    }

    break;
  }

  sensors_cleanup();

  return cpu_cores_temp;
}

int main() {

  std::vector<double> cpu_cores_temp = get_cpu_cores_temperatures();

  if (cpu_cores_temp.empty()) {
    printf("Error while reading temps from cores\n");
    return 1;
  }
 
  for (int core = 0; core < cpu_cores_temp.size(); core++) {
    printf("Core %d : %1.f C\n", core, cpu_cores_temp.at(core));
  }
  
  cpu_cores_temp.clear();

  return 0;
}
