#include <iostream>
#include <vector>
#include "include/qr.hpp"
#include "include/rs.hpp"

int roundup(double d) {
    const int i = static_cast<int>(d); 
    return (d - i) > 0 ? i + 1 : i;
}

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


    int nsize  = 134;
    int ndata = 46;
    int nerro = 88;
    int nb = 4;
    std::vector<int> bytes (nsize);

    const int dc = roundup(ndata / (double)nb); 
    const int ec = roundup(nerro / (double)nb); 
    // printf("%i ", nerro + ndata);
    // write_blocks(dc, ec, nb, {});
    // read_blocks(dc, ec, nb, bytes);
    int version = 4;
    int ecc = QR::H;
    int mask = 0;


    const int infos = (gen_format_info((ecc << 3) | mask) ^ 0x5412);

    std::cout << QR::information[ecc][mask] << "\n";
}
