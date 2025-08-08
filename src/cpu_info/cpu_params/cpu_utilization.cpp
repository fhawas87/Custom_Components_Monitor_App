#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <thread>


struct cpu_proc_stats {
  unsigned long long user;
  unsigned long long nice;
  unsigned long long system;
  unsigned long long idle;
  unsigned long long iowait;
  unsigned long long irq;
  unsigned long long softirq;
  unsigned long long steal;
  unsigned long long guest;
  unsigned long long guest_nice;
};

cpu_proc_stats fill_struct_of_cpu_stats() {

  FILE *cpu_stats_file_info = fopen("/proc/stat", "r");
  if (!cpu_stats_file_info) {
    printf("Something wrong with the /proc/stat path\n");
    fclose(cpu_stats_file_info);

    return {};
  }

  cpu_proc_stats proc_info_stat_read;
  char stats_buffer[512];
  fgets(stats_buffer, sizeof(stats_buffer), cpu_stats_file_info);
  fclose(cpu_stats_file_info);

  int stats_read = sscanf(stats_buffer,
                    "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                    &proc_info_stat_read.user,
                    &proc_info_stat_read.nice,
                    &proc_info_stat_read.system,
                    &proc_info_stat_read.idle,
                    &proc_info_stat_read.iowait,
                    &proc_info_stat_read.irq,
                    &proc_info_stat_read.softirq,
                    &proc_info_stat_read.steal,
                    &proc_info_stat_read.guest,
                    &proc_info_stat_read.guest_nice);

  return proc_info_stat_read;
}

double get_cpu_utilization() {
  
  // not sure if formula of this cpu utlization calculations is correct
  // but final result in percentage value looks fine

  cpu_proc_stats first_sample = fill_struct_of_cpu_stats();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  cpu_proc_stats second_sample = fill_struct_of_cpu_stats();

  unsigned long long first_sample_sum = first_sample.user    + first_sample.nice + 
                                        first_sample.system  + first_sample.idle +
                                        first_sample.iowait  + first_sample.irq +
                                        first_sample.softirq + first_sample.steal +
                                        first_sample.guest   + first_sample.guest_nice;

  unsigned long long second_sample_sum = second_sample.user    + second_sample.nice +
                                         second_sample.system  + second_sample.idle +
                                         second_sample.iowait  + second_sample.irq +
                                         second_sample.softirq + second_sample.steal +
                                         second_sample.guest   + second_sample.guest_nice;

  unsigned long long idle1_iowait1 = first_sample.idle + first_sample.iowait;
  unsigned long long idle2_iowait2 = second_sample.idle + second_sample.iowait;
  unsigned long long delta_of_two_samples = second_sample_sum - first_sample_sum;
  unsigned long long idle_iowait_delta_of_two_samples = idle2_iowait2 - idle1_iowait1;

  double cpu_utilization = ((static_cast<double>(delta_of_two_samples)) - (static_cast<double>(idle_iowait_delta_of_two_samples))) / delta_of_two_samples * 100;
  
  return cpu_utilization;
}

