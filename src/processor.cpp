#include "processor.h"

#include "linux_parser.h"


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long allJiffies = LinuxParser::Jiffies();
  long activeJiffies = LinuxParser::ActiveJiffies();
  return 1.0*activeJiffies/allJiffies;
}