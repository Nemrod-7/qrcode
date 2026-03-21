#include <iostream>

namespace QR {
  namespace info {
      const std::string ecc[4] = {"Medium", "Low", "Quartile", "High"};
      const std::string mode[4] = {"NUMERIC", "ALPHANUM", "BYTE", "KANJI"};
  };
};

int main () {

    std::cout << QR::info::ecc[0];
}
