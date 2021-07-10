#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int p): pid_(p) {
    SetCpuUtilization();
};

int Process::Pid() { return pid_; }

void Process::SetCpuUtilization() { 
  long active = LinuxParser::ActiveJiffies(pid_);
  long total = LinuxParser::UpTime(pid_) * sysconf(_SC_CLK_TCK);

  cpuUsage_ = (active - prevActive_) / (float)(total - prevTotal_);
  
  prevTotal_ = total;
  prevActive_ = active;
}

float Process::CpuUtilization() { return cpuUsage_; }

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Status() { return LinuxParser::Status(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const { 
    return (cpuUsage_ > a.cpuUsage_); 
}

bool Process::operator==(Process const& a) const {
    return (pid_ == a.pid_);
}

bool Process::operator==(int const& a) const {
    return (pid_ == a);
}
