#include <iostream>
#include <vector>
#include "include/qr.hpp"
// #include "include/rs.hpp"
using namespace std;


std::vector<int> write_blocks(int ndata, int ec, int nb, const std::string &bits) {

    const int rem = ndata % nb, lim = (nb - rem), end = ndata / nb;
    std::vector<int> bytes(ndata + ec * 4);

    for (int block = 0; block < nb; block++) {
        const int dc = block < lim ? end : end + 1;
        std::vector<int> rs_code(ec + dc);

        for (int i = 0; i < dc; i++) {
            int ii = i * nb + block + ((block >= lim && i == dc - 1) ? rem : 0) ;
            // rs_code[i] = std::stoi(bits.substr((block * dc + i) * 8, 8), nullptr,2);
            bytes[ii] = rs_code[i];

            printf("%3i ",  ii);
        }
        // printf("|");
     
        for (int i = 0; i < ec; i++) {
            int ii = i * nb + block  + ndata;
            bytes[ii] = rs_code[i + dc];
            // printf("%3i ", ii);
        }

        printf("\n");
    }
    return bytes;
}
void read_blocks(int ndata, int ec, int nb, const std::vector<int> &bytes) {
    const int rem = ndata % nb, lim = (nb - rem), end = ndata / nb;

    for (int block = 0; block < nb; block++) {
        const int dc = block < lim ? end : end + 1;
        std::vector<int> rs_code(ec + dc);

        for (int i = 0; i < dc; i++) {
            int ii = i * nb + block + ((block >= lim && i == dc - 1) ? rem : 0) ;
            printf("%3i ",  ii);
        }
        // printf("|");
     
        for (int i = 0; i < ec; i++) {
            int ii = i * nb + block + ndata;
            // printf("%3i ", ii);
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

    write_blocks(ndata, ec, nb, "");
    read_blocks(ndata, ec, nb, {});

}
