#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization(std::string cpu) { 
  
  int cpu_no = 0;
  if (cpu == "cpu0"){
      cpu_no = 1;
  }
  else if(cpu == "cpu1"){
      cpu_no = 2;
  }
  long active = LinuxParser::ActiveJiffies(cpu);
  long total = LinuxParser::Jiffies(cpu);
  
  float cpu_usage = (active - prevActive_[cpu_no]) / (float)(total - prevTotal_[cpu_no]);
  prevTotal_[cpu_no] = total;
  prevActive_[cpu_no] = active;
  
  return cpu_usage;
}