#include "low-math.h"

// Absolute value return as float
float toPos(float x){
  if (x>=0) return x;
  return x*(-1);
}

/*!
  \brief        calculate the Newton–Raphson method with 3 steps
  \param[in]    x: 'a' in Newton–Raphson formula ( number to be rooted )
  \attention    formula: x_(n+1) = (1/2)(x_(n) + a/x_(n)) {15 iteration->accuracy ~10^8}
  \param[out]   x(n+1): approximated value of square root (result)
  \retval       float
*/
float newtonSqrt(float x){
  float val;                          // x0 -> guess baes on conditions..
  if (x<0) return 0;                  // ..1. only real numbers..
  else if (x < 1) val = 0.99;         // ..2. guess less than 1 if root term also less than 1
  else val = x/2.0;                   // ..3. if larger, guess half..

  // calculate the Newton–Raphson method with 15 iterations { accurate upto ~10^8}
  for (int i=0; i<15; i++){
    val = 1.0/2.0 * (val + x/val);
  }
  return val;
}

int32_t ssqrt(int32_t n) {
  int64_t val = n << 10;
  unsigned long temp, g=0, b = 0x8000, bshft = 15;
  do {
    if (val >= (temp = (((g << 1) + b)<<bshft--))) {
      g += b;
      val -= temp;
    }
  } while (b >>= 1);
  return g;
}