#pragma once
#ifndef FDM_WORLD_LIB_BACHELIER_HPP
#define FDM_WORLD_LIB_BACHELIER_HPP

#include <algorithm>
#include <cmath>

#include "inlines.hpp"  // IWYU pragma: keep
#include "specialFunctions.hpp"
#include "mVector.hpp"

using std::max;

//	class
class Bachelier {
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

  static bool fdRunner(const double s0, const double r, const double mu,
                       const double sigma, const double expiry,
                       const double strike, const bool dig,
                       const int pc,      //	put (-1) call (1)
                       const int ea,      //	european (0), american (1)
                       const int smooth,  //	smoothing
                       const double theta, const int wind, const double numStd,
                       const int numt, const int numx, const bool update,
                       const int numPr, double& res0, mVector<double>& s,
                       mVector<double>& res, std::string& error);
};

//	call
template <class V>
V Bachelier::call(V expiry, V strike, V forward, V volatility) {
  if (expiry <= 0.0) return max(0.0, forward - strike);

  V std = volatility * sqrt(expiry);
  V x = (forward - strike) / std;
  V pdf, res;
  res = (forward - strike) * SpecialFunctions::normalCdf(x, pdf);
  res += std * pdf;

  //	done
  return res;
}

//	vega
template <class V>
V Bachelier::vega(V expiry, V strike, V forward, V volatility) {
  if (expiry <= 0.0) return 0.0;

  V st = sqrt(expiry);
  V x = (forward - strike) / (volatility * st);
  V res = st * SpecialFunctions::normalPdf(x);

  //	done
  return res;
}

#endif  // FDM_WORLD_LIB_BACHELIER_HPP