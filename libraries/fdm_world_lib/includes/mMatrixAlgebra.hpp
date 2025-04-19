#pragma once
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
}  // namespace mMatrixAlgebra

#endif  // FDM_WORLD_LIB_MMATRIX_ALGEBRA_HPP