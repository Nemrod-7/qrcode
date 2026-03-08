#include <iostream>
#include <cstdint>
#include <vector>

#include "include/grid.hpp"

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
    //        data  ,   ecc
    // M1 =  2 bytes,  3 bytes,
    // M2 =  4 bytes,  6 bytes ? L
    // M3 =  7 bytes, 10 bytes,
    // M4 = 10 bytes, 14 bytes ?

    // L=1, M=0, Q=3, H=2
    //const int ecc = 1, mask = 4;
    //const int data = bitset<15>(information[ecc][mask]).to_ulong() ^ 0x5412;

    for (int version = 0; version < 20; version++) {

    }

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
