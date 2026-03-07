#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

namespace bit {
    bool chk (uint64_t num, uint64_t ix) { return num >> ix &1ull; }
    uint64_t set (uint64_t num, uint64_t ix) { return num | 1ull << ix; }
    uint64_t tog (uint64_t num, uint64_t ix) { return num ^ 1ull << ix; }
    uint64_t clr (uint64_t num, uint64_t ix) { return num & ~(1ull << ix); }

    uint64_t cnt (uint64_t num) {
        uint64_t cnt = 0;

        do { cnt += num &1; } while (num >>= 1);

        return cnt;
    }

};

enum ECC {M, L, H, Q};

#include "grid.hpp"

const char information[4][8][16] = {
  { "101010000010010", "101000100100101", "101111001111100", "101101101001011", "100010111111001", "100000011001110", "100111110010111", "100101010100000" }, // M
  { "111011111000100", "111001011110011", "111110110101010", "111100010011101", "110011000101111", "110001100011000", "110110001000001", "110100101110110" }, // L
  { "001011010001001", "001001110111110", "001110011100111", "001100111010000", "000011101100010", "000001001010101", "000110100001100", "000100000111011" }, // H
  { "011010101011111", "011000001101000", "011111100110001", "011101000000110", "010010010110100", "010000110000011", "010111011011010", "010101111101101" }, // Q
};

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
int to_binary (const vector<int> &exp) {
  int num = 0;

  for (auto &p : exp) {
      num |= 1 << p;
  }
  return num;
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

int shift (int codewords, int generator, int ec, int dc) {
    for (int i = dc - 1; i >= 0; i--) {
        if (codewords >> (i + (ec - dc)) & 1) {
            codewords ^= generator << i;
        }
    }
    return codewords;
}

///////////////////////////////// BHC code /////////////////////////////////////
int gen_format_info (int ecc, int mask) { // (15,5) BCH code for t = 3 Suitable for hardware-style or embedded implementations
    // generate the information error code
    // standard narrow-sense primitive BCH over GF(2) generator polynomial
    // g(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1 (binary : 10100110111 integer : 1335 hex : 0x537)

    // (Bose–Chaudhuri–Hocquenghem code) [5 bits][     10 bits    ]
    // the qr format info is in the form [ info ][error correction]
    // [ecc (2 bits) | mask (3 bits)][ BCH code (10 bits) ]

    // gen : 01010 0110111000 ( polynomial padded to match the information size (ie : 15 bits))
    // num : 01100 0000000000 (ex for ecc 1 and mask 4 -> [01100][00000 00000])
    // For decoding (syndrome computation + Berlekamp–Massey), additional GF(16) arithmetic is required
    const int data_bits = ((ecc << 3) | mask) << 10;
    const int codewords = shift(data_bits, 0x537, 15, 5);
    // mask it (xor) with 101010000010010 (integer : 21522 hex :  0x5412)
    return (data_bits | codewords) ^ 0x5412;
}
int dec_format_info (int data) { // BHC error correction
    // Only because we have only 32 possible codes (ie : 2^5), sophisticated algorithms
    // such as Berlekamp-Massey for decoding BCH codes would be overkill in this case.
    // it's much easier to simply try each one and pick the one with the smallest distance.
    int form = -1, maxv = 15;

    for (int test = 0; test < 32; test++) {
        const int code = (test << 10) ^ shift(test << 10, 0x537, 15, 5);
        const int dist = bit::cnt(data ^ code);

        if (dist < maxv) {
            maxv = dist;
            form = test;
        } else if (dist == maxv) { // if there is more than one possibility ?
            maxv = -1;
        }
    }

    return form;
}

////////////////////////////////////////////////////////////////////////////////
int golay_code (int version) {
      // golay error code [6 bits ][ 12 bits             ]
      // version string :[version][error correction code]
      // g(x) : x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1 (binary : 1111100100101 integer : 7973 hex : 0x1f25)
      const int data_bits = version << 12;
      return data_bits | shift(data_bits, 0x1f25, 18, 6);
}



int main () {
    cout << "\n\n\n";
    //        data  ,   ecc
    // M1 =  2 bytes,  3 bytes,
    // M2 =  4 bytes,  6 bytes ? L
    // M3 =  7 bytes, 10 bytes,
    // M4 = 10 bytes, 14 bytes ?

    // L=1, M=0, Q=3, H=2
    const int ecc = 1, mask = 4;
    const int data = bitset<15>(information[ecc][mask]).to_ulong() ^ 0x5412;

    cout << bitset<15> (data) << "\n";


    // for (int version = 0; version < 20; version++) {
    //
    // }

    for (int ecc = 0; ecc < 4; ecc++) {
        for (int mask = 0; mask < 8; mask++) {
          // if (((data_bits >> 8) &1) == 0) {
          //     data_bits |= 1 << 8;
          // }
            // const int data_bits = bitset<15>(information[ecc][mask]).to_ulong();
            // const int codewords = shift(data_bits ^ 0x5412, 0x537, 15, 5);

        }
    }

    // for (int i = 0; i < 4; i++) {
    //     cout << grid(micro_qr(i)) << "\n";
    // }


}
