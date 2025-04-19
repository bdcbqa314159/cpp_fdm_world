#pragma once
#ifndef FDM_WORLD_LIB_SOLVER_HPP
#define FDM_WORLD_LIB_SOLVER_HPP

#include <string>

#include "mMatrix.hpp"
#include "mVector.hpp"

using std::string;

class SolverObjective {
 public:
  virtual double value(double x) { return 0.0; }

  virtual double deriv(double x) { return 0.0; }
};

//	solver
class Solver {
 public:
  static bool newtonRaphson(SolverObjective& obj, double& x, int& numIter,
                            double& epsilon, string* error);
};

#endif  // FDM_WORLD_LIB_SOLVER_HPP