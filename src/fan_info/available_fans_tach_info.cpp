#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <sensors/sensors.h>


std::vector<double> get_available_fans_speed() {
  std::vector<double> fan_tach_info;
  int chip_index = 0;
  const sensors_chip_name* chip_name;
  char chip_buffer[64];

  while ((chip_name = sensors_get_detected_chips(NULL, &chip_index)) != NULL) {
    sensors_snprintf_chip_name(chip_buffer, sizeof(chip_buffer), chip_name);
    if (strncmp(chip_buffer, "nct6797-isa", 11) != 0) {
      continue;
    }
    int feature_index = 0;
    const sensors_feature* feature;

    while ((feature = sensors_get_features(chip_name, &feature_index)) != NULL) {
      if (feature->type != SENSORS_FEATURE_FAN) { continue; }
      
      char* label = sensors_get_label(chip_name, feature);
      if (strncmp(label, "fan", 3) != 0) { free(label); continue; }
      free(label);

      const sensors_subfeature* sub_feature = sensors_get_subfeature(chip_name, feature, SENSORS_SUBFEATURE_FAN_INPUT);
      if (!sub_feature) { continue; }

      double current_fan_speed_RPM = 0;
      if (sensors_get_value(chip_name, sub_feature->number, &current_fan_speed_RPM) == 0) {
        fan_tach_info.emplace_back(current_fan_speed_RPM);
      }
    }

    break;
  }

  return fan_tach_info;
}
