#pragma once
#ifndef FDM_WORLD_INLINES_HPP
#define FDM_WORLD_INLINES_HPP

class Inlines {
 public:
  template <typename T>
  static T bound(T a, T x, T b);
  template <typename T>
  static void swap(T& a, T& b);
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

#endif  // FDM_WORLD_INLINES_HPP
