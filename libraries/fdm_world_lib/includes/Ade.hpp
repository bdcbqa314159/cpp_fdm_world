#pragma once
#ifndef FDM_WORLD_LIB_AED_HPP
#define FDM_WORLD_LIB_AED_HPP

#include "mFiniteDifference.hpp"
#include "mMatrixAlgebra.hpp"

//	class declaration
template <class V>
class kAde {
 public:
  //	init
  void init(int numV, const mVector<V>& x);

  //	1st order operators
  static void dx(const mVector<V>& x, const mMatrix<V>& dxd,
                 const mMatrix<V>& dxu, mMatrix<V>& Dxd, mMatrix<V>& Dxu);

  //	2nd order operators
  static void dxx(const mVector<V>& x, const mMatrix<V>& dxd,
                  const mMatrix<V>& dxu, mMatrix<V>& Dxxd, mMatrix<V>& Dxxu);

  //	calc A row
  static void calcArow(const V& r2,  //	r/2
                       const V& mu,
                       const V& var2,  //	var/2
                       const int row,
                       const int col,  //	center col
                       const mMatrix<V>* Dx, const mMatrix<V>& Dxx,
                       mMatrix<V>& A);

  //	calc A operator
  void calcA(int wind, mMatrix<V>& Ad, mMatrix<V>& Au) const;

  //	calc B = 1 + dt A
  static void calcB(const V& one, const V& dt,
                    const int col,  //	center col
                    const mMatrix<V>& A, mMatrix<V>& B);

  //	roll bwd
  void rollBwd(const V& dt, int wind, mVector<mVector<V> >& res);

  //	roll fwd
  void rollFwd(const V& dt, int wind, mVector<mVector<V> >& res);

  //	x, r, mu, var
  mVector<V> myX, myR, myMu, myVar;

  //	res
  mVector<mVector<V> > myRes;

  //	helpers
  mMatrix<V> mydxd, mydxu, myDxd, myDxu, myDxxd, myDxxu;

  //	operators
  mMatrix<V> myAd, myAu, myBd, myBu;

  //	helpers
  mVector<V> myVe;
  mVector<mVector<V> > myResd, myResu;
};

//	init
template <class V>
void kAde<V>::init(int numV, const mVector<V>& x) {
  //	set x
  myX = x;

  //	dim
  int m = myX.size();

  //	resize
  myR.resize(m, 0.0);
  myMu.resize(m, 0.0);
  myVar.resize(m, 0.0);
  myRes.resize(numV, mVector<V>(m, 0.0));

  //	construct operators
  kFiniteDifference::dxd(myX, mydxd);
  kFiniteDifference::dxu(myX, mydxu);

  //	calc operators
  dx(myX, mydxd, mydxu, myDxd, myDxu);
  dxx(myX, mydxd, mydxu, myDxxd, myDxxu);

  //	helpers
  myVe.resize(m);
  myResd.resize(numV);
  myResu.resize(numV);

  //	done
  return;
}

//	1st order operators
template <class V>
void kAde<V>::dx(const mVector<V>& x, const mMatrix<V>& dxd,
                 const mMatrix<V>& dxu, mMatrix<V>& Dxd, mMatrix<V>& Dxu) {
  //	resize
  Dxd.resize(x.size(), 2);
  Dxu.resize(x.size(), 2);

  //	tjek
  if (!x.size()) return;

  //	dim
  int m = x.size() - 1;
  int n = 2;

  //	helps
  V lamd, lamu;
  int i, j;
  for (i = 0; i <= m; ++i) {
    if (0 < i && i < m) {
      lamu = (x(i) - x(i - 1)) / (x(i + 1) - x(i - 1));
      lamd = 1.0 - lamu;
    } else {
      lamd = lamu = 1.0;
    }
    for (j = 0; j < n; ++j) {
      Dxd(i, j) = lamd * dxd(i, j);
      Dxu(i, j) = lamu * dxu(i, j);
    }
  }

  //	done
  return;
}

//	2nd order operators
template <class V>
void kAde<V>::dxx(const mVector<V>& x, const mMatrix<V>& dxd,
                  const mMatrix<V>& dxu, mMatrix<V>& Dxxd, mMatrix<V>& Dxxu) {
  //	resize
  Dxxd.resize(x.size(), 2);
  Dxxu.resize(x.size(), 2);

  //	tjek
  if (!x.size()) return;

  //	dims
  int m = x.size() - 1;
  int n = 2;

  //	helps
  V lam;
  int i, j;

  //	fill
  for (i = 0; i <= m; ++i) {
    if (0 < i && i < m) {
      lam = 2.0 / (x(i + 1) - x(i - 1));
    } else {
      lam = 0.0;
    }
    for (j = 0; j < n; ++j) {
      Dxxd(i, j) = -lam * dxd(i, j);
      Dxxu(i, j) = lam * dxu(i, j);
    }
  }

  //	done
  return;
}

//	calc A row
template <class V>
void kAde<V>::calcArow(const V& r2,  //	r/2
                       const V& mu,
                       const V& var2,  //	var/2
                       const int row,
                       const int col,  //	center col
                       const mMatrix<V>* Dx, const mMatrix<V>& Dxx,
                       mMatrix<V>& A) {
  //	dim
  int n = Dxx.cols();

  //	helps
  int j;

  //	set values
  for (j = 0; j < n; ++j) {
    A(row, j) = var2 * Dxx(row, j);
    if (Dx) A(row, j) += mu * (*Dx)(row, j);
  }
  A(row, col) -= r2;

  //	done
  return;
}

