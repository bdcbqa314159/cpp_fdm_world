#include "Bachelier.hpp"

#include "solver.hpp"

class BachelierObj : public SolverObjective {
 public:
  //	constructor
  BachelierObj(double expiry, double strike, double price, double forward)
      : SolverObjective(),
        myExpiry(expiry),
        myStrike(strike),
        myPrice(price),
        myForward(forward) {}

  //	value
  virtual double value(double x) {
    double res = Bachelier::call(myExpiry, myStrike, myForward, x) - myPrice;

    //	done
    return res;
  }

  //	deriv
  virtual double deriv(double x) {
    double res = Bachelier::vega(myExpiry, myStrike, myForward, x);

    //	done
    return res;
  }

  //	private parts
 private:
  //	expiry
  double myExpiry;
  double myStrike;
  double myPrice;
  double myForward;
};

//	implied
double Bachelier::implied(double expiry, double strike, double price,
                          double forward) {
  //	calc intrinsic
  double intrinc = max(0.0, forward - strike);
  if (price <= intrinc) return 0.0;

  //	objective
  BachelierObj obj(expiry, strike, price, forward);

  //	start guess
  double volatility = 0.1;
  int numIter = 10;
  double epsilon = (price - intrinc) * Constants::epsilon();

  //	solve
  Solver::newtonRaphson(obj, volatility, numIter, epsilon, nullptr);

  //	bound
  volatility = max(0.0, volatility);

  //	done
  return volatility;
}
