#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>

#include <nvml.h>
#include <sensors/sensors.h>

#include "gpu_model/gpu_model.h"
#include "gpu_params/gpu_params.h"
#include "cpu_model/cpu_model.h"
#include "cpu_params/cpu_params.h"
#include "ram_params.h"
#include "fan_params.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define MAX_SAMPLES_HISTORY 400

std::vector<float>                              gpu_temp_ring;
std::vector<float>                              gpu_usage_ring;
std::vector<float>                              gpu_freq_ring;
std::vector<float>                              gpu_fan_speed_ring;
std::vector<float>                              gpu_power_usage_ring;
std::vector<float>                              gpu_vram_usage_ring;
std::vector<float>                              cpu_util_ring;
std::vector<float>                              ram_usage_ring;
std::vector<std::vector<double>>                cpu_temps_ring;
std::vector<std::vector<double>>                fans_speed_ring;
std::vector<std::vector<unsigned int>>          cpu_freqs_ring;

int number_of_cores;
int number_of_available_fans = 0;

struct gpu_stats {

  unsigned int temp;
  unsigned int usage;
  unsigned int freq;
  unsigned int fan_speed;
  unsigned int power_usage;
  unsigned int vram_usage;
  std::vector<unsigned int> vram_info;
  std::string model;
};

struct cpu_stats {

  std::vector<double> temps;
  std::vector<unsigned int> freqs;
  std::string model;
  double util;

};

struct other_stats {

  unsigned int ram_usage;
  std::vector<double> fans_speed;
};

struct stats {

  gpu_stats gpu;
  cpu_stats cpu;
  other_stats other;
};

