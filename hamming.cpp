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

namespace hamming {
    // hamming code (7 4) : total of 7 bits for 4 bits of data
    int encode1 (int data) {
        const bool kernel[4][7] = {
            {1,1,1,0,0,0,1}, {1,1,0,0,0,1,0}, {1,0,1,0,1,0,0}, {0,1,1,1,0,0,0},
        };
        int code = 0;

        for (int j = 0; j < 7; j++) {
            // bool par = 0;
            for (int i = 0; i < 4; i++) {
                // bool bit = ((data >> i) & 1);
                // par ^= bit & kernel[i][j];
                code ^= (((data >> i) & 1) & kernel[i][j]) << (6 - j);
                // printf("(%i x %i) ", bit, kernel[i][j]);
            }
            // printf(" = %i\n", par);
        }

        return code;
    }
    int encode2 (int data) {
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

    int decode1 (int byte) {

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
    int decode2 (int input) {
        const int d1 = (input >> 3 & 1);
        const int d2 = (input >> 2 & 1);
        const int d3 = (input >> 1 & 1);
        const int d4 = (input >> 0 & 1);
        const int par = input >> 4;

        const int p1 = (d2 ^ d3 ^ d4) << 2;
        const int p2 = (d1 ^ d3 ^ d4) << 1;
        const int p3 = (d1 ^ d2 ^ d4) << 0;
        const int syn = p1 | p2 | p3;

        switch (par ^ syn)  {
            case 1 : input ^= 1 << 4; break;
            case 2 : input ^= 1 << 5; break;
            case 3 : input ^= 1 << 3; break;
            case 4 : input ^= 1 << 6; break;
            case 5 : input ^= 1 << 2; break;
            case 6 : input ^= 1 << 1; break;
            case 7 : input ^= 1 << 0; break;
        }

        return input;
    }
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

    const int n = 2;
    const int total = (1 << n) - 1;
    const int ndata = total - n ;

    vector<int> d(total), p(total - ndata);

    for (int i = 2; i < 10; i++) {
        int total = (1 << i) - 1;
        int ndata = total - i ;

        printf("%i %i %i\n",i,  total, ndata);
    }

    // for (int j = 0; j < n - 1; j++) {
    //     for (int i = 0; i < n; i++) {
    //         if (j != i) p[j] ^= d[i];
    //     }
    // }

    // cout << bitset<7>(code);
    // for (int i = 6; i > 3; i--) {
    //   printf("%i %i\n", (byte >>i & 1), p[6 - i]);
    // }

    // string msg = "1010011001000111011000011000110001";
    // correct(4,6, msg);
    // correct(2,3, "11011010001");


}
