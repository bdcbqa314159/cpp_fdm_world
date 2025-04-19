#include "solver.hpp"

bool Solver::newtonRaphson(SolverObjective& obj, double& x, int& numIter,
                           double& epsilon, string* error) {
  int i{};
  double y{}, dydx{};

  for (i = 0; i < numIter; ++i) {
    y = obj.value(x);
    dydx = obj.deriv(x);
    x -= y / dydx;

    if (fabs(y) <= epsilon) break;
  }

  numIter = min(i + 1, numIter);
  epsilon = obj.value(x);

  return true;
}