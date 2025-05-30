#ifndef UTILS_H
#define UTILS_H

#include <Print.h>

inline void printDigits(Print& output, int digits) {
  if (digits <10) output.print("0");
  output.print(digits);
}

#endif