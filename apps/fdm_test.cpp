#include <iostream>

#include "dl_loader"      // IWYU pragma: keep
#include "fdm_world_lib"  // IWYU pragma: keep

int main() {
  std::cout << "working with dynamic loading...\n";

  // The following code does not work because the dynamic library does not have
  // any element exported

  // double expiry = 1.0, strike = 100.0, price = 10.0, forward = 105.0;
  // double vol = Bachelier::implied(expiry, strike, price, forward);
  // std::cout << "implied vol: " << vol << "\n";
  // std::cout << "Bachelier call: "
  //           << Bachelier::call(expiry, strike, forward, vol) << "\n";

  return 0;
}