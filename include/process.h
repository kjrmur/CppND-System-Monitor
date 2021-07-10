#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process();
  Process(int p);
  void SetCpuUtilization();
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization();                  
  std::string Ram();                       
  std::string Status();
  long int UpTime();                       
  bool operator<(Process const& a) const;  
  bool operator==(Process const& a) const;
  bool operator==(int const& a) const;

private:
  int pid_{0};
  float cpuUsage_{0};
  long prevActive_{0};
  long prevTotal_{0};
};

#endif