#ifdef WIN32
#include <windows.h>

#include "code_cpp.hpp"
#include "excel_sdk_api"
#include "exported.hpp"
#include "fdm_world_lib"
#include "xlUtils.h"

//	Wrappers

EXPORT_C {
  EXPORTED double xMultiply2Numbers(double x, double y) { return x * y; }

  EXPORTED LPXLOPER12 xMatrixMul(LPXLOPER12 A_in, LPXLOPER12 B_in) {
    FreeAllTempMemory();

    mMatrix<double> A;
    if (!kXlUtils::getMatrix(A_in, A))
      return TempStr12("input 1 is not a matrix");

    mMatrix<double> B;
    if (!kXlUtils::getMatrix(B_in, B))
      return TempStr12("input 2 is not a vector");

    if (B.rows() != A.cols())
      return TempStr12(
          "input 2 must have number of rows same as input 1 number of cols");

    mMatrix<double> res;
    mMatrixAlgebra::mmult(A, B, res);

    LPXLOPER12 out = TempXLOPER12();
    kXlUtils::setMatrix(res, out);
    return out;
  }

  EXPORTED LPXLOPER12 xBachelierCall(LPXLOPER12 expiry_, LPXLOPER12 strike_,
                                     LPXLOPER12 forward_,
                                     LPXLOPER12 volatility_) {
    FreeAllTempMemory();

    //	helps
    string err;

    //	get expiry
    double expiry;
    if (!kXlUtils::getDbl(expiry_, 0, 0, expiry, &err))
      return kXlUtils::setError(err);

    //	get strike
    double strike;
    if (!kXlUtils::getDbl(strike_, 0, 0, strike, &err))
      return kXlUtils::setError(err);

    //	get forwad
    double forward;
    if (!kXlUtils::getDbl(forward_, 0, 0, forward, &err))
      return kXlUtils::setError(err);

    //	get volatility
    double volatility;
    if (!kXlUtils::getDbl(volatility_, 0, 0, volatility, &err))
      return kXlUtils::setError(err);

    //	calc
    double call = Bachelier::call(expiry, strike, forward, volatility);

    //	set output
    LPXLOPER12 out = kXlUtils::getOper(1, 1);
    kXlUtils::setDbl(0, 0, call, out);

    //	done
    return out;
  }

  EXPORTED LPXLOPER12 xBachelierImplied(LPXLOPER12 expiry_, LPXLOPER12 strike_,
                                        LPXLOPER12 price_,
                                        LPXLOPER12 forward_) {
    FreeAllTempMemory();

    //	helps
    string err;

    //	get expiry
    double expiry;
    if (!kXlUtils::getDbl(expiry_, 0, 0, expiry, &err))
      return kXlUtils::setError(err);

    //	get strike
    double strike;
    if (!kXlUtils::getDbl(strike_, 0, 0, strike, &err))
      return kXlUtils::setError(err);

    //	get volatility
    double price;
    if (!kXlUtils::getDbl(price_, 0, 0, price, &err))
      return kXlUtils::setError(err);

    //	get forward
    double forward;
    if (!kXlUtils::getDbl(forward_, 0, 0, forward, &err))
      return kXlUtils::setError(err);

    //	calc
    double volatility = Bachelier::implied(expiry, strike, price, forward);

    //	set output
    LPXLOPER12 out = kXlUtils::getOper(1, 1);
    kXlUtils::setDbl(0, 0, volatility, out);

    //	done
    return out;
  }

  EXPORTED LPXLOPER12 xBlackCall(LPXLOPER12 expiry_, LPXLOPER12 strike_,
                                 LPXLOPER12 forward_, LPXLOPER12 volatility_) {
    FreeAllTempMemory();

    //	helps
    string err;

    //	get expiry
    double expiry;
    if (!kXlUtils::getDbl(expiry_, 0, 0, expiry, &err))
      return kXlUtils::setError(err);

    //	get strike
    double strike;
    if (!kXlUtils::getDbl(strike_, 0, 0, strike, &err))
      return kXlUtils::setError(err);

    //	get forwad
    double forward;
    if (!kXlUtils::getDbl(forward_, 0, 0, forward, &err))
      return kXlUtils::setError(err);

    //	get volatility
    double volatility;
    if (!kXlUtils::getDbl(volatility_, 0, 0, volatility, &err))
      return kXlUtils::setError(err);

    //	calc
    double call = Black::call(expiry, strike, forward, volatility);

    //	set output
    LPXLOPER12 out = kXlUtils::getOper(1, 1);
    kXlUtils::setDbl(0, 0, call, out);

    //	done
    return out;
  }

  EXPORTED LPXLOPER12 xBlackImplied(LPXLOPER12 expiry_, LPXLOPER12 strike_,
                                    LPXLOPER12 price_, LPXLOPER12 forward_) {
    FreeAllTempMemory();

    //	helps
    string err;

    //	get expiry
    double expiry;
    if (!kXlUtils::getDbl(expiry_, 0, 0, expiry, &err))
      return kXlUtils::setError(err);

    //	get strike
    double strike;
    if (!kXlUtils::getDbl(strike_, 0, 0, strike, &err))
      return kXlUtils::setError(err);

    //	get volatility
    double price;
    if (!kXlUtils::getDbl(price_, 0, 0, price, &err))
      return kXlUtils::setError(err);

    //	get forward
    double forward;
    if (!kXlUtils::getDbl(forward_, 0, 0, forward, &err))
      return kXlUtils::setError(err);

    //	calc
    double volatility = Black::implied(expiry, strike, price, forward);

    //	set output
    LPXLOPER12 out = kXlUtils::getOper(1, 1);
    kXlUtils::setDbl(0, 0, volatility, out);

    //	done
    return out;
  }

  EXPORTED LPXLOPER12 xTridag(LPXLOPER12 A_in, LPXLOPER12 b_in) {
    FreeAllTempMemory();

    kMatrix<double> A;
    if (!kXlUtils::getMatrix(A_in, A))
      return TempStr12("input 1 is not a matrix");

    kVector<double> b;
    if (!kXlUtils::getVector(b_in, b))
      return TempStr12("input 2 is not a vector");

    if (b.size() != A.rows())
      return TempStr12("input 1 must have same number of rows as input 2");

    kVector<double> gam, res;
    kMatrixAlgebra::tridag(A, b, res, gam);

    LPXLOPER12 out = TempXLOPER12();
    kXlUtils::setVector(res, out);
    return out;
  }

  EXPORTED LPXLOPER12 xBanmul(LPXLOPER12 A_in, LPXLOPER12 x_in, double m1_in,
                              double m2_in) {
    FreeAllTempMemory();

    kMatrix<double> A;
    if (!kXlUtils::getMatrix(A_in, A))
      return TempStr12("input 1 is not a matrix");

    kVector<double> x;
    if (!kXlUtils::getVector(x_in, x))
      return TempStr12("input 2 is not a vector");

    if (x.size() != A.rows())
      return TempStr12("input 1 must have same number of rows as input 2");

    kVector<double> res;
    int m1 = (int)(m1_in + 0.5), m2 = (int)(m2_in + 0.5);
    kMatrixAlgebra::banmul(A, m1, m2, x, res);

    LPXLOPER12 out = TempXLOPER12();
    kXlUtils::setVector(res, out);
    return out;
  }

  //	Registers

  EXPORTED int xlAutoOpen(void) {
    XLOPER12 xDLL;

    Excel12f(xlGetName, &xDLL, 0);

    Excel12f(xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
             (LPXLOPER12)TempStr12(L"xMultiply2Numbers"),
             (LPXLOPER12)TempStr12(L"BBB"),
             (LPXLOPER12)TempStr12(L"xMultiply2Numbers"),
             (LPXLOPER12)TempStr12(L"x, y"), (LPXLOPER12)TempStr12(L"1"),
             (LPXLOPER12)TempStr12(L"myOwnCppFunctions"),
             (LPXLOPER12)TempStr12(L""), (LPXLOPER12)TempStr12(L""),
             (LPXLOPER12)TempStr12(L"Multiplies 2 numbers"),
             (LPXLOPER12)TempStr12(L""));

    Excel12f(xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
             (LPXLOPER12)TempStr12(L"xMatrixMul"),
             (LPXLOPER12)TempStr12(L"QQQ"),
             (LPXLOPER12)TempStr12(L"xMatrixMul"),
             (LPXLOPER12)TempStr12(L"A, B"), (LPXLOPER12)TempStr12(L"1"),
             (LPXLOPER12)TempStr12(L"myOwnCppFunctions"),
             (LPXLOPER12)TempStr12(L""), (LPXLOPER12)TempStr12(L""),
             (LPXLOPER12)TempStr12(L"Multiplying 2 matrices."),
             (LPXLOPER12)TempStr12(L""));

    Excel12f(xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
             (LPXLOPER12)TempStr12(L"xBachelierCall"),
             (LPXLOPER12)TempStr12(L"QQQQQ"),
             (LPXLOPER12)TempStr12(L"xBachelierCall"),
             (LPXLOPER12)TempStr12(L"expiry, strike, forward, volatility"),
             (LPXLOPER12)TempStr12(L"1"),
             (LPXLOPER12)TempStr12(L"myOwnCppFunctions"),
             (LPXLOPER12)TempStr12(L""), (LPXLOPER12)TempStr12(L""),
             (LPXLOPER12)TempStr12(L"Price option in Bachelier model"),
             (LPXLOPER12)TempStr12(L""));

    Excel12f(
        xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
        (LPXLOPER12)TempStr12(L"xBachelierImplied"),
        (LPXLOPER12)TempStr12(L"QQQQQ"),
        (LPXLOPER12)TempStr12(L"xBachelierImplied"),
        (LPXLOPER12)TempStr12(L"expiry, strike, price, forward"),
        (LPXLOPER12)TempStr12(L"1"),
        (LPXLOPER12)TempStr12(L"myOwnCppFunctions"), (LPXLOPER12)TempStr12(L""),
        (LPXLOPER12)TempStr12(L""),
        (LPXLOPER12)TempStr12(L"Compute implied volatility in Bachelier model"),
        (LPXLOPER12)TempStr12(L""));

    Excel12f(xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
             (LPXLOPER12)TempStr12(L"xBlackCall"),
             (LPXLOPER12)TempStr12(L"QQQQQ"),
             (LPXLOPER12)TempStr12(L"xBlackCall"),
             (LPXLOPER12)TempStr12(L"expiry, strike, forward, volatility"),
             (LPXLOPER12)TempStr12(L"1"),
             (LPXLOPER12)TempStr12(L"myOwnCppFunctions"),
             (LPXLOPER12)TempStr12(L""), (LPXLOPER12)TempStr12(L""),
             (LPXLOPER12)TempStr12(L"Price option in Black model"),
             (LPXLOPER12)TempStr12(L""));

    Excel12f(
        xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
        (LPXLOPER12)TempStr12(L"xBlackImplied"),
        (LPXLOPER12)TempStr12(L"QQQQQ"),
        (LPXLOPER12)TempStr12(L"xBlackImplied"),
        (LPXLOPER12)TempStr12(L"expiry, strike, price, forward"),
        (LPXLOPER12)TempStr12(L"1"),
        (LPXLOPER12)TempStr12(L"myOwnCppFunctions"), (LPXLOPER12)TempStr12(L""),
        (LPXLOPER12)TempStr12(L""),
        (LPXLOPER12)TempStr12(L"Compute implied volatility in Black model"),
        (LPXLOPER12)TempStr12(L""));

    Excel12f(xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
             (LPXLOPER12)TempStr12(L"xTridag"), (LPXLOPER12)TempStr12(L"QQQ"),
             (LPXLOPER12)TempStr12(L"xTridag"), (LPXLOPER12)TempStr12(L"A, b"),
             (LPXLOPER12)TempStr12(L"1"),
             (LPXLOPER12)TempStr12(L"myOwnCppFunctions"),
             (LPXLOPER12)TempStr12(L""), (LPXLOPER12)TempStr12(L""),
             (LPXLOPER12)TempStr12(L"Solving tri-diagonal system."),
             (LPXLOPER12)TempStr12(L""));

    Excel12f(xlfRegister, 0, 11, (LPXLOPER12)&xDLL,
             (LPXLOPER12)TempStr12(L"xBanmul"), (LPXLOPER12)TempStr12(L"QQQBB"),
             (LPXLOPER12)TempStr12(L"xBanmul"),
             (LPXLOPER12)TempStr12(L"A, x, m1, m2"),
             (LPXLOPER12)TempStr12(L"1"),
             (LPXLOPER12)TempStr12(L"myOwnCppFunctions"),
             (LPXLOPER12)TempStr12(L""), (LPXLOPER12)TempStr12(L""),
             (LPXLOPER12)TempStr12(L"Multiplying band-matrix with vector."),
             (LPXLOPER12)TempStr12(L""));

    /* Free the XLL filename */
    Excel12f(xlFree, 0, 1, (LPXLOPER12)&xDLL);

    return 1;
  }
}

#endif  // WIN32