/*
void gpu_info(const std::string& gpu_model_name, nvmlReturn_t initialize_result) {
  
  if (initialize_result == NVML_SUCCESS) {
    printf("Result code : (%d)\n", static_cast<int>(initialize_result));
    printf("Result info : '%s'\n", nvmlErrorString(initialize_result));
    printf("NVML has been properly initialized\n\n"); 


    std::vector<unsigned int>             gpu_VRAM_memory_info                       = get_gpu_VRAM_info();
    unsigned int                          current_gpu_temp                           = get_current_gpu_temperature();
    unsigned int                          current_gpu_core_usage                     = get_core_utilization_percentage_rate();
    unsigned int                          current_gpu_clock_freq                     = get_gpu_clock_frequency();
    unsigned int                          current_gpu_fan_speed                      = get_gpu_fan_speed();
    unsigned int                          current_gpu_power_usage                    = get_gpu_power_usage();

    unsigned int                          total_VRAM_installed_MiB                   = gpu_VRAM_memory_info.at(0);
    unsigned int                          current_VRAM_used_MiB                      = gpu_VRAM_memory_info.at(1);
    unsigned int                          current_VRAM_free_MiB                      = gpu_VRAM_memory_info.at(2);
    float                                 current_VRAM_memory_usage                  = gpu_VRAM_memory_info.at(3);
    
//                                  TEST PRINT
// <------------------------------------------------------------------------------>
    
    printf("------------------------------------------\n");
    printf("%s\n", gpu_model_name.c_str());
    printf("------------------------------------------\n");
    printf("Temperature            : %d°C\n", current_gpu_temp);
    printf("GPU Core usage         : %d %%\n", current_gpu_core_usage);
    printf("VRAM memory usage      : %0.f %%\n", current_VRAM_memory_usage);
    printf("Total VRAM installed   : %d MiB\n", total_VRAM_installed_MiB);
    printf("Total VRAM used        : %d MiB\n", current_VRAM_used_MiB);
    printf("Total VRAM free        : %d MiB\n", current_VRAM_free_MiB);
    printf("GPU clock frequency    : %d MHz\n", current_gpu_clock_freq);
    printf("GPU fan speed          : %d RPM\n", current_gpu_fan_speed);
    printf("GPU power usage        : %d W\n", current_gpu_power_usage);
    
    gpu_VRAM_memory_info.clear();
  }
  else {
    if (initialize_result == NVML_ERROR_DRIVER_NOT_LOADED) {
      printf("\nNvidia driver is not runnig\n");
      printf("ERROR code : (%d)\n", static_cast<int>(initialize_result));
      printf("ERROR info : '%s'\n", nvmlErrorString(initialize_result));
    }
    if (initialize_result == NVML_ERROR_NO_PERMISSION) {
      printf("\nNVML does not have permission to communicate to the driver\n");
      printf("ERROR code : (%d)\n", static_cast<int>(initialize_result));
      printf("ERROR info : '%s'\n", nvmlErrorString(initialize_result));
    }
    if (initialize_result == NVML_ERROR_UNKNOWN) {
      printf("\nUnknown error !\n");
      printf("ERROR code : (%d)\n", static_cast<int>(initialize_result));
      printf("ERROR info : '%s'\n", nvmlErrorString(initialize_result));
    }
  }
}

void cpu_info(const std::string& cpu_model_name) {

  std::vector<double>                     cpu_cores_temperatures                     = get_cpu_cores_temperatures();
  size_t                                  number_of_cores                            = cpu_cores_temperatures.size();
  std::vector<unsigned int>               cpu_cores_frequencies                      = get_cpu_cores_frequencies(number_of_cores);
  double                                  cpu_utilization_percentage_value           = get_cpu_utilization();
  
  if (cpu_cores_temperatures.empty()) {

    printf("Problem with reading cpu cores temperatures !\n");
  }
  //                                    TEST PRINT
  // <------------------------------------------------------------------------>
  
  printf("-------------------------------------------");
  printf("\n%s\n", cpu_model_name.c_str());
  printf("-------------------------------------------");
  printf("\nCPU utilization        : %0.f%%\n", cpu_utilization_percentage_value);

  for (int core = 0; core < cpu_cores_temperatures.size(); core++) {

    printf("Core %d                 : %0.f°C   %d MHz\n", core, cpu_cores_temperatures.at(core), cpu_cores_frequencies.at(core));
  }
  
  cpu_cores_temperatures.clear();
  cpu_cores_frequencies.clear();
}

void ram_info() {

  unsigned int ram_usage = get_ram_memory_usage();
  
  printf("-------------------------------------------\n");
  printf("RAM usage              : %d %%\n", ram_usage);
  printf("-------------------------------------------");
  
}

void fan_info() {

  std::vector<double>                     fans_tach_info                             = get_available_fans_speed();

  if (fans_tach_info.size() == 0) { printf("\nEMPTY VECTOR\n"); }
  
  for (int fan = 0; fan < fans_tach_info.size(); fan++) {
    if (fans_tach_info.at(fan) == 0) { continue; }
    printf("\nFan %d                  : %0.f RPM", fan, fans_tach_info.at(fan));
  }
  printf("\n-------------------------------------------\n");
  
}
*/


static inline void manage_ring_data_dec(std::vector<float> &vec, float val) { // HERE I THINK I WILL HAVE TO CLEAR VECTOR IF SIZE == MAX_SAMPLES_HISTORY
  if (vec.size() == MAX_SAMPLES_HISTORY) { 
    vec.erase(vec.begin());
  }
  vec.emplace_back(val);
}

static inline void manage_ring_data_vec_int(std::vector<std::vector<unsigned int>> &vec, const std::vector<unsigned int> &val) {
  if (vec.size() == MAX_SAMPLES_HISTORY) {
    vec.erase(vec.begin());
  }
  vec.emplace_back(val);
}

static inline void manage_ring_data_vec_double(std::vector<std::vector<double>> &vec, const std::vector<double> &val) {
  if (vec.size() == MAX_SAMPLES_HISTORY) {
    vec.erase(vec.begin());
  }
  vec.emplace_back(val);
}

