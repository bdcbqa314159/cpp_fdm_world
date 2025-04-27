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
void tridag(const mMatrixView<V>& A, const mVectorView<V>& r, mVectorView<V>& u,
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
            mVector<V>& gam) {
  int n = A.rows();
  if (u.size() < n) u.resize(n);
  if (gam.size() < n) gam.resize(n);

  auto A_ = A();
  auto r_ = r();
  auto u_ = u();
  auto gam_ = gam();

  tridag(A(), r(), u_, gam_);
  return;
}

template <class V>
void banmul(const mMatrixView<V>& A, int m1, int m2, const mVectorView<V>& b,
            mVectorView<V>& x) {
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
  auto b_ = b();
  auto x_ = x();
  banmul(A(), m1, m2, b_, x_);
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

//	transposing a ban matrix
template <class V>
void transpose(const mMatrix<V>& A, const int m1, const int m2,
               mMatrix<V>& At) {
  //	resize
  At.resize(A.rows(), A.cols());

  //	loop
  int n = A.rows() - 1;
  int i, j, k, l, jl, ju;
  for (i = 0; i <= n; ++i) {
    jl = max(0, i - m1);
    ju = min(i + m2, n);
    for (j = jl; j <= ju; ++j) {
      k = j - i + m1;
      l = i - j + m2;
      At(j, l) = A(i, k);
    }
  }

  //	done
  return;
}

//	solve: A u = r where A is left diag
template <class V>
void leftdag(const mMatrix<V>& A,  //	nx2
             const mVector<V>& r, mVector<V>& u) {
  //	resize
  u.resize(A.rows());

  //	tjek
  if (!A.rows()) return;

  //	dims
  int n = A.rows() - 1;

  //	helps
  int i;

  //	loop
  u(0) = r(0) / A(0, 1);
  for (i = 1; i <= n; ++i) {
    u(i) = (r(i) - A(i, 0) * u(i - 1)) / A(i, 1);
  }

  //	done
  return;
}

//	solve: A u = r where A is right diag
template <class V>
void rightdag(const mMatrix<V>& A,  //	nx2
              const mVector<V>& r, mVector<V>& u) {
  //	resize
  u.resize(A.rows());

  //	tjek
  if (!A.rows()) return;

  //	dims
  int n = A.rows() - 1;

  //	helps
  int i;

  //	loop
  u(n) = r(n) / A(n, 0);
  for (i = n - 1; i >= 0; --i) {
    u(i) = (r(i) - A(i, 1) * u(i + 1)) / A(i, 0);
  }

  //	done
  return;
}

}  // namespace mMatrixAlgebra

#endif  // FDM_WORLD_LIB_MMATRIX_ALGEBRA_HPP