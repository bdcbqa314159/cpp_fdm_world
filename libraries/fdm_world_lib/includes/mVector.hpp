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
class MatrixView;
template <typename T>
class VectorView;

template <typename T = double>
class Vector {
 public:
  using Container = vector<T>;

  //	declarations
  using value_type = T;

  Vector() = default;
  explicit Vector(size_t size) : myData(size) {}
  Vector(size_t size, T t0) : myData(size, t0) {}
  Vector(const Vector& rhs) = default;
  Vector(Vector&& rhs) noexcept = default;
  ~Vector() noexcept = default;

  Vector& operator=(const Vector& rhs) = default;
  Vector& operator=(Vector&&) noexcept = default;

  Vector& operator=(const T& t) {
    for (size_t i = 0; i < myData.size(); ++i) myData[i] = t;
    return *this;
  }

  //	element access
  const T& operator[](size_t i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size()) {
      throw std::runtime_error("Vector subscript out of range");
    }
#endif
    return myData[i];
  }

  T& operator[](size_t i) {
#ifdef _DEBUG
    if (i < 0 || i >= size()) {
      throw std::runtime_error("Vector subscript out of range");
    }
#endif
    return myData[i];
  }

  const T& operator()(size_t i) const { return operator[](i); }
  T& operator()(size_t i) { return operator[](i); }

  //	funcs
  size_t size() const { return myData.size(); }
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
  explicit operator VectorView<T>() { return VectorView<T>(*this); }
  explicit operator const VectorView<T>() const { return VectorView<T>(*this); }

  const VectorView<T> operator()() const { return VectorView<T>(*this); }
  VectorView<T> operator()() { return VectorView<T>(*this); }
  const VectorView<T> operator()(size_t i, size_t size) const {
    return VectorView<T>(&(*this)(i), size);
  }
  VectorView<T> operator()(int i, int size) {
    return VectorView<T>(&(*this)(i), size);
  }

  const MatrixView<T> asRowMatrix() const {
    return MatrixView<T>(const_cast<Vector<T>*>(this)->myData, 1, size());
  }
  const MatrixView<T> asColMatrix() const {
    return MatrixView<T>(const_cast<Vector<T>*>(this)->myData, size(), 1);
  }
  MatrixView<T> asRowMatrix() { return MatrixView<T>(data(), 1, size()); }
  MatrixView<T> asColMatrix() { return MatrixView<T>(data(), size(), 1); }

 private:
  Container myData;
};

template <typename T>
class MatrixView;

template <typename T>
class VectorView {
 public:
  //	declarations
  using view = span<T>;
  using value_type = T;

  //	trivi c'tors
  VectorView() noexcept = default;
  VectorView(const VectorView&) noexcept = default;
  VectorView(VectorView&&) noexcept = default;
  ~VectorView() noexcept = default;

  //	c'tors will make a view on the rhs (i.e. updating values will update the
  // rhs)
  VectorView(Vector<T>& rhs) : myView(rhs.data()) {}
  VectorView(const Vector<T>& rhs)
      : myView(const_cast<Vector<T>&>(rhs).data()) {}
  VectorView(vector<T>& rhs) : myView(rhs) {}
  VectorView(const vector<T>& rhs) : myView(const_cast<vector<T>&>(rhs)) {}
  VectorView(T& rhs) : myView(&rhs, 1) {}
  VectorView(const T& rhs) : myView(&const_cast<T&>(rhs), 1) {}
  explicit VectorView(view& rhs) : myView(rhs) {}
  VectorView(T* t, size_t count) : myView(t, count) {}
  VectorView(const T* t, size_t count) : myView(const_cast<T*>(t), count) {}

  //	trivi assign
  VectorView& operator=(const VectorView&) noexcept = default;
  VectorView& operator=(VectorView&&) noexcept = default;

  //	assign from single value
  VectorView& operator=(const T& t) {
    for (size_t i = 0; i < myView.size(); ++i) myView[i] = t;
    return *this;
  }

  //	to matrix view
  const MatrixView<T> asRowMatrix() const {
    return MatrixView<T>(const_cast<VectorView<T>*>(this)->myView, 1, size());
  }
  const MatrixView<T> asColMatrix() const {
    return MatrixView<T>(const_cast<VectorView<T>*>(this)->myView, size(), 1);
  }
  MatrixView<T> asRowMatrix() { return MatrixView<T>(data(), 1, size()); }
  MatrixView<T> asColMatrix() { return MatrixView<T>(data(), size(), 1); }

  //	to view
  const VectorView<T> operator()(size_t i, size_t size) const {
    return VectorView<T>(&(*this)(i), size);
  }
  VectorView<T> operator()(size_t i, size_t size) {
    return VectorView<T>(&(*this)(i), size);
  }

  //	element access
  const T& operator[](size_t i) const {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("VectorView subscript out of range");
#endif
    return myView[i];
  }

  T& operator[](size_t i) {
#ifdef _DEBUG
    if (i < 0 || i >= size())
      throw std::runtime_error("VectorView subscript out of range");
#endif
    return myView[i];
  }

  const T& operator()(size_t i) const { return operator[](i); }
  T& operator()(size_t i) { return operator[](i); }

  //	funcs
  size_t size() const { return myView.size(); }
  bool empty() const { return size() == 0; }

  const view& data() const { return myView; }
  view& data() { return myView; }

 private:
  view myView;
};

#endif  // FDM_WORLD_LIB_MVECTOR_HPP