#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

#include "grid.hpp"


static std::string grid (const std::vector<std::vector<int>> &grid) {
    std::string os;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            os += (grid[i][j] == 0) ? ". " : "# ";
        }
        os += '\n';
    }
    return os;
}

int bin2int(const std::string &src) { return stoi(src, nullptr, 2); }

int gen_format_info(int ecc, int mask) {
    // generator polynomial x^10 + x^8 + x^5 + x^4 + x^2 + x + 1 (binary form : 10100110111 integer : 1335)

    // the qr format info is in the form [ info ][error correction]
    //                                   [5 bits][     10 bits    ]
    // gen : 010100110111000 ( polynomial padded to match the information size (ie : 15 bits))
    // num : 011000000000000 (ex for ecc 1 and mask 4 -> [01100][00000 00000])
    const int num = ((ecc << 3) | mask) << 10;
    int gen = 1335 << 3; // "10100110111000";
    int res = num;

    // performs 4 division
    for (int i = 0; i < 4; i++) {
        res ^= (gen >> i);
    }
    // mask it (xor) with 101010000010010 (ie : 21522 in integer form (qr specification))
    return (num | res) ^ 21522;
}

void golay_code() {
  int version = 7;
  int nbits = 6;
      // golay error code
      // the bch 18,6
      //                 [6 bits ][ 12 bits             ]
      // version string :[version][error correction code]
      // generator polynomial : x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
      // generator polynomial : 1111100100101 in binary form (integer 7973)
      const int num = version << 12;
      int gen = 7973 << 0;
      int res = num;

      // cout << bitset<18>(num) << "\n";
      // cout << bitset<18>(gen) << "\n";

      // for (int i = 0; i < 6; i++) {
      //     res ^= (gen >> i);
      //     cout << bitset<18>(res) << "\n";
      // }
      //
}

std::vector<std::vector<int>> micro_qr (int version) { // M1, M2, M3, M4
    const int size = 2 * version + 11;
    std::vector<std::vector<int>> grid(size, std::vector<int>(size, 2));
    finder(grid, 3, 3);

    for (int i = 0; i < 8; i++) {
        grid[7][i] = grid[i][7] = 0; // quiet zone
        grid[8][i + 1] = grid[i + 1][8] = 0; // format information zone
    }

    for (int i = 8; i < size; i++) { // timing patttern
        grid[0][i] = grid[i][0] = i % 2 == 0;
    }

    return grid;
}

int main () {
    cout << "\n\n\n";

    // L=1, M=0, Q=3, H=2

    int size = 11;

    for (int i = 0; i < 4; i++) {
        cout << grid(micro_qr(i)) << "\n";
    }

    //
    // for (int i = 0; i < size; i++) {
    //   for (int j = 0; j < size; j++) {
    //       if (grid[i][j] == 0) {
    //           // cout << "  ";
    //       } else {
    //       }
    //       cout << grid[i][j] << " ";
    //   }
    //   cout << '\n';
    // }

    cout << "\nend\n";
}
