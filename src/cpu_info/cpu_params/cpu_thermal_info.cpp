#include <stdio.h>
#include <string.h> // strncmp()
#include <stdlib.h> // free()
#include <vector>

#include <sensors/sensors.h>


std::vector<float> get_cpu_cores_temperatures() {
  std::vector<float> cpu_cores_temp;
  int chip_index = 0;
  const sensors_chip_name* chip_name;
  char chip_buffer[64];

  while ((chip_name = sensors_get_detected_chips(NULL, &chip_index)) != NULL) {
    sensors_snprintf_chip_name(chip_buffer, sizeof(chip_buffer), chip_name);
    if (strncmp(chip_buffer, "coretemp-isa-0000", 17) != 0) {
      continue;
    }
    int feature_index = 0;
    const sensors_feature* feature;

    while ((feature = sensors_get_features(chip_name, &feature_index)) != NULL) {
      if (feature->type != SENSORS_FEATURE_TEMP) { continue; }

      char* label = sensors_get_label(chip_name, feature);
      if (strncmp(label, "Package", 7) == 0) { free(label); continue ;}
      free(label);
  
      const sensors_subfeature* sub_feature = sensors_get_subfeature(chip_name, feature, SENSORS_SUBFEATURE_TEMP_INPUT);
      if (!sub_feature) { continue; }

      double current_core_temperature = 0.0;
      if (sensors_get_value(chip_name, sub_feature->number, &current_core_temperature) == 0) {
        cpu_cores_temp.emplace_back((float)current_core_temperature);
      }
    }

    break;
  }

  return cpu_cores_temp;
}

