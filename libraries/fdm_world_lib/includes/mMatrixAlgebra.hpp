#pragma once
#include "inlines.hpp"
#include "mVector.hpp"
#ifndef FDM_WORLD_LIB_MMATRIX_ALGEBRA_HPP
#define FDM_WORLD_LIB_MMATRIX_ALGEBRA_HPP

#include "mMatrix.hpp"

namespace mMatrixAlgebra {
//	mat mult res = A * B
template <class U, class V, class W>
void mmult(const mMatrix<U>& a, const mMatrix<V>& b, mMatrix<W>& ab) {
  //	dims
  int m1 = a.rows();
  int m2 = a.cols();
  int m3 = b.cols();

  ab.resize(m1, m3, 0.0);

  //	calc
  for (int i = 0; i < m1; ++i) {
    for (int k = 0; k < m2; ++k) {
      for (int j = 0; j < m3; ++j) ab(i, j) += a(i, k) * b(k, j);
    }
  }

  //	done
  return;
}

template <class V>
void tridag(const mMatrixView<V>& A, const mVectorView<V>& r, mVector<V>& u,
            mVectorView<V>& gam) {
  V bet = A(0, 1);
  int n = A.rows();

  u(0) = r(0) / bet;

  for (int j = 1; j < n; ++j) {
    gam(j) = A(j - 1, 2) / bet;
    bet = A(j, 1) - A(j, 0) * gam(j);
    u(j) = (r(j) - A(j, 0) * u(j - 1)) / bet;
  }

  for (int j = n - 2; j >= 0; --j) {
    u(j) -= gam(j) * u(j + 1);
  }

  return;
}

template <class V>
void tridag(const mMatrix<V>& A, const mVector<V>& r, mVector<V>& u,
            mVectorView<V>& gam) {
  int n = A.rows();
  if (u.size() < n) u.resize(n);
  if (gam.size() < n) gam.resize(n);

  tridag(A(), r(), u(), gam());
  return;
}

template <class V>
void banmul(const mMatrixView<V>& A, int m1, int m2, const mVectorView<V>& b,
            mVector<V>& x) {
  int n = A.rows() - 1;
  V xi;
  for (int i = 0; i <= n; ++i) {
    int jl = max<int>(0, i - m1);
    int ju = min<int>(n, i + m2);
    xi = 0;

    for (int j = jl; j <= ju; ++j) {
      int k = j - i + m1;
      xi += A(i, k) * b(j);
    }

    x(i) = xi;
  }

  return;
}

template <class V>
void banmul(const mMatrix<V>& A, int m1, int m2, const mVector<V>& b,
            mVector<V>& x) {
  int n = A.rows() - 1;
  x.resize(n + 1);
  banmul(A(), m1, m2, b(), x());
  return;
}

template <class V>
void transpose(const int mm, mMatrix<V>& A) {
  int n = A.rows() - 1;

  for (int i = 0; i <= n; ++i) {
    int jl = max(0, i - mm);
    for (int j = jl; j < i; ++j) {
      int k = j - i + mm;
      int l = i - j + mm;
      Inlines::swap(A(i, k), A(j, l));
    }
  }
}

}  // namespace mMatrixAlgebra

#endif  // FDM_WORLD_LIB_MMATRIX_ALGEBRA_HPP