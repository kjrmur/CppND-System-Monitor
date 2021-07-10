#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

class Processor {
 public:
  float Utilization(std::string cpu);

 private:
  long prevActive_[3] = {0, 0, 0};
  long prevTotal_[3] = {0, 0, 0};
};

#endif