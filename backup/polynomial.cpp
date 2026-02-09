#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

using i8 = short int;
using polynomial = std::vector<i8>;

using namespace std;

namespace show {
    std::string simplified (const polynomial &rem) {
        std::string os;
        for (int i = 0; i < rem.size(); i++) {
            std::string num = std::to_string(rem[i]);
            os += std::string(3 - num.size(), ' ') + num + " ";
        }
        return os;
    }
    std::string poly (const polynomial &p) {
        string os;

        for (int i = 0; i < p.size(); i++) {
            int val = p[i], ex = p.size() - i - 1;

            if (val != 0) {
                if (os.size() || val < 0) os += val > 0 ? " + " : " - ";
                os += std::to_string(abs(val));
                if (ex > 0) os += "x";
                if (ex > 1) os += "^" + std::to_string(ex);
            }
        }
        return os;
    }
};

i8 add (i8 a, i8 b) { return a ^ b; }
i8 sub (i8 a, i8 b) { return a ^ b; }

polynomial add (const polynomial &a, const polynomial &b) {
    const int size = max(a.size(), b.size());
    const int ov = size - b.size();
    polynomial res (size);

    for (int i = 0; i < a.size(); i++) {
        res[i + size - a.size()] = a[i];
    }

    for (int i = 0; i < b.size(); i++) {
        res[i + ov] = add(res[i + ov], b[i]);
    }

    return res;
}
polynomial PolyScale(const polynomial &p, u8 argX) {
  polynomial res (p.size());

  for (int i = 0; i < p.size(); i++) {
      res[i] = gf256::mul(p[i], argX);
  }

  return res;
}
int PolyEval(const polynomial &p, u8 argX) {
  int tmp, val = p[0];

  for (int i = 1; i < p.size(); i++) {
      // tmp = gf256::mul(val, argX);
      // tmp = tempValu ^ p[i];
      // val = tempValu;
      val = gf256::mul(val, argX) ^ p[i];
  }

  return val;
}

int main() {

    for (int i = 0; i < 3; i++) cout << "\n";

    polynomial a = {8,2,9,-3,-7};
    polynomial b = {5,6,0,-4};

    // for (int i = 0; i < 7; i++) {
    //       edc.push_back(0);
    // }
    std::cout << show::poly(a) << "\n";
    std::cout << show::poly(b) << "\n";
    std::cout << "---------------\n";
    const int size = max(a.size(), b.size());
    polynomial res;


    // std::cout << show::poly(res) << "\n";
    // std::cout << rs.show();
}
