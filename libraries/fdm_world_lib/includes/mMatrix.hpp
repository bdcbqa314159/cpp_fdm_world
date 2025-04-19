#pragma once
#ifndef FDM_WORLD_LIB_MMATRIX_HPP
#define FDM_WORLD_LIB_MMATRIX_HPP

#include "mVector.hpp"

template <typename T>
class mMatrixView;

template <typename T = double>
class mMatrix {
 public:
  //	declarations
  using Container = vector<T>;
  using value_type = T;

  //	trivi c'tors
  mMatrix() = default;
  mMatrix(size_t rows, size_t cols)
      : myData(rows * cols), myRows((int)rows), myCols((int)cols) {}
  mMatrix(size_t rows, size_t cols, T t0)
      : myData(rows * cols, t0), myRows((int)rows), myCols((int)cols) {}
  mMatrix(const mMatrix& rhs) = default;
  mMatrix(mMatrix&& rhs) noexcept = default;
  ~mMatrix() noexcept = default;

  //	trivi assign
  mMatrix& operator=(const mMatrix& rhs) = default;
  mMatrix& operator=(mMatrix&& rhs) = default;

  //	assign from single value
  mMatrix& operator=(const T& t) {
    const int s = (int)myData.size();
    for (int i = 0; i < s; ++i) myData[i] = t;
    return *this;
  }

  //	funcs
  int rows() const { return myRows; }
  int cols() const { return myCols; }
  int size() const { return (int)myData.size(); }
  bool empty() const { return size() == 0; }

  //	row,col to idx
  int rcToIdx(int i, int j) const { return i * myCols + j; }
  int rToIdx(int i) const { return i * myCols; }

  //	get element
  const T& operator()(int i, int j) const {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrix row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrix col subscript out of range");
#endif
    return myData[rcToIdx(i, j)];
  }
  T& operator()(int i, int j) {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrix row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrix col subscript out of range");
#endif
    return myData[rcToIdx(i, j)];
  }

  const T& operator[](int i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrix subscript out of range");
#endif
    return myData[i];
  }

  T& operator[](int i) {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrix subscript out of range");
#endif
    return myData[i];
  }

  //	resize
  void resize(size_t rows, size_t cols) { resize(rows, cols, T{}); }
  void resize(size_t rows, size_t cols, const T& t0) {
    if (cols == myCols) {
      myData.resize(rows * cols, t0);
      myRows = (int)rows;
      return;
    }

    Container tmp;
    swap(tmp, myData);
    myData.resize(rows * cols, t0);

    int minR = min<int>((int)rows, myRows);
    int minC = min<int>((int)cols, myCols);
    for (int i = 0; i < minR; ++i) {
      for (int j = 0; j < minC; ++j) {
        myData[i * cols + j] = tmp[i * myCols + j];
      }
    }

    myRows = (int)rows;
    myCols = (int)cols;
  }
  void clear() { resize(0, 0); }

  //	data
  const Container& data() const { return myData; }
  Container& data() { return myData; }

  //	get matrix as vector view
  explicit operator const mVectorView<T>() const {
    return mVectorView<T>(myData);
  }
  explicit operator mVectorView<T>() { return mVectorView<T>(myData); }
  const mVectorView<T> asVector() const { return mVectorView<T>(myData); }
  mVectorView<T> asVector() { return mVectorView<T>(myData); }

  //	get row view (if deep copy is needed, cast to mVector)
  const mMatrixView<T> operator()() const { return mMatrixView<T>(*this); }
  mMatrixView<T> operator()() { return mMatrixView<T>(*this); }
  const mVectorView<T> operator()(int i) const {
    //	since a span is read/write it takes non-const pointer arg in c'tor and
    // we have to use a const_cast for this const method (but returns const
    // span)
    T* t = &(const_cast<mMatrix*>(this)->myData[rToIdx(i)]);
    return mVectorView<T>(t, myCols);
  }
  mVectorView<T> operator()(int i) {
    return mVectorView<T>(&myData[rToIdx(i)], myCols);
  }

 private:
  Container myData;
  int myRows{0};
  int myCols{0};
};

template <typename T>
class mMatrixView {
 public:
  //	declarations
  using view = span<T>;
  using value_type = T;

  //	trivi c'tors
  mMatrixView() noexcept = default;
  mMatrixView(const mMatrixView&) noexcept = default;
  mMatrixView(mMatrixView&&) noexcept = default;
  ~mMatrixView() noexcept = default;

