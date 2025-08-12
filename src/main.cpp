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

#define MAX_SAMPLES_HISTORY 200

int current_sample_index = -1;

std::vector<float>                              gpu_temp_ring;
std::vector<float>                              gpu_usage_ring;
std::vector<float>                              gpu_freq_ring;
std::vector<float>                              gpu_fan_speed_ring;
std::vector<float>                              gpu_power_usage_ring;
std::vector<float>                              gpu_vram_usage_ring;
std::vector<float>                              cpu_util_ring;
std::vector<float>                              ram_usage_ring;
std::vector<std::vector<float>>                cpu_temps_ring;
std::vector<std::vector<float>>                fans_speed_ring;
std::vector<std::vector<unsigned int>>          cpu_freqs_ring;

size_t number_of_cores = 0;
size_t number_of_available_fans = 0;

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

static inline void manage_ring_data_dec(std::vector<float> &vec, float val) { // HERE I THINK I WILL HAVE TO CLEAR VECTOR IF SIZE == MAX_SAMPLES_HISTORY
  if (vec.size() >= MAX_SAMPLES_HISTORY) { 
    vec.erase(vec.begin());
  }
  vec.emplace_back(val);
}

static inline void manage_ring_data_vec_int(std::vector<std::vector<unsigned int>> &vec, const std::vector<unsigned int> &val) {
  if (vec.size() >= MAX_SAMPLES_HISTORY) {
    vec.erase(vec.begin());
  }
  vec.emplace_back(val);
}

static inline void manage_ring_data_vec_double(std::vector<std::vector<float>> &vec, const std::vector<double> &val) {
  if (vec[0].size() >= MAX_SAMPLES_HISTORY) {
    //vec.erase(vec.begin());
    for (int i = 0; i < number_of_cores; i++) {
      vec[i].erase(vec[i].begin());
      //vec[i].emplace_back((float)val.at(i));
    }
    current_sample_index = MAX_SAMPLES_HISTORY;
  }
  for (int i = 0; i < number_of_cores; i++) {
    vec[i].emplace_back((float)val.at(i));
  }
  //vec.emplace_back(val);
  //current_sample_index++;
}

static inline stats get_samples() {
  stats current_stats{};
                     
  current_stats.gpu.temp            = get_current_gpu_temperature();                    manage_ring_data_dec(gpu_temp_ring, (float)current_stats.gpu.temp); 
  current_stats.gpu.usage           = get_core_utilization_percentage_rate();           manage_ring_data_dec(gpu_usage_ring, (float)current_stats.gpu.usage);
  current_stats.gpu.freq            = get_gpu_clock_frequency();                        manage_ring_data_dec(gpu_freq_ring, (float)current_stats.gpu.freq);
  current_stats.gpu.fan_speed       = get_gpu_fan_speed();                              manage_ring_data_dec(gpu_fan_speed_ring, (float)current_stats.gpu.fan_speed);
  current_stats.gpu.power_usage     = get_gpu_power_usage();                            manage_ring_data_dec(gpu_power_usage_ring, (float)current_stats.gpu.power_usage);
  current_stats.gpu.vram_info       = get_gpu_VRAM_info();                          
  current_stats.gpu.vram_usage      = current_stats.gpu.vram_info.at(3);                manage_ring_data_dec(gpu_vram_usage_ring, (float)current_stats.gpu.vram_usage);

  current_stats.cpu.model           = get_cpu_model_name();                         
  current_stats.cpu.temps           = get_cpu_cores_temperatures();                     //manage_ring_data_vec_double(cpu_temps_ring, current_stats.cpu.temps);
  number_of_cores                   = current_stats.cpu.temps.size();
  for (int i = 0; i < number_of_cores; i++) {
    double current_core_value = current_stats.cpu.temps.at(i);
    cpu_temps_ring.at(i).emplace_back(current_core_value);
  }
                                                                                        manage_ring_data_vec_double(cpu_temps_ring, current_stats.cpu.temps);
  //current_stats.cpu.freqs           = get_cpu_cores_frequencies(number_of_cores);       manage_ring_data_vec_int(cpu_freqs_ring, current_stats.cpu.freqs);
  current_stats.cpu.util            = get_cpu_utilization();                            manage_ring_data_dec(cpu_util_ring, (float)current_stats.cpu.util);

  current_stats.other.ram_usage     = get_ram_memory_usage();                           manage_ring_data_dec(ram_usage_ring, (float)current_stats.other.ram_usage);
  //current_stats.other.fans_speed    = get_available_fans_speed();                       manage_ring_data_vec_double(fans_speed_ring, current_stats.other.fans_speed);
  number_of_available_fans          = current_stats.other.fans_speed.size();

  return current_stats;
}

