#pragma once
#ifndef FDM_WORLD_CONSTANTS_HPP
#define FDM_WORLD_CONSTANTS_HPP

class Constants {
 public:
  static double pi() { return 3.1415'9265'3589'79; }
  static double oneOverSqrt2Pi() { return 0.3989'4228'0401'433; }
  static double epsilon() { return 1.0e-10; }
  static double dblPrecision() { return 1.0e-15; }
};

#endif  // FDM_WORLD_CONSTANTS_HPP