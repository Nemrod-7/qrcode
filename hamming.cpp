#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

int add (bool a, bool b) { return a ^ b; }
int mul (bool a, bool b) { return a & b; }

std::string tri_encode(const std::string &txt) {
    std::string os;

    for (auto &ch : txt) {
        int mask = 1 << 7;
        do {
            bool bit = mask & ch;
            os += std::string(3, bit + '0');
        } while(mask >>= 1);
    }
    return os;
}
std::string tri_decode(const std::string &bits) {
    std::string os;
    int byte = 0;

    for (size_t i = 0; i < bits.size(); i += 24) {
        int byte = 0;

        for (size_t j = 0, ix = 0; j < 8; j++, ix = i + (j * 3)) {
            int bit = (bits[ix] + bits[ix + 1] + bits[ix + 2] - 3 * '0') > 1;
            byte |= bit << (7 - j);
        }
    }

    return os;
}

std::string correct(int m, int n, std::string bits) { // correct a M x N byte
  bool col[n] = {0}, row[m] = {0};
  int x = -1, y = -1;
  std::string parity = bits.substr(m * n);

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      int bit = bits[n * i + j] - '0';
      row[i] ^= bit;
      col[j] ^= bit;
    }
  }

  for (int i = 0; i < m; i++) {
    if (row[i] != parity[i] - '0') {
        y = i;
    }
  }

    for (int i = 0; i < n; i++) {
        if (col[i] != parity[m + i] - '0') {
          x = i;
        }
    }

    if (x != -1 && y != -1) {
        bits[y * n + x] = bits[y * n + x] == '0' ? '1' : '0';
    } else if (y != -1) {
        bits[m * n + y] = bits[m * n + y] == '0' ? '1' : '0';
    } else if (x != -1) {
        bits[m * n + m + x] = bits[m * n + m + x] == '0' ? '1' : '0';
    }
    return bits;
}

int hamming_encode1(int num) {
    const bool kernel[4][7] = {
        {1,1,1,0,0,0,1}, {1,1,0,0,0,1,0}, {1,0,1,0,1,0,0}, {0,1,1,1,0,0,0},
    };
    int code = 0;

    for (int j = 0; j < 7; j++) {
      // bool par = 0;
      for (int i = 0; i < 4; i++) {
            // bool bit = ((num >> i) & 1);
            // par ^= bit & kernel[i][j];
            code ^= (((num >> i) & 1) & kernel[i][j]) << (6 - j);
            // printf("(%i x %i) ", bit, kernel[i][j]);
        }
        // printf(" = %i\n", par);
    }

    return code;
}
int hamming_encode2(int data) {
    const bool d1 = (data >> 3 & 1);
    const bool d2 = (data >> 2 & 1);
    const bool d3 = (data >> 1 & 1);
    const bool d4 = (data >> 0 & 1);
    int par = 0;

    par |= (d2 ^ d3 ^ d4) << 6;
    par |= (d1 ^ d3 ^ d4) << 5;
    par |= (d1 ^ d2 ^ d4) << 4;

    return par | data;
}

int hamming_decode(int byte) {

  bool chk[3][7] = {
    {1, 0, 1, 1,  1, 0, 0},
    {1, 1, 0, 1,  0, 1, 0},
    {1, 1, 1, 0,  0, 0, 1}
  };

  for (int k = 0; k < 7; k++) {
    const int data = byte ^ (1 << k); // just modify 1 bit
    int synd = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 7; j++) {
            bool bit = ((data >> j) & 1);
            synd ^= (bit & chk[i][j]) << i;
            // printf("(%i %i) ", chk[i][j], bit);
        }
        // printf("\n");
    }
    if (synd == 0) return data;
    // printf(" : %i", pary);
    // printf("\n");
  }
  return 0;
}

int main() {
  /*
  hamming code (7 4)
  |d1 d2 d3 d4|
  |1 0 1 0|
      .
  |p1 p2 p3 d1 d2 d3 d4|
  |0 1 1 1 0 0 0|
  |1 0 1 0 1 0 0|
  |1 1 0 0 0 1 0|
  |1 1 1 0 0 0 1|
      =
  |1 0 1 1 0 1 0|
   | | | | | | +-->(1 × 0) + (0 × 0) + (1 × 0) + (0 × 1)
   | | | | | +---->(1 × 0) + (0 × 0) + (1 × 1) + (0 × 0)
   | | | | +------>(1 × 0) + (0 × 1) + (1 × 0) + (0 × 0)
   | | | +-------->(1 × 1) + (0 × 0) + (1 × 0) + (0 × 0)
   | | +---------->(1 × 1) + (0 × 1) + (1 × 0) + (0 × 1)
   | +------------>(1 × 1) + (0 × 0) + (1 × 1) + (0 × 1)
   +-------------->(1 × 0) + (0 × 1) + (1 × 1) + (0 × 1)
  */

  cout << "\n\n\n";

  int byte = bitset<7>("1011011").to_ulong();

  hamming_decode(byte);

  const bool d1 = (byte >> 3 & 1);
  const bool d2 = (byte >> 2 & 1);
  const bool d3 = (byte >> 1 & 1);
  const bool d4 = (byte >> 0 & 1);

  bool p1 = d2 ^ d3 ^ d4;
  bool p2 = d1 ^ d3 ^ d4;
  bool p3 = d1 ^ d2 ^ d4;

  // printf("%i %i %i : %i\n", d2, d3, d4, p1);
  // printf("%i %i %i : %i\n", d1, d3, d4, p2);
  // printf("%i %i %i : %i\n", d1, d2, d4, p3);
  // printf("---------\n");

  // cout << bitset<7>(code);
  // for (int i = 6; i > 3; i--) {
  //   printf("%i %i\n", (byte >>i & 1), p[6 - i]);
  // }
  int minv = 18, best = 0;

  for (int data = 0; data < 16; data++) {
      int code = hamming_encode2(data);
      int diff = code ^ byte;
      int cnt = 0;

      do { cnt += diff & 1; } while (diff >>= 1);

      if (cnt < minv) {
          minv = cnt;
          best = code;
      }

  }

  // printf("%i %i", hamming_decode(byte), best);
  // printf("%i \n", best);
  // for (int i = 0; i < 9; i++) {
  //     printf("%i", (byte >> i) & 1);
  // }


  // string msg = "1010011001000111011000011000110001";
  // correct(4,6, msg);
  // correct(2,3, "11011010001");


}
