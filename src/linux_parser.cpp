#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T> T getKey(T filepath, string searchKey){
  	string line, key, val;
  	std::ifstream stream(filepath);
    if (stream.is_open()) {
    while(std::getline(stream, line)){
    	std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == searchKey) {
          return val;
        }
      }
    }
    }
  return val;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string val;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "PRETTY_NAME") {
          std::replace(val.begin(), val.end(), '_', ' ');
          return val;
        }
      }
    }
  }
  return val;
}

// DONE: An example of how to read data from the filesystem
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

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, val;
  float memTotal, memFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key >> val) {
        if (key == "MemTotal") {
          memTotal = std::stof(val);
        }
      if (key == "MemFree") {
          memFree = std::stof(val);
      }
    }
  }
    return ((memTotal - memFree)/memTotal); //see https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, upTime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
    stream.close();
  }
    return std::stol(upTime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line, key;
  int sum{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
  	while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key;
      if (key=="cpu"){
        int val;
      	while(linestream >> val){
        	sum+=val;
        }
        return sum;
      }
    }  
  }
  return sum;
  }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, val;
  vector<long> vals;
  string path = kProcDirectory + to_string(pid) + kStatFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> val) {
      vals.push_back(stol(val));
    }
  }
  return vals[13] + vals[14]; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
vector<string> jiffies = CpuUtilization();
return stol(jiffies[CPUStates::kNice_]) +
  stol(jiffies[CPUStates::kUser_]) +
  stol(jiffies[CPUStates::kIRQ_]) +
  stol(jiffies[CPUStates::kSystem_]) +
  stol(jiffies[CPUStates::kSoftIRQ_]) + 
  stol(jiffies[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
vector<string> cpu = CpuUtilization();
  return stol(cpu[CPUStates::kIdle_]) + stol(cpu[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, _, val;
  vector<string> vals;
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> _ ;
    while (linestream >> val){
      vals.push_back(val);
      }
  }
  return vals;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, val;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "processes") {
          stream.close();
          return std::stoi(val);
        }
      }
    }
  }
  return -1;
  }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  return std::stoi(getKey(kProcDirectory + kStatFilename, "procs_running"));
  }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, command);
  }
  return command; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  return getKey(kProcDirectory + to_string(pid) + kStatusFilename, "VmSize:"); //todo: evtl. umrechnen
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  return getKey(kProcDirectory + to_string(pid) + kStatusFilename, "Uid:");; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line, username, _, uid;
  std::ifstream stream(kPasswordPath); // stored in /etc/passwd
  if(stream.is_open()){
    while(std::getline(stream, line)){
      // format is name:x:uid:... -> replace ":" by blank character
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> username >> _ >> uid){
        if(uid == LinuxParser::Uid(pid)){
        return username;
        }
      }
    }
  }
  return "not known";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  // linux stores data in /proc/[pid]/stat
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, val;
  vector<string> vals;
  
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> val){
      vals.push_back(val);
    }
  }
    return LinuxParser::UpTime() - (0.01 * std::stol(vals[21])); }
