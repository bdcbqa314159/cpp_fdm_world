#pragma once
#ifndef FDM_WORLD_LIB_MMATRIX_HPP
#define FDM_WORLD_LIB_MMATRIX_HPP

#include "mVector.hpp"

template <typename T>
class mMatrixView;

template <typename T = double>
class mMatrix {
 public:
  using Container = vector<T>;
  using value_type = T;

  mMatrix() = default;
  mMatrix(size_t rows, size_t cols)
      : myData(rows * cols), myRows((int)rows), myCols((int)cols) {}
  mMatrix(size_t rows, size_t cols, T t0)
      : myData(rows * cols, t0), myRows((int)rows), myCols((int)cols) {}
  mMatrix(const mMatrix& rhs) = default;
  mMatrix(mMatrix&& rhs) noexcept = default;
  ~mMatrix() noexcept = default;

  mMatrix& operator=(const mMatrix& rhs) = default;
  mMatrix& operator=(mMatrix&& rhs) = default;

  mMatrix& operator=(const T& t) {
    const size_t s = myData.size();
    for (size_t i = 0; i < s; ++i) myData[i] = t;
    return *this;
  }

  size_t rows() const { return myRows; }
  size_t cols() const { return myCols; }
  size_t size() const { return myData.size(); }
  bool empty() const { return size() == 0; }

  size_t rcToIdx(size_t i, size_t j) const { return i * myCols + j; }
  size_t rToIdx(size_t i) const { return i * myCols; }

  const T& operator()(size_t i, size_t j) const {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrix row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrix col subscript out of range");
#endif
    return myData[rcToIdx(i, j)];
  }
  T& operator()(size_t i, size_t j) {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrix row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrix col subscript out of range");
#endif
    return myData[rcToIdx(i, j)];
  }

  const T& operator[](size_t i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrix subscript out of range");
#endif
    return myData[i];
  }

  T& operator[](size_t i) {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrix subscript out of range");
#endif
    return myData[i];
  }

  void resize(size_t rows, size_t cols) { resize(rows, cols, T{}); }
  void resize(size_t rows, size_t cols, const T& t0) {
    if (cols == myCols) {
      myData.resize(rows * cols, t0);
      myRows = rows;
      return;
    }

    Container tmp;
    swap(tmp, myData);
    myData.resize(rows * cols, t0);

    size_t minR = min<size_t>(rows, myRows);
    size_t minC = min<size_t>(cols, myCols);
    for (size_t i = 0; i < minR; ++i) {
      for (size_t j = 0; j < minC; ++j) {
        myData[i * cols + j] = tmp[i * myCols + j];
      }
    }

    myRows = rows;
    myCols = cols;
  }
  void clear() { resize(0, 0); }

  const Container& data() const { return myData; }
  Container& data() { return myData; }

  explicit operator const kVectorView<T>() const {
    return kVectorView<T>(myData);
  }
  explicit operator kVectorView<T>() { return kVectorView<T>(myData); }
  const kVectorView<T> asVector() const { return kVectorView<T>(myData); }
  kVectorView<T> asVector() { return kVectorView<T>(myData); }

  const mMatrixView<T> operator()() const { return mMatrixView<T>(*this); }
  mMatrixView<T> operator()() { return mMatrixView<T>(*this); }
  const kVectorView<T> operator()(int i) const {
    T* t = &(const_cast<mMatrix*>(this)->myData[rToIdx(i)]);
    return kVectorView<T>(t, myCols);
  }
  kVectorView<T> operator()(size_t i) {
    return kVectorView<T>(&myData[rToIdx(i)], myCols);
  }

 private:
  Container myData;
  size_t myRows{0};
  size_t myCols{0};
};

template <typename T>
class mMatrixView {
 public:
  using view = span<T>;
  using value_type = T;

  mMatrixView() noexcept = default;
  mMatrixView(const mMatrixView&) noexcept = default;
  mMatrixView(mMatrixView&&) noexcept = default;
  ~mMatrixView() noexcept = default;

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

  mMatrixView& operator=(const mMatrixView&) noexcept = default;
  mMatrixView& operator=(mMatrixView&&) noexcept = default;

  mMatrixView& operator=(const T& t) {
    const size_t s = myView.size();
    for (size_t i = 0; i < s; ++i) myView[i] = t;
    return *this;
  }

  size_t rows() const { return myRows; }
  size_t cols() const { return myCols; }
  size_t size() const { return myView.size(); }
  bool empty() const { return size() == 0; }

  size_t rcToIdx(size_t i, size_t j) const { return i * myCols + j; }
  size_t rToIdx(size_t i) const { return i * myCols; }

  const T& operator()(size_t i, size_t j) const {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrixView row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrixView col subscript out of range");
#endif
    return myView[rcToIdx(i, j)];
  }
  T& operator()(size_t i, size_t j) {
#ifdef _DEBUG
    if (i < 0 || i >= myRows)
      throw std::runtime_error("mMatrixView row subscript out of range");
    if (j < 0 || j >= myCols)
      throw std::runtime_error("mMatrixView col subscript out of range");
#endif
    return myView[rcToIdx(i, j)];
  }

  const T& operator[](size_t i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrixView subscript out of range");
#endif
    return myView[i];
  }

  T& operator[](size_t i) {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mMatrixView subscript out of range");
#endif
    return myView[i];
  }

  explicit operator const kVectorView<T>() const {
    return kVectorView<T>(myView);
  }
  explicit operator kVectorView<T>() { return kVectorView<T>(myView); }
  const kVectorView<T> asVector() const { return kVectorView<T>(myView); }
  kVectorView<T> asVector() { return kVectorView<T>(myView); }

  const kVectorView<T> operator()(size_t i) const {
    T* t = &(const_cast<mMatrixView*>(this)->myView[rToIdx(i)]);
    return kVectorView<T>(t, myCols);
  }
  kVectorView<T> operator()(size_t i) {
    return kVectorView<T>(&myView[rToIdx(i)], myCols);
  }

  const view& data() const { return myView; }
  view& data() { return myView; }

 private:
  view myView;
  size_t myRows{0};
  size_t myCols{0};
};

#endif  // FDM_WORLD_LIB_MMATRIX_HPP