#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
using std::map;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { 
    //remove any deleted processes
    auto pids = LinuxParser::Pids();
    for (Process p: processes_){
        auto it = find(pids.begin(), pids.end(), p.Pid());
        if(it == pids.end()){
            remove(processes_.begin(), processes_.end(), p);
        }
    }
    //add new or update existing processes
    for (int p: pids){
        auto it = find (processes_.begin(), processes_.end(), p);
        if (it == processes_.end()) {
            Process process(p);
            processes_.push_back(process);
        }
        else {
            (*it).SetCpuUtilization();
        }
    }

    std::sort(processes_.begin(), processes_.end(), std::less<Process>());
    return processes_; }

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }