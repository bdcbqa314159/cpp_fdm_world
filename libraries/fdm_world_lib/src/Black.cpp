#include "Black.hpp"
#include "solver.hpp"

class BlackObj : public SolverObjective {
 public:
  //	constructor
  BlackObj(double expiry, double strike, double price, double forward)
      : SolverObjective(),
        myExpiry(expiry),
        myStrike(strike),
        myPrice(price),
        myForward(forward) {}

  //	value
  virtual double value(double x) {
    double res = Black::call(myExpiry, myStrike, myForward, x) - myPrice;

    //	done
    return res;
  }

  //	deriv
  virtual double deriv(double x) {
    double res = Black::vega(myExpiry, myStrike, myForward, x);

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
double Black::implied(double expiry, double strike, double price,
                          double forward) {
  //	calc intrinsic
  double intrinc = max(0.0, forward - strike);
  if (price <= intrinc) return 0.0;

  //	objective
  BlackObj obj(expiry, strike, price, forward);

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


//	fd runner
bool Black::fdRunner(const double s0, const double r, const double mu,
                      const double sigma, const double expiry,
                      const double strike, const bool dig,
                      const int pc,      //	put (-1) call (1)
                      const int ea,      //	european (0), american (1)
                      const int smooth,  //	smoothing
                      const double theta, const int wind, const double numStd,
                      const int numT, const int numS, const bool update,
                      const int numPr, double& res0, mVector<double>& s,
                      mVector<double>& res, string& error) {
  //	done
  return true;
}