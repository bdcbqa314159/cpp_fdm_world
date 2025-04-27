#pragma once
#ifndef FDM_WORLD_LIB_FD1D_HPP
#define FDM_WORLD_LIB_FD1D_HPP

#include "mFiniteDifference.hpp"
#include "mMatrixAlgebra.hpp"

//	class declaration
template <class V>
class kFd1d {
 public:
  //	init
  void init(int numV, const mVector<V>& x, bool log);

  const mVector<V>& r() const { return myR; }
  const mVector<V>& mu() const { return myMu; }
  const mVector<V>& var() const { return myVar; }
  const mVector<V>& x() const { return myX; }
  const mVector<mVector<V>>& res() const { return myRes; }

  mVector<V>& r() { return myR; }
  mVector<V>& mu() { return myMu; }
  mVector<V>& var() { return myVar; }
  mVector<V>& x() { return myX; }
  mVector<mVector<V>>& res() { return myRes; }

  //	operator
  void calcAx(V one, V dtTheta, int wind, bool tr, mMatrix<V>& A) const;

  //	roll bwd
  void rollBwd(V dt, bool update, V theta, int wind, mVector<mVector<V>>& res);

  //	roll fwd
  void rollFwd(V dt, bool update, V theta, int wind, mVector<mVector<V>>& res);

 private:
  //	r, mu, var
  mVector<V> myX, myR, myMu, myVar;

  //	diff operators
  mMatrix<V> myDxd, myDxu, myDx, myDxx;

  //	operator matrix
  mMatrix<V> myAe, myAi;

  //	helper
  mVector<V> myVs, myWs;

  //	vector of results
  mVector<mVector<V>> myRes;
};

//	init
template <class V>
void kFd1d<V>::init(int numV, const mVector<V>& x, bool log) {
  myX = x;
  myRes.resize(numV);
  for (int k = 0; k < numV; ++k) myRes[k].resize(myX.size());

  //	resize params
  myR.resize(myX.size(), 0.0);
  myMu.resize(myX.size(), 0.0);
  myVar.resize(myX.size(), 0.0);

  kFiniteDifference::dx(-1, myX, myDxd);
  kFiniteDifference::dx(0, myX, myDx);
  kFiniteDifference::dx(1, myX, myDxu);
  kFiniteDifference::dxx(myX, myDxx);

  if (myX.empty()) return;

  int numC = myDx.cols();

  //	log transform case
  if (log) {
    int n = myX.size() - 1;
    for (int i = 1; i < n; ++i) {
      for (int j = 0; j < numC; ++j) myDxx(i, j) -= myDx(i, j);
    }
  }

  myAe.resize(myX.size(), numC);
  myAi.resize(myX.size(), numC);
  myVs.resize(myX.size());
  myWs.resize(myX.size());

  //	done
  return;
}

//	construct operator
template <class V>
void kFd1d<V>::calcAx(V one, V dtTheta, int wind, bool tr,
                      mMatrix<V>& A) const {
  //	dims
  int n = myX.size();
  int m = myDx.cols();
  int mm = m / 2;

  //	resize
  A.resize(n, m);

  //	helps
  int i, j;

  //	wind
  const mMatrix<V>* Dx = 0;
  if (wind < 0)
    Dx = &myDxd;
  else if (wind == 0)
    Dx = &myDx;
  else if (wind == 1)
    Dx = &myDxu;

  //	fill
  for (i = 0; i < n; ++i) {
    if (wind > 1) {
      Dx = myMu(i) < 0.0 ? &myDxd : &myDxu;
    }
    for (j = 0; j < m; ++j) {
      A(i, j) =
          dtTheta * (myMu(i) * (*Dx)(i, j) + 0.5 * myVar(i) * myDxx(i, j));
    }
    A(i, mm) += one - dtTheta * myR(i);
  }

  if (tr) mMatrixAlgebra::transpose(mm, A);

  //	done
  return;
}

//	roll bwd
template <class V>
void kFd1d<V>::rollBwd(V dt, bool update, V theta, int wind,
                       mVector<mVector<V>>& res) {
  //	helps
  int k;

  //	dims
  int n = myX.size();
  int mm = 1;
  int numV = (int)res.size();

  //	explicit
  if (theta != 1.0) {
    if (update) calcAx(1.0, dt * (1.0 - theta), wind, false, myAe);
    for (k = 0; k < numV; ++k) {
      myVs = res[k];
      mMatrixAlgebra::banmul(myAe, mm, mm, myVs, res[k]);
    }
  }

  //	implicit
  if (theta != 0.0) {
    if (update) calcAx(1.0, -dt * theta, wind, false, myAi);
    for (k = 0; k < numV; ++k) {
      myVs = res[k];
      mMatrixAlgebra::tridag(myAi, myVs, res[k], myWs);
    }
  }

  //	done
  return;
}

//	roll fwd
template <class V>
void kFd1d<V>::rollFwd(V dt, bool update, V theta, int wind,
                       mVector<mVector<V>>& res) {
  //	helps
  int k;

  //	dims
  int n = myX.size();
  int mm = myDx.cols() / 2;
  int numV = (int)res.size();

  //	implicit
  if (theta != 0.0) {
    if (update) calcAx(1.0, -dt * theta, wind, true, myAi);
    for (k = 0; k < numV; ++k) {
      myVs = res[k];
      mMatrixAlgebra::tridag(myAi, myVs, res[k], myWs);
    }
  }

  //	explicit
  if (theta != 1.0) {
    if (update) calcAx(1.0, dt * (1.0 - theta), wind, true, myAe);
    for (k = 0; k < numV; ++k) {
      myVs = res[k];
      mMatrixAlgebra::banmul(myAe, mm, mm, myVs, res[k]);
    }
  }

  //	done
  return;
}



#endif  // FDM_WORLD_LIB_FD1D_HPP