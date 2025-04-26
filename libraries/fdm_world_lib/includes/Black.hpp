#pragma once
#ifndef FDM_WORLD_LIB_BLACK_HPP
#define FDM_WORLD_LIB_BLACK_HPP

#include <algorithm>
#include <cmath>

#include "inlines.hpp"  // IWYU pragma: keep
#include "specialFunctions.hpp"

using std::max;

//	class
class Black {
 public:
  //	call
  template <class V>
  static V call(V expiry,  //	in years
                V strike, V forward, V volatility);

  //	vega
  template <class V>
  static V vega(V expiry, V strike, V forward, V volatility);

  //	implied
  static double implied(double expiry, double strike, double price,
                        double forward);
};

//	call
template <class V>
V Black::call(V expiry, V strike, V forward, V volatility) {
  if (expiry <= 0.0) return max(0.0, forward - strike);

  V std = volatility * sqrt(expiry);

  V d1 = log(forward / strike) / std + std;
  V d2 = log(forward / strike) / std - std;

  V temp1,temp2;
  V D1 = SpecialFunctions::normalPdf(d1, temp1);
  V D2 = SpecialFunctions::normalPdf(d2, temp2);

  return forward * D1 - strike * D2;
}

//	vega
template <class V>
V Black::vega(V expiry,V strike, V forward, V volatility) {
  if (expiry <= 0.0) return 0.0;

  V std = volatility * sqrt(expiry);

  V d1 = log(forward / strike) / std + std;

  V st = sqrt(expiry);

  V res = st * forward * SpecialFunctions::normalPdf(d1);

  //	done
  return res;
}

#endif  // FDM_WORLD_LIB_BACHELIER_HPP