  //	c'tors will make a view on the rhs (i.e. updating values will update the
  // rhs)
  mMatrixView(mMatrix<T>& rhs)
      : myView(rhs.data()), myRows(rhs.rows()), myCols(rhs.cols()) {}
  mMatrixView(const mMatrix<T>& rhs)
      : myView(const_cast<mMatrix<T>&>(rhs).data()),
        myRows(rhs.rows()),
        myCols(rhs.cols()) {}
  mMatrixView(T& rhs) : myView(&rhs, 1), myRows(1), myCols(1) {}
  mMatrixView(const T& rhs)
      : myView(&const_cast<T&>(rhs), 1), myRows(1), myCols(1) {}

  mMatrixView(vector<T>& rhs, size_t rows, size_t cols)
      : myView(rhs), myRows((int)rows), myCols((int)cols) {
#ifdef _DEBUG
    if (myRows * myCols != myView.size())
      throw std::runtime_error(
          "mMatrixView::mMatrixView(vector): Row/col vs view size mismatch");
#endif
  }

  mMatrixView(const vector<T>& rhs, size_t rows, size_t cols)
      : myView(const_cast<vector<T>&>(rhs)),
        myRows((int)rows),
        myCols((int)cols) {
#ifdef _DEBUG
    if (myRows * myCols != myView.size())
      throw std::runtime_error(
          "mMatrixView::mMatrixView(const vector): Row/col vs view size "
          "mismatch");
#endif
  }

  mMatrixView(view& rhs, size_t rows, size_t cols)
      : myView(rhs), myRows((int)rows), myCols((int)cols) {
#ifdef _DEBUG
    if (myRows * myCols != myView.size())
      throw std::runtime_error(
          "mMatrixView::mMatrixView(view): Row/col vs view size mismatch");
#endif
  }

  mMatrixView(T* t, size_t rows, size_t cols)
      : myView(t, rows * cols), myRows((int)rows), myCols((int)cols) {
#ifdef _DEBUG
    if (myRows * myCols != myView.size())
      throw std::runtime_error(
          "mMatrixView::mMatrixView(T*): Row/col vs view size mismatch");
#endif
  }

  mMatrixView(const T* t, size_t rows, size_t cols)
      : myView(const_cast<T*>(t), rows * cols),
        myRows((int)rows),
        myCols((int)cols) {
#ifdef _DEBUG
    if (myRows * myCols != myView.size())
      throw std::runtime_error(
          "mMatrixView::mMatrixView(const T*): Row/col vs view size mismatch");
#endif
  }

  //	trivi assign
  mMatrixView& operator=(const mMatrixView&) noexcept = default;
  mMatrixView& operator=(mMatrixView&&) noexcept = default;

  //	assign from single value
  mMatrixView& operator=(const T& t) {
    const int s = (int)myView.size();
    for (int i = 0; i < s; ++i) myView[i] = t;
    return *this;
  }

  //	funcs
  int rows() const { return myRows; }
  int cols() const { return myCols; }
  int size() const { return (int)myView.size(); }
  bool empty() const { return size() == 0; }

  //	row,col to idx
  int rcToIdx(int i, int j) const { return i * myCols + j; }
  int rToIdx(int i) const { return i * myCols; }

  //	get element
  const T& operator()(int i, int j) const {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrixView row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrixView col subscript out of range");
#endif
    return myView[rcToIdx(i, j)];
  }
  T& operator()(int i, int j) {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrixView row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrixView col subscript out of range");
#endif
    return myView[rcToIdx(i, j)];
  }

  const T& operator[](int i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrixView subscript out of range");
#endif
    return myView[i];
  }

  T& operator[](int i) {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrixView subscript out of range");
#endif
    return myView[i];
  }

  //	get matrix as vector view
  explicit operator const mVectorView<T>() const {
    return mVectorView<T>(myView);
  }
  explicit operator mVectorView<T>() { return mVectorView<T>(myView); }
  const mVectorView<T> asVector() const { return mVectorView<T>(myView); }
  mVectorView<T> asVector() { return mVectorView<T>(myView); }

  //	get row view (if deep copy is needed, cast to mVector)
  const mVectorView<T> operator()(int i) const {
    //	since a span is read/write it takes non-const pointer arg in c'tor and
    // we have to use a const_cast for this const method (but returns const
    // span)
    T* t = &(const_cast<mMatrixView*>(this)->myView[rToIdx(i)]);
    return mVectorView<T>(t, myCols);
  }
  mVectorView<T> operator()(int i) {
    return mVectorView<T>(&myView[rToIdx(i)], myCols);
  }

  const view& data() const { return myView; }
  view& data() { return myView; }

 private:
  view myView;
  int myRows{0};
  int myCols{0};
};

#endif  // FDM_WORLD_LIB_MMATRIX_HPP