#pragma once
#ifndef FDM_WORLD_INLINES_HPP
#define FDM_WORLD_INLINES_HPP

class Inlines {
 public:
  template <typename T>
  static T bound(T a, T x, T b);
  template <typename T>
  static void swap(T& a, T& b);
  template <class V>
  static int sign(V x);
  template <class V>
  static V sqr(V x);
};

template <typename T>
T Inlines::bound(T a, T x, T b) {
  return max(a, min(x, b));
}

template <typename T>
void Inlines::swap(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
  return;
}

template <class V>
int Inlines::sign(V x) {
  return x < 0.0 ? -1 : (x > 0.0 ? 1 : 0);
}

template <class V>
V Inlines::sqr(V x) {
  return x * x;
}

#endif  // FDM_WORLD_INLINES_HPP