static inline stats get_samples() {
  stats current_stats{};
                     
  current_stats.gpu.temp            =       get_current_gpu_temperature();                                  manage_ring_data_dec(gpu_temp_ring, (float)current_stats.gpu.temp); 
  current_stats.gpu.usage           =       get_core_utilization_percentage_rate();                         manage_ring_data_dec(gpu_usage_ring, (float)current_stats.gpu.usage);
  current_stats.gpu.freq            =       get_gpu_clock_frequency();                                      manage_ring_data_dec(gpu_freq_ring, (float)current_stats.gpu.freq);
  current_stats.gpu.fan_speed       =       get_gpu_fan_speed();                                            manage_ring_data_dec(gpu_fan_speed_ring, (float)current_stats.gpu.fan_speed);
  current_stats.gpu.power_usage     =       get_gpu_power_usage();                                          manage_ring_data_dec(gpu_power_usage_ring, (float)current_stats.gpu.power_usage);
  current_stats.gpu.vram_info       =       get_gpu_VRAM_info();                          
  current_stats.gpu.vram_usage      =       current_stats.gpu.vram_info.at(3);                              manage_ring_data_dec(gpu_vram_usage_ring, (float)current_stats.gpu.vram_usage);

  current_stats.cpu.model           =       get_cpu_model_name();                         
  current_stats.cpu.temps           =       get_cpu_cores_temperatures();                                   manage_ring_data_vec_double(cpu_temps_ring, current_stats.cpu.temps);
  size_t number_of_cores                   =       current_stats.cpu.temps.size();
  current_stats.cpu.freqs           =       get_cpu_cores_frequencies(number_of_cores);                     manage_ring_data_vec_int(cpu_freqs_ring, current_stats.cpu.freqs);
  current_stats.cpu.util            =       get_cpu_utilization();                                          manage_ring_data_dec(cpu_util_ring, (float)current_stats.cpu.util);

  current_stats.other.ram_usage     =       get_ram_memory_usage();                                         manage_ring_data_dec(ram_usage_ring, (float)current_stats.other.ram_usage);
  current_stats.other.fans_speed    =       get_available_fans_speed();                                     manage_ring_data_vec_double(fans_speed_ring, current_stats.other.fans_speed);

  return current_stats;
}

