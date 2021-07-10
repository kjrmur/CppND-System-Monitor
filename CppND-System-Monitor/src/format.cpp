#include <string>
#include <iomanip> 

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int h = seconds / (60 * 60);
    seconds -= h * (60 * 60);

    int m = seconds / 60;
    seconds -= m * 60;

    std::stringstream out;
    out << std::setfill('0') << std::setw(2) << h << ':' 
            << std::setw(2) << m << ':' << std::setw(2) << seconds;

    return out.str();
}