#pragma once
#ifndef FDM_WORLD_LIB_FINITE_DIFERENCE_HPP
#define FDM_WORLD_LIB_FINITE_DIFERENCE_HPP

#include "mVector.hpp"
#include "mMatrix.hpp"

class kFiniteDifference {
 public:
  //	1st order diff operator
  template <class V>
  static void dx(const int wind, const mVector<V>& x, mMatrix<V>& dx);

  //	2nd order diff operator
  template <class V>
  static void dxx(const mVector<V>& x, mMatrix<V>& dxx);

  template <class V>
  static void dxd(const mVector<V>& x, mMatrix<V>& out);

  template <class V>
  static void dxu(const mVector<V>& x, mMatrix<V>& out);

  template <class V>
  static V smoothCall(const V& xl, const V& xu,
                                        const V& strike);

  template <class V>
  static V smoothDigital(const V& xl, const V& xu, const V& strike);


};

//	dx operator
template <class V>
void kFiniteDifference::dx(const int wind, const mVector<V>& x,
                           mMatrix<V>& out) {
  out.resize(x.size(), 3);
  if (!x.size()) return;
  int n = x.size() - 1;
  if (wind >= 0) {
    V dxu = x(1) - x(0);
    out(0, 0) = V(0.0);
    out(0, 1) = -1.0 / dxu;
    out(0, 2) = 1.0 / dxu;
  } else {
    out(0, 0) = V(0.0);
    out(0, 1) = V(0.0);
    out(0, 2) = V(0.0);
  }

  for (int i = 1; i < n; ++i) {
    V dxl = x(i) - x(i - 1);
    V dxu = x(i + 1) - x(i);
    if (wind < 0) {
      out(i, 0) = -1.0 / dxl;
      out(i, 1) = 1.0 / dxl;
      out(i, 2) = V(0.0);
    } else if (wind == 0) {
      out(i, 0) = -dxu / dxl / (dxl + dxu);
      out(i, 1) = (dxu / dxl - dxl / dxu) / (dxl + dxu);
      out(i, 2) = dxl / dxu / (dxl + dxu);
    } else {
      out(i, 0) = V(0.0);
      out(i, 1) = -1.0 / dxu;
      out(i, 2) = 1.0 / dxu;
    }
  }

  if (wind <= 0) {
    V dxl = x(n) - x(n - 1);
    out(n, 0) = -1.0 / dxl;
    out(n, 1) = 1.0 / dxl;
    out(n, 2) = V(0.0);
  } else {
    out(n, 0) = V(0.0);
    out(n, 1) = V(0.0);
    out(n, 2) = V(0.0);
  }

  //	done
  return;
}

//	2nd order diff operator
template <class V>
void kFiniteDifference::dxx(const mVector<V>& x, mMatrix<V>& out) {
  out.resize(x.size(), 3);
  if (!x.size()) return;

  int n = x.size() - 1;

  out(0, 0) = V(0.0);
  out(0, 1) = V(0.0);
  out(0, 2) = V(0.0);

  for (int i = 1; i < n; ++i) {
    V dxl = x(i) - x(i - 1);
    V dxu = x(i + 1) - x(i);
    out(i, 0) = 2.0 / (dxl * (dxl + dxu));
    out(i, 1) = -(2.0 / dxl + 2.0 / dxu) / (dxl + dxu);
    out(i, 2) = 2.0 / (dxu * (dxl + dxu));
  }

  out(n, 0) = V(0.0);
  out(n, 1) = V(0.0);
  out(n, 2) = V(0.0);

  //	done
  return;
}


//	dx down
template <class V>
void kFiniteDifference::dxd(const mVector<V>& x,
                mMatrix<V>& out)  //	n x 2
{
  out.resize(x.size(), 2);

  if (!x.size()) return;

  V dxl;

  out(0, 0) = 0.0;
  out(0, 1) = 0.0;
  for (int i = 1; i < x.size(); ++i) {
    dxl = x(i) - x(i - 1);
    out(i, 0) = -1.0 / dxl;
    out(i, 1) = 1.0 / dxl;
  }

  //	done
  return;
}

//	dx up
template <class V>
void kFiniteDifference::dxu(const mVector<V>& x,
                mMatrix<V>& out)  //	n x 2
{
  out.resize(x.size(), 2);

  if (!x.size()) return;

  V dxu;

  int i, n = x.size() - 1;
  for (i = 0; i < n; ++i) {
    dxu = x(i + 1) - x(i);
    out(i, 0) = -1.0 / dxu;
    out(i, 1) = 1.0 / dxu;
  }
  out(n, 0) = 0.0;
  out(n, 1) = 0.0;

  //	done
  return;
}

//	smooth call payoff function
template <class V>
V kFiniteDifference::smoothCall(const V& xl, const V& xu,
                                       const V& strike) {
  V res;
  if (xu <= strike) {
    res = 0.0;
  } else if (strike <= xl) {
    res = 0.5 * (xl + xu) - strike;
  } else {
    res = 0.5 * Inlines::sqr(xu - strike) / (xu - xl);
  }

  //	done
  return res;
}

//	smooth digital payoff function
template <class V>
V kFiniteDifference::smoothDigital(const V& xl, const V& xu,
                                          const V& strike) {
  V res;
  if (xu <= strike) {
    res = 0.0;
  } else if (strike <= xl) {
    res = 1.0;
  } else {
    res = (xu - strike) / (xu - xl);
  }

  //	done
  return res;
}

#endif  // FDM_WORLD_LIB_FINITE_DIFERENCE_HPP