static inline void draw_gpu_chart(std::string &gpu_model) {
  if (ImGui::Begin(gpu_model.c_str())) {
    if (ImPlot::BeginPlot("GPU Temperature")) {
      ImPlot::SetupAxes("t[s]", "C");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 25, 80, ImGuiCond_Always);
      if (!gpu_temp_ring.empty()) {
        ImPlot::PlotLine("GPU Temperature", gpu_temp_ring.data(), (int)gpu_temp_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("GPU Usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!gpu_usage_ring.empty()) {
        ImPlot::PlotLine("GPU Usage", gpu_usage_ring.data(), (int)gpu_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("GPU Frequqncy")) {
      ImPlot::SetupAxes("t[s]", "MHz");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 3000, ImGuiCond_Always);
      if (!gpu_freq_ring.empty()) {
        ImPlot::PlotLine("GPU Frequency", gpu_freq_ring.data(), (int)gpu_freq_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("VRAM Usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!gpu_vram_usage_ring.empty()) {
        ImPlot::PlotLine("VRAM Usage", gpu_vram_usage_ring.data(), (int)gpu_vram_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("GPU Power Usage")) {
      ImPlot::SetupAxes("t[s]", "W");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!gpu_power_usage_ring.empty()) {
        ImPlot::PlotLine("GPU Power Usage", gpu_power_usage_ring.data(), (int)gpu_power_usage_ring.size());
      }
      ImPlot::EndPlot();
    }
  }
  ImGui::End();
}

//static inline generate_and_fill_vectors(std::vector<std::vector<double>>& vec, )

static inline void draw_cpu_chart(std::string &cpu_model) { 
  if (ImGui::Begin(cpu_model.c_str())) {
    
    //std::vector<double> current_ring_values = cpu_temps_ring.back(); // BECAUSE ALWAYS LAST VECTOR INDEX CONTAINS CURRENT DATA WITH RING APPROACH
    for (int core = 0; core < number_of_cores; core++) {
      //std::vector<double> current_core_ring_value = cpu_temps_ring.at(i).back();
      std::string current_core_index = "Core " + std::to_string(core);
      if (ImPlot::BeginPlot(current_core_index.c_str())) {
        ImPlot::SetupAxes("t[s]", "C");
        ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 90, ImGuiCond_Always);
        if (!cpu_temps_ring.empty()) {
          //std::vector<float> current_core_ring_vec = cpu_temps_ring[core];
          ImPlot::PlotLine(current_core_index.c_str(), cpu_temps_ring[core].data(), cpu_temps_ring[core].size());
        }
      }
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("CPU Usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!cpu_util_ring.empty()) {
        ImPlot::PlotLine("CPU Usage", cpu_util_ring.data(), cpu_util_ring.size());
      }
      ImPlot::EndPlot();
    }
  ImGui::End();
  }
}

static inline void draw_ram_chart() {
  if (ImGui::Begin("RAM MEMORY")) {
    if (ImPlot::BeginPlot("RAM Usage")) {
      ImPlot::SetupAxes("t[s]", "%");
      ImPlot::SetupAxesLimits(0, MAX_SAMPLES_HISTORY, 0, 100, ImGuiCond_Always);
      if (!ram_usage_ring.empty()) {
        ImPlot::PlotLine("RAM Usage", ram_usage_ring.data(), ram_usage_ring.size());
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

static void ligh_plot_light(const ImVec4 &ACCENT) {
  ImPlotStyle &ps = ImPlot::GetStyle();
  ps.LineWeight = 2.0f;
  ps.Colors[ImPlotCol_PlotBg]   = ImVec4(0.98f, 0.99f, 1.0f, 1.0f);
  ps.Colors[ImPlotCol_AxisText] = ImVec4(0.12f, 0.12f, 0.14, 1.0f);
  ps.Colors[ImPlotCol_AxisGrid] = ImVec4(0, 0, 0, 0.10f);
  ps.Colors[ImPlotCol_FrameBg]  = ImVec4(1, 1, 1, 1);

  static bool colormap_installed = false;
  if (!colormap_installed) {
    ImVec4 mono[2] = { ACCENT };
    ImPlot::AddColormap("MonoAccent", mono, 2);
    ImPlot::PushColormap("MonoAccent");
    colormap_installed = true;
  }
}


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
  ImGui::StyleColorsLight();
  const ImVec4 Blue = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
  ligh_plot_light(Blue);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
  
  double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  
    double current_time = glfwGetTime();
    if (current_time - last_time >= 1.0) {
      stats current_stats = get_samples();
      current_sample_index++;
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

  nvmlShutdown();
  sensors_cleanup();

  return 0;
}

