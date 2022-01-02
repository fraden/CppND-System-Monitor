#include <sstream>
#include <iomanip>
#include <string>

#include "format.h"

using std::string;
using std::stringstream;
using std::setw;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int minutes, hours;

    minutes = seconds / 60 % 60;
    hours = seconds / 3600 % 100;
    seconds = seconds % 60;

    std::string hh, mm, ss;
  	stringstream time;
    hh = std::to_string(hours);
    mm = std::to_string(minutes);
    ss = std::to_string(seconds);

    // source: https://www.cplusplus.com/reference/iomanip/setfill/
    time << std::setfill('0') << std::setw(2) << hh << ":" 
      << std::setfill('0') << std::setw(2) << mm << ":" 
      << std::setfill('0') << std::setw(2) << ss;
    return time.str();
}