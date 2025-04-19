#pragma once
#ifndef FDM_WORLD_LIB_SPECIAL_FUNCTIONS_HPP
#define FDM_WORLD_LIB_SPECIAL_FUNCTIONS_HPP

#include <cmath>

#include "constants.hpp"

class SpecialFunctions {
 public:
  template <class T>
  static T normalPdf(T x);

  template <class T>
  static T normalPolynomial(T x);

  template <class T>
  static T normalCdf(T x, T& pdf);
};

template <class T>
T SpecialFunctions::normalPdf(T x) {
  return Constants::oneOverSqrt2Pi() * std::exp(-0.5 * x * x);
}

template <class T>
T SpecialFunctions::normalPolynomial(T x) {
  const double p = 0.23164'1900;
  const double b1 = 0.31938'1530;
  const double b2 = -0.35656'3782;
  const double b3 = 1.78147'7937;
  const double b4 = -1.82125'5978;
  const double b5 = 1.33027'4429;

  T t = 1. / (1. + p * x);
  T result = t * (b1 + t * (b2 + t * (b3 + t * (b4 + t * b5))));

  return result;
}

template <class T>
T SpecialFunctions::normalCdf(T x, T& pdf) {
  pdf = normalPdf(x);
  T result = pdf * normalPolynomial(fabs(x));

  if (x > 0.) {
    result = 1. - result;
  }

  return result;
}

#endif  // FDM_WORLD_LIB_SPECIAL_FUNCTIONS_HPP