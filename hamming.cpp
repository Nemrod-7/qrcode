#include <iostream>
#include <string>
#include <vector>

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

namespace hamming {
    /*
       hamming code (7 4) : total of 7 bits for 4 bits of data

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


       returns : p1 p2 d1 p3 d2 d3 d4
       */
    int encode (int data) {
      const bool d1 = ((data >> 3) & 1);
      const bool d2 = ((data >> 2) & 1);
      const bool d3 = ((data >> 1) & 1);
      const bool d4 = ((data >> 0) & 1);
      int block = d4 | (d3 << 1) | (d2 << 2) | (d1 << 4);

      block |= (d2 ^ d3 ^ d4) << 6;
      block |= (d1 ^ d3 ^ d4) << 5;
      block |= (d1 ^ d2 ^ d4) << 3;

      return block;
    }
    int decode (int data) {
        const bool d1 = ((data >> 4) & 1);
        const bool d2 = ((data >> 2) & 1);
        const bool d3 = ((data >> 1) & 1);
        const bool d4 = ((data >> 0) & 1);
        const int par = ((data >> 5) << 1) | ((data >> 3) & 1) ;
        const int syn = (d1 ^ d2 ^ d4) | ((d1 ^ d3 ^ d4) << 1) | ((d2 ^ d3 ^ d4) << 2);

        switch (par ^ syn)  {
            case 1 : data ^= 1 << 4; break;
            case 2 : data ^= 1 << 5; break;
            case 3 : data ^= 1 << 3; break;
            case 4 : data ^= 1 << 6; break;
            case 5 : data ^= 1 << 2; break;
            case 6 : data ^= 1 << 1; break;
            case 7 : data ^= 1 << 0; break;
        }

        return (d4 | (d3 << 1) | (d2 << 2) | (d1 << 3));
    }
    // int encode1 (int data) {
    //     const bool kernel[4][7] = {
    //         {1,1,1,0,0,0,1},
    //         {1,1,0,0,0,1,0},
    //         {1,0,1,0,1,0,0},
    //         {0,1,1,1,0,0,0},
    //     };
    //     int code = 0;
    //     int cod2 = 0;
    //     const int ker2[7] = {
    //       0b1110, 0b1101, 0b1011, 0b0001, 0b0010, 0b100, 0b1000
    //     };
    //
    //     const int ker3[4] {
    //       0b1000111,
    //       0b0100011,
    //       0b0010101,
    //       0b0001110,
    //     };
    //
    //     for (int i = 0; i < 4; i++) {
    //         cod2 ^= data & ker3[i];
    //         printf("%b %b\n", cod2, ker3);
    //     }
    //
    //     for (int j = 0; j < 7; j++) {
    //         // bool par = 0;
    //         for (int i = 0; i < 4; i++) {
    //             // bool bit = ((data >> i) & 1);
    //             // par ^= bit & kernel[i][j];
    //             code ^= (((data >> i) & 1) & kernel[i][j]) << (6 - j);
    //             // printf("(%i x %i) ", bit, kernel[i][j]);
    //         }
    //         // printf(" = %i\n", par);
    //     }
    //     // printf("%b %i\n", code, code);
    //     // printf("%b %i\n", cod2, cod2);
    //     return code;
    // }

    // int decode1 (int byte) {
    //
    //     bool chk[3][7] = {
    //         {1, 0, 1, 1,  1, 0, 0},
    //         {1, 1, 0, 1,  0, 1, 0},
    //         {1, 1, 1, 0,  0, 0, 1}
    //     };
    //
    //     for (int k = 0; k < 7; k++) {
    //         const int data = byte ^ (1 << k); // just modify 1 bit
    //         int synd = 0;
    //
    //         for (int i = 0; i < 3; i++) {
    //             for (int j = 0; j < 7; j++) {
    //                 bool bit = ((data >> j) & 1);
    //                 synd ^= (bit & chk[i][j]) << i;
    //                 // printf("(%i %i) ", chk[i][j], bit);
    //             }
    //             // printf("\n");
    //         }
    //         if (synd == 0) return data;
    //         // printf(" : %i", pary);
    //         // printf("\n");
    //     }
    //     return 0;
    // }

}

int main() {

    cout << "\n\n\n";

    int order = 256;
    int field[order];
    const int n = 2;
    const int total = (1 << n) - 1;
    const int ndata = total - n ;

    const int prim = 0b1011;
    const int genr = 0b10;
    int exp = 1;
    int max = 1 << 5;

    for (int i = 1; i < 16; i++) {
      if (exp >= 8) exp ^= prim;
      field[i] = exp;
      printf("%05b %i\n", exp, exp);
      exp <<= 1;
    }



}
