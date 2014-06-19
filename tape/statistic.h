
#ifndef STATISTIC_H
#define STATISTIC_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>


static const double EulerGamma = 0.577215664901532;

inline 
double H(double n) {
  const double x1 = 1.0 / (2.0 * n);
  const double x2 = x1 / (6.0 * n);
  const double x3 = x2 / (10.0 * n * n);
  return log(n) + EulerGamma + x1 - x2 + x3;
}

class zipf {
private:
  double ln_n;
  double c; //inverse of H(n) 

public:
  zipf(uint64_t size) : ln_n(log(double(size))),  c(1.0 / H(double(size)))  {}

  void reset(uint64_t size) {
    ln_n = log(double(size));
    c = 1.0 / H(double(size));
  }

  double p(uint64_t i) {
    return c / double(i);
  }
   
  uint64_t random() {
    double r = drand48();
    return uint64_t(floor(exp(r * ln_n + EulerGamma * (r - 1))));
  }
};

class exponential {
private:
  double mu;

public:
  exponential(double x)  : mu(x) {}

  void reset(double x) {
    mu = x;
  }

  double random() {
    return -mu * log(1 - drand48());
  }
};

class bernoulli {
private:
  double p;

public:
  bernoulli(double p) : p(p) {}

  void reset(double p_new) {
    p = p_new;
  }
    
  bool random() {
    return drand48() < p;
  }
};
    
#endif