//	calc A operator
template <class V>
void kAde<V>::calcA(int wind, mMatrix<V>& Ad, mMatrix<V>& Au) const {
  //	resize
  Ad.resize(myX.size(), 2);
  Au.resize(myX.size(), 2);

  //	helps
  int i;
  V r2, mu, var2;

  //	tjek
  const mMatrix<V>* Dxd = nullptr;
  const mMatrix<V>* Dxu = nullptr;
  if (wind == 0) {
    Dxd = &myDxd;
    Dxu = &myDxu;
  } else if (wind == -1) {
    Dxd = &mydxd;
  } else if (wind == 1) {
    Dxu = &mydxu;
  }

  for (i = 0; i < myX.size(); ++i) {
    r2 = 0.5 * myR(i);
    mu = myMu(i);
    var2 = 0.5 * myVar(i);
    if (wind == 2) {
      Dxd = nullptr;
      Dxu = nullptr;
      if (mu < 0.0) {
        Dxd = &mydxd;
      } else if (0.0 < mu) {
        Dxu = &mydxu;
      }
    }
    calcArow(r2, mu, var2, i, 1, Dxd, myDxxd, Ad);
    calcArow(r2, mu, var2, i, 0, Dxu, myDxxu, Au);
  }

  //	done
  return;
}

//	calc B = 1 + dt A
template <class V>
void kAde<V>::calcB(const V& one, const V& dt,
                    const int col,  //	center col
                    const mMatrix<V>& A, mMatrix<V>& B) {
  B.resize(A.rows(), A.cols());

  //	helps
  int i, j;
  for (i = 0; i < A.rows(); ++i) {
    for (j = 0; j < A.cols(); ++j) {
      B(i, j) = dt * A(i, j);
    }
    B(i, col) += one;
  }

  //	done
  return;
}

//	roll bwd
template <class V>
void kAde<V>::rollBwd(const V& dt, int wind, mVector<mVector<V> >& res) {
  //	tjek
  if (!myX.size()) return;

  //	dim
  int numV = res.size();

  //	helps
  int h, i;

  //	calc A
  calcA(wind, myAd, myAu);

  //	explicit (u) and implicit (d) roll back
  calcB(1.0, dt, 0, myAu, myBu);
  calcB(1.0, -dt, 1, myAd, myBd);
  for (h = 0; h < numV; ++h) {
    mMatrixAlgebra::banmul(myBu, 0, 1, res(h), myVe);
    mMatrixAlgebra::leftdag(myBd, myVe, myResd(h));
  }

  //	explicit (d) and implicit (u) roll back
  calcB(1.0, dt, 1, myAd, myBd);
  calcB(1.0, -dt, 0, myAu, myBu);
  for (h = 0; h < numV; ++h) {
    mMatrixAlgebra::banmul(myBd, 1, 0, res(h), myVe);
    mMatrixAlgebra::rightdag(myBu, myVe, myResu(h));
  }

  //	set result
  for (h = 0; h < numV; ++h) {
    const mVector<V>& resd = myResd(h);
    const mVector<V>& resu = myResu(h);
    mVector<V>& resh = res(h);
    for (i = 0; i < myX.size(); ++i) {
      resh(i) = 0.5 * (resd(i) + resu(i));
    }
  }

  //	done
  return;
}

//	roll fwd
template <class V>
void kAde<V>::rollFwd(const V& dt, int wind, mVector<mVector<V> >& res) {
  //	tjek
  if (!myX.size()) return;

  //	dims
  int numV = res.size();

  //	helps
  int h, i;

  //	calc A
  calcA(wind, myBd, myBu);
  mMatrixAlgebra::transpose(myBd, 1, 0, myAu);
  mMatrixAlgebra::transpose(myBu, 0, 1, myAd);

  //	implicit (u) and explicit (d) roll
  calcB(1.0, -dt, 0, myAu, myBu);
  calcB(1.0, dt, 1, myAd, myBd);
  for (h = 0; h < numV; ++h) {
    myResd(h) = res(h);
    mMatrixAlgebra::rightdag(myBu, myResd(h), myVe);
    mMatrixAlgebra::banmul(myBd, 1, 0, myVe, myResd(h));
  }

  //	implicit (d) and explicit (d) roll
  calcB(1.0, -dt, 1, myAd, myBd);
  calcB(1.0, dt, 0, myAu, myBu);
  for (h = 0; h < numV; ++h) {
    myResu(h) = res(h);
    mMatrixAlgebra::leftdag(myBd, myResu(h), myVe);
    mMatrixAlgebra::banmul(myBu, 0, 1, myVe, myResu(h));
  }

  //	set result
  for (h = 0; h < numV; ++h) {
    const mVector<V>& resd = myResd(h);
    const mVector<V>& resu = myResu(h);
    mVector<V>& resh = res(h);
    for (i = 0; i < myX.size(); ++i) {
      resh(i) = 0.5 * (resd(i) + resu(i));
    }
  }

  //	done
  return;
}


#endif  // FDM_WORLD_LIB_AED_HPP