#include <iostream>
#include <cstdint>
#include <vector>

#include "include/grid.hpp"
#include "include/rs.hpp"

using namespace std;

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
std::vector<std::vector<int>> micro_write (const std::string &txt) {
    int version = 1;
    std::vector<std::vector<int>> qr = micro_qr(version);



    return qr;
}
std::string micro_read (const std::vector<std::vector<int>> &qr) {
   
    const int version = (qr.size() - 11) / 2;
    // const int size = qr.size();
    std::cout << ":: decoding :: " << "\n"; 
    std::cout << "   micro QR    " << "\n";
    std::cout << "Version    : " << version << "\n" << std::flush;

    std::string format[2];
    std::string txt, src, bits;

    // format information zone
    for (int i = 0; i < 8; i++) {
       format[0] += qr[8][i + 1] + '0';
       format[1] += qr[i + 1][8] + '0'; 
    }

    if (format[0] != format[1]) {
        const int a = dec_format_info(bin2int(format[0]));
        const int b = dec_format_info(bin2int(format[1]));
        std::cout << "The format information contains errors.\n";

        if (a != b) {
            std::cout << "can't decode qr : too much damage.\n";
            return "";
        }
    } else {
        std::cout << "The format information has no errors\n";
    }

    const int ecc = bin2int(format[0].substr(0,2)) ^ 2;
    const int mask = bin2int(format[0].substr(2,3)) ^ 5;

    std::cout << "Mask       : " << mask << "\n";
    // std::cout << "Ecc mode   : " << Infos::ECC(ecc) << "\n\n";

    // // read qr code
    // for (auto &[x,y] : grid_pos(mk_grid(version))) {
    //     src += (set_mask(mask, x, y) ^ qr[y][x]) + '0';
    // }
    
    // const std::string sub1 = src.substr(dc * 8, dc * 8); // binary data block
    // const std::string sub2 = src.substr(ndata * 8 + ec * 8, ec * 8); // binary ecc block
    //
    // std::cout << "Decoding ecc block " ;
    // polynomial code(get_bits2(sub1 + sub2, dc + ec));
    // polynomial reed = rs_decode(code, ec);
    //
    // if (reed.size() == 0)  {
    //     std::cout << "can't decode qr : too much damage.\n";
    //     return "";
    // }
    // // std::cout << show::simplified(code) << "\n\n" ;
    // // std::cout << show::simplified(reed) << "\n\n" ;
    //
    // for (int i = 0; i < dc; i++) {
    //     bits += std::bitset<8>(reed[i]).to_string();
    // }
    //
    // const int mode = bin2int(bits.substr(0, 4)); // the message mode is inscribed in the first 4 bits
    // const int nbit = get_len(version, mode); // the message size is inscribed in the 8th, 9th or 10th following bits (depending of the version and te mode)
    // const int mlen = bin2int(bits.substr(4, nbit));
    //
    // std::cout << "Mode       : " << Infos::mode(mode) << "\n";
    // std::cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n\n";
    //
    // bits = bits.substr(4 + nbit);
    //
    // return decode(bits, mode, mlen);

    return "";
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

    for (int i = 0; i < 1; i++) {
        auto qr = micro_qr(i);

        micro_read(qr);

    }


}
