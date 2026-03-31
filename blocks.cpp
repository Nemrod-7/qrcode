#include <iostream>
#include <vector>
#include "include/qr.hpp"
// #include "include/rs.hpp"
using namespace std;


void write_blocks(int dc, int ec, int nb, const std::vector<std::vector<int>> &blocks) {

    for (int i = 0; i < dc; i++) {
        for (int block = 0; block < nb; block++) {
            const int ix = i + block * dc ;
            printf("%3i ", ix);
            // printf("[%3i %3i]", block, i);
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < ec; i++) {
        for (int block = 0; block < nb; block++) {
            const int ix = i + block * ec ;
            printf("%3i ", ix + dc);
            // printf("[%3i %3i]", block, i + dc);
        }
        printf("\n");
    }

    printf("\n");
}
void read_blocks(int dc, int ec, int nb, const std::vector<int> &bytes) {

    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < dc; j++) {
            const int ix = j * nb + i ;
            printf("%3i ", ix);
        }

        printf("|");
        for (int j = 0; j < ec; j++) {
            const int ix = j * nb + i;
            printf("%3i ", ix + dc * nb);
        }

        printf("\n");
    }


}

int main () {

    const int ecc = QR::H;
    const int version = 5;

    const int ndata = QR::codewords[ecc][version];
    const int nb = QR::err_blocks[ecc][version];
    const int ec = QR::ecsize[ecc][version];
    const int rem = ndata % nb, lim = (nb - rem), end = ndata / nb;
    // [11,22] // [11,22] // [12,22] // [12,22]
    for (int block = 0; block < nb; block++) {
        int dc = block < lim ? end : end + 1;
        int total = dc + ec;
        cout << dc << " ";
    }
    cout << endl;

    for (int i = 0; i < (end +  1); i++) {
        for (int block = 0; block < nb; block++) {
            const int ix = i + block * (end +  1) ;


            printf("%3i ", ix + 1);
            // printf("[%3i %3i]", block, i);
        }
        printf("\n");
    }



}