static inline void draw_gpu_chart(std::string &gpu_model) {
  if (ImGui::Begin(gpu_model.c_str())) {
    if (ImPlot::BeginPlot("temp")) {
      ImPlot::SetupAxes("t[s]", "C");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 25, 80, ImGuiCond_Always);
      if (!gpu_temp_ring.empty()) {
        ImPlot::PlotLine("temp", gpu_temp_ring.data(), (int)gpu_temp_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!gpu_usage_ring.empty()) {
        ImPlot::PlotLine("usage", gpu_usage_ring.data(), (int)gpu_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("freq")) {
      ImPlot::SetupAxes("t[s]", "MHz");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 3000, ImGuiCond_Always);
      if (!gpu_freq_ring.empty()) {
        ImPlot::PlotLine("freq", gpu_freq_ring.data(), (int)gpu_freq_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("VRAM usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!gpu_vram_usage_ring.empty()) {
        ImPlot::PlotLine("VRAM usage", gpu_vram_usage_ring.data(), (int)gpu_vram_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("power usage")) {
      ImPlot::SetupAxes("t[s]", "W");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!gpu_power_usage_ring.empty()) {
        ImPlot::PlotLine("power usage", gpu_power_usage_ring.data(), (int)gpu_power_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
  }
  ImGui::End();
}

static inline void draw_cpu_chart(std::string &cpu_model) {
  /*
  if (ImGui::Begin(cpu_model.c_str())) {
    if (ImPlot::BeginPlot("usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!cpu_util_ring.empty()) {
        ImPlot::PlotLine("usage", cpu_util_ring.data(), cpu_util_ring.size());
      }
      ImPlot::EndPlot();
    }
    for (int i = 0; i < number_of_cores; i++) {
      char core_index[30];
      snprintf(core_index, sizeof(core_index), "Core %d##plot temp", i + 1);
      if (ImPlot::BeginPlot(core_index)) {
        ImPlot::SetupAxes("t[s]", "C");
        ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 90, ImGuiCond_Always);
        if (!cpu_temps_ring.empty()) {
          ImPLot::PlotLine(core_index, cpu_temps_ring.at(i).data(), (int)cpu_temps_ring.size());
        }
        ImPlot::EndPlot();
      }
    }
  }
  ImGui::End();
  */ 
  /*
  if (ImGui::Begin(cpu_model.c_str())) {
    for (int i = 0; i < number_of_cores; i++) {
      //std::vector<double> current_core_values_vec = cpu_temps_ring.back();
      //int current_core_vec_size = cpu_temps_ring.back().size();
      char core_index[30];
      snprintf(core_index, sizeof(core_index), "core %d temp", i + 1);
      if (ImPlot::BeginPlot(core_index)) {
        ImPlot::SetupAxes("t[s]", "C");
        ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 90, ImGuiCond_Always);
        if (!cpu_temps_ring.empty()) {
          ImPlot::PlotLine(core_index, current_core_values_vec.data(), 6);
        }
        ImPlot::EndPlot();
      }
    }
  }
  */
  //ImGui::End();
}

static inline void draw_ram_chart() {
  if (ImGui::Begin("RAM MEMORY")) {
    if (ImPlot::BeginPlot("usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!ram_usage_ring.empty()) {
        ImPlot::PlotLine("usage", ram_usage_ring.data(), ram_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
  }
  ImGui::End();
}
/*
static inline void draw_fan_chart(int &number_of_available_fans) {
  //current_available_fans_value = fans_speed_ring.back();
  std::vector<double> current_available_fans_value = fans_speed_ring.back();
  if (ImGui::Begin("FANS SPEED")) {
    if (ImPlot::BeginPlot("speed")) {
      ImPlot::SetupAxes("t[s]", "RPM");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 3000, ImGuiCond_Always);
      if (!fans_speed_ring.empty()) {
        ImPlot::PlotLine("speed", current_available_fans_value.data(), number_of_available_fans);
      }
      ImPlot::EndPlot();
    }
  }
  ImGui::End();
}
*/
int main() {
  
  nvmlReturn_t initialize_result;
  initialize_result = nvmlInit_v2();
  if (initialize_result != NVML_SUCCESS) {
    printf("\nERROR CODE : ( %d ), info : '%s'\n", (int)initialize_result, nvmlErrorString(initialize_result));
  }
  if (sensors_init(NULL) != 0) {
    printf("ERROR code : libsensors initialization problem\n");
    return -1;
  }

  std::string gpu_model = get_accessible_device_name();
  std::string cpu_model = get_cpu_model_name();
  
  if (!glfwInit()) { return -1; }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(1920, 1080, "Custom Afterburner Application", nullptr, nullptr);

  if (!window) { glfwTerminate(); return -1; }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (glewInit() != GLEW_OK) { return -1; }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
  
  double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  
    double current_time = glfwGetTime();
    if (current_time - last_time >= 1.0) {
      stats current_stats = get_samples();
      number_of_cores = current_stats.cpu.temps.size();
      number_of_available_fans = current_stats.other.fans_speed.size();
      last_time = current_time;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    draw_gpu_chart(gpu_model);
    draw_cpu_chart(cpu_model);
    draw_ram_chart();
    //draw_fan_chart(number_of_available_fans);

    ImGui::Render();
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();



  /*
  std::string gpu_model_name = get_accessible_device_name();
  std::string cpu_model_name = get_cpu_model_name();
  
  gpu_info(gpu_model_name, initialize_result);
  cpu_info(cpu_model_name);
  ram_info();
  fan_info();
  */

  nvmlShutdown();
  sensors_cleanup();

  return 0;
}

