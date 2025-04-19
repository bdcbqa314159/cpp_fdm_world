#pragma once
#ifndef FDM_WORLD_INLINES_HPP
#define FDM_WORLD_INLINES_HPP

class Inlines {
 public:
  template <typename T>
  static T bound(T a, T x, T b);
};

template <typename T>
T Inlines::bound(T a, T x, T b) {
  return max(a, min(x, b));
}

#endif  // FDM_WORLD_INLINES_HPP
