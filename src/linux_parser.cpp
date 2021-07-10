#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <experimental/filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for(auto& file: std::experimental::filesystem::directory_iterator(kProcDirectory.c_str())) {
    auto path = file.path();
    if(path.has_filename()) {
      string filename = path.filename();
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float total_mem, free_mem = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total_mem = stof(value);
        }
        if (key == "MemFree:") {
          free_mem = stof(value);
        }
      }
    }
  }
  return (total_mem - free_mem) / total_mem;
}

long LinuxParser::UpTime() { 
  string time, line;
  long uptime = 0;
  
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> time;
    uptime = std::stol(time);
  }
  return uptime;
 }

long LinuxParser::Jiffies(string cpu) { 
  auto values = LinuxParser::CpuUtilization(cpu);
  long total = 0;
  for (int i = 0; i < kGuest_; i++) {
    total += stol(values[i]);
  }
  return total;
 }

long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }

  long total_time = 0;
  for (int i = 13; i < 17; i++) {
    total_time += stol(values[i]);
  }

  return total_time;
}

long LinuxParser::ActiveJiffies(string cpu) { 
  auto values = LinuxParser::CpuUtilization(cpu);
  long active = stol(values[kUser_]) + stol(values[kNice_]) + stol(values[kSystem_]) + stol(values[kIRQ_]) + stol(values[kSoftIRQ_]) + stol(values[kSteal_]);
  return active;
 }

long LinuxParser::IdleJiffies(string cpu) { 
  auto values = LinuxParser::CpuUtilization(cpu);
  long tot_idle = stol(values[kIdle_]) + stol(values[kIOwait_]);
  return tot_idle;
 }

vector<string> LinuxParser::CpuUtilization(string cpu) { 
  string line, value;
  vector<string> values;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> value;
      if (value == cpu) {
        while (linestream >> value) {
          values.push_back(value);
        }
        return values;
      }
    }
  }
  return values;
 }

int LinuxParser::TotalProcesses() { 
  string line, key, value;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
 }

int LinuxParser::RunningProcesses() {
  string line, key, value;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) { 
  string line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) { 
  string line, key, value;
  float kb_mem = 0;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          kb_mem = stof(value);
        }
      }
    }
  }
  std::ostringstream out;
  out.precision(2);
  out << std::setfill(' ') << std::setw(8) << std::fixed << (kb_mem/1024);

  return out.str(); 
}

string LinuxParser::Status(int pid) { 
  string line, key, value;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "State:") {
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Uid(int pid) { 
  string line, key, value;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;  
}

string LinuxParser::User(int pid) { 
  string line, name, skip, value;
  string uid = LinuxParser::Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> skip >> value) {
        if (value == uid) {
          return name;
        }
      }
    }
  }
  return name;
 }

long LinuxParser::UpTime(int pid) { 
  long sys_uptime = LinuxParser::UpTime();
  string line, value;
  vector<string> values;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  float starttime = stof(values[21]) / sysconf(_SC_CLK_TCK);
  return (sys_uptime - starttime);
 }

