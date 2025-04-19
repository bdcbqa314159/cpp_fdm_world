#pragma once
#ifndef FDM_WORLD_LIB_MVECTOR_HPP
#define FDM_WORLD_LIB_MVECTOR_HPP

#include <algorithm>
#include <set>
#include <span>
#include <stdexcept>
#include <vector>

using std::conditional;
using std::enable_if_t;
using std::max;
using std::min;
using std::set;
// using std::span;
using std::vector;

template <typename T>
class mMatrixView;
template <typename T>
class mVectorView;

template <typename T = double>
class mVector {
 public:
  using Container = vector<T>;

  //	declarations
  using value_type = T;

  //	trivi c'tors
  mVector() = default;
  explicit mVector(size_t size) : myData(size) {}
  mVector(size_t size, T t0) : myData(size, t0) {}
  mVector(const mVector& rhs) = default;
  mVector(mVector&& rhs) noexcept = default;
  ~mVector() noexcept = default;

  //	trivi assign
  mVector& operator=(const mVector& rhs) = default;
  mVector& operator=(mVector&&) noexcept = default;

  //	assign from single value
  mVector& operator=(const T& t) {
    const int s = (int)myData.size();
    for (int i = 0; i < s; ++i) myData[i] = t;
    return *this;
  }

  //	element access
  const T& operator[](int i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size()) {
      throw std::runtime_error("mVector subscript out of range");
    }
#endif
    return myData[i];
  }

  T& operator[](int i) {
#ifdef _DEBUG
    if (i < 0 || i >= size()) {
      throw std::runtime_error("mVector subscript out of range");
    }
#endif
    return myData[i];
  }

  const T& operator()(int i) const { return operator[](i); }
  T& operator()(int i) { return operator[](i); }

  //	funcs
  int size() const { return (int)myData.size(); }
  bool empty() const { return myData.empty(); }
  void reserve(size_t s) { myData.reserve(s); }
  void resize(size_t s) { myData.resize(s); }
  void resize(size_t s, const T& t0) { myData.resize(s, t0); }
  void assign(size_t s, const T& t0) { myData.assign(s, t0); }
  void clear() { myData.clear(); }
  void push_back(const T& val) { myData.push_back(val); }
  void push_back(T&& val) { myData.push_back(val); }

  const Container& data() const { return myData; }
  Container& data() { return myData; }

  //	to view
  explicit operator mVectorView<T>() { return mVectorView<T>(*this); }
  explicit operator const mVectorView<T>() const {
    return mVectorView<T>(*this);
  }

  const mVectorView<T> operator()() const { return mVectorView<T>(*this); }
  mVectorView<T> operator()() { return mVectorView<T>(*this); }
  const mVectorView<T> operator()(int i, int size) const {
    return mVectorView<T>(&(*this)(i), size);
  }
  mVectorView<T> operator()(int i, int size) {
    return mVectorView<T>(&(*this)(i), size);
  }

  const mMatrixView<T> asRowMatrix() const {
    return mMatrixView<T>(const_cast<mVector<T>*>(this)->myData, 1, size());
  }
  const mMatrixView<T> asColMatrix() const {
    return mMatrixView<T>(const_cast<mVector<T>*>(this)->myData, size(), 1);
  }
  mMatrixView<T> asRowMatrix() { return mMatrixView<T>(data(), 1, size()); }
  mMatrixView<T> asColMatrix() { return mMatrixView<T>(data(), size(), 1); }

 private:
  Container myData;
};

template <typename T>
class mMatrixView;

template <typename T>
class mVectorView {
 public:
  //	declarations
  using view = span<T>;
  using value_type = T;

  //	trivi c'tors
  mVectorView() noexcept = default;
  mVectorView(const mVectorView&) noexcept = default;
  mVectorView(mVectorView&&) noexcept = default;
  ~mVectorView() noexcept = default;

  //	c'tors will make a view on the rhs (i.e. updating values will update the
  // rhs)
  mVectorView(mVector<T>& rhs) : myView(rhs.data()) {}
  mVectorView(const mVector<T>& rhs)
      : myView(const_cast<mVector<T>&>(rhs).data()) {}
  mVectorView(vector<T>& rhs) : myView(rhs) {}
  mVectorView(const vector<T>& rhs) : myView(const_cast<vector<T>&>(rhs)) {}
  mVectorView(T& rhs) : myView(&rhs, 1) {}
  mVectorView(const T& rhs) : myView(&const_cast<T&>(rhs), 1) {}
  explicit mVectorView(view& rhs) : myView(rhs) {}
  mVectorView(T* t, size_t count) : myView(t, count) {}
  mVectorView(const T* t, size_t count) : myView(const_cast<T*>(t), count) {}

  //	trivi assign
  mVectorView& operator=(const mVectorView&) noexcept = default;
  mVectorView& operator=(mVectorView&&) noexcept = default;

  //	assign from single value
  mVectorView& operator=(const T& t) {
    const int s = (int)myView.size();
    for (int i = 0; i < s; ++i) myView[i] = t;
    return *this;
  }

  //	to matrix view
  const mMatrixView<T> asRowMatrix() const {
    return mMatrixView<T>(const_cast<mVectorView<T>*>(this)->myView, 1, size());
  }
  const mMatrixView<T> asColMatrix() const {
    return mMatrixView<T>(const_cast<mVectorView<T>*>(this)->myView, size(), 1);
  }
  mMatrixView<T> asRowMatrix() { return mMatrixView<T>(data(), 1, size()); }
  mMatrixView<T> asColMatrix() { return mMatrixView<T>(data(), size(), 1); }

  //	to view
  const mVectorView<T> operator()(int i, int size) const {
    return mVectorView<T>(&(*this)(i), size);
  }
  mVectorView<T> operator()(int i, int size) {
    return mVectorView<T>(&(*this)(i), size);
  }

  //	element access
  const T& operator[](int i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mVectorView subscript out of range");
#endif
    return myView[i];
  }

  T& operator[](int i) {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("mVectorView subscript out of range");
#endif
    return myView[i];
  }

  const T& operator()(int i) const { return operator[](i); }
  T& operator()(int i) { return operator[](i); }

  //	funcs
  int size() const { return (int)myView.size(); }
  bool empty() const { return size() == 0; }

  const view& data() const { return myView; }
  view& data() { return myView; }

 private:
  view myView;
};

#endif  // FDM_WORLD_LIB_MVECTOR_HPP