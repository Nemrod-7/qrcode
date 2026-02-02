#include <iostream>
#include <cstdint>
// #include <fstream>
// #include <iomanip>
#include <vector>
#include <bitset>

#include "rs.hpp"
#include "qr.hpp"

using namespace std;

int main () {

    std::cout << "\n\n\n";
    // cout << "0x" << hex << setfill('0') << setw(2) << EXP[i]  << ", ";
    const std::string msg = "Hi";
    const u8 pad[2] = {236,17};

    const u8 version = 1, ecc = 0;
    int ec = neblocks[version][ecc]; // n ecc  codewords
    int dc = ndblocks[version][ecc]; // n data codewords

    auto bits = bitset<4>(4).to_string(); // BYTE
    bits += bitset<8>(msg.size()).to_string(); // SIZE
    for (auto &ch : msg) bits += bitset<8>(ch).to_string();
    bits += "0000";

    bits += bitset<8>(pad[0]).to_string();
    bits += bitset<8>(pad[1]).to_string();
    bits += bitset<8>(pad[0]).to_string();
    bits += bitset<8>(pad[1]).to_string();
    bits += bitset<8>(pad[0]).to_string();
    // cout << show::bits(bits) << "\n";

    const polynomial edc = rs_encode(get_bits2(bits, ec + dc), ec);
    // cout << show::simplified(edc) << "\n";

    for (int i = dc; i < edc.size(); i++) {
      cout << (int) edc[i] << " ";
    }
    // edc[3] = 9;
    // edc[7] = -1;
    // edc[10] = 50;
    // cout << ec << " " << dc << "\n";
    const polynomial clean = rs_decode(edc, ec);

    for (auto &byte : edc) {
        // bits += std::bitset<8>(byte).to_string();
    }

    cout << show::simplified(clean) << "\n";

    std::cout << "\nexit\n";
}
