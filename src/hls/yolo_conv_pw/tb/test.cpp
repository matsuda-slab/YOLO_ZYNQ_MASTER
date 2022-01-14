#include <iostream>
#include "include/conv_tb.h"
#include "ap_fixed.h"

int main() {
  std::cout << tb_input[0] << std::endl;
  std::cout << 256 * tb_input[0] << std::endl;
  std::cout << short(256 * tb_input[0]) << std::endl;
  std::cout << (ap_fixed<16, 8>)(short(256 * tb_input[0])) << std::endl;

  short a = 70;
  ap_fixed<16, 8> a_fixed = a;
  std::cout << (ap_fixed<16, 8>)(a_fixed/256) << std::endl;
  std::cout << a_fixed << std::endl;

  return 0;
}
