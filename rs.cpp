#include <iostream>
#include <cstdint>
// #include <fstream>
// #include <iomanip>
#include <vector>
#include <bitset>
#include <algorithm>

#include "GF256.hpp"
#include "qr.hpp"

using namespace std;

void showbits (const string &bits) {
    for (int i = 0; i < bits.size(); i += 8) {
        cout << stoi(bits.substr(i, 8), nullptr,2) << " ";
    }
    cout << endl;
}

polynomial getEDC(const std::string &bits, int total) {
    int index = total - 1, ec = total - bits.size() / 8;
    polynomial p(total);

    for (int i = 0; i < bits.size(); i += 8) {
        p[index--] = stoi(bits.substr(i, 8), nullptr,2);
    }

    polynomial gen = gf256::generator(ec);
    return gf256::rest(p, gen);
}

void display(const polynomial &rem) {
    for (int i = 0; i < rem.size(); i++) {
        printf("%i ", rem[i]);
    }
    printf("\n");
}

int main () {

    // cout << "0x" << hex << setfill('0') << setw(2) << EXP[i]  << ", ";
    const string msg = "https://www.qrcode.com/";
    const u8 pad[2] = {236,17};

    const u8 version = 2, ecc = 2;
    int ec = neblocks[version][ecc]; // n ecc  codewords
    int dc = ndblocks[version][ecc]; // n data codewords

    auto bits = bitset<4>(4).to_string(); // BYTE
    bits += bitset<8>(msg.size()).to_string(); // SIZE
    for (auto &ch : msg) bits += bitset<8>(ch).to_string();
    bits += "0000";


    bits += bitset<8>(pad[0]).to_string();
    bits += bitset<8>(pad[1]).to_string();
    bits += bitset<8>(pad[0]).to_string();

    showbits(bits);
    polynomial edc = getEDC(bits, ec + dc);

    for (auto &byte : edc) bits += bitset<8>(byte).to_string();

    // display(edc);
    // Uint8Array(17) [1, 59, 13, 104, 189, 68, 209, 30, 8, 163, 65, 41, 229, 98, 50, 36, 59]
    // Uint8Array(16) [52, 61, 242, 187, 29, 7, 216, 249, 103, 87, 95, 69, 188, 134, 57, 20]

    // cout << gf256::show(p) << "\n";
    // cout << gf256::show(gen) << "\n";
    // cout << "--------------------\n";
    // cout << gf256::show(quo) << "\n";
    // cout << gf256::show(rem) << "\n";


    // printf("%i ", ec;
    // int val = 1;
    // for (int exp = 1; exp < 256; exp++) {
    //     cout << exp << " " << exp % 255;
    //     cout << "\n";
    //     val = val > 127 ? ((val << 1) ^ 285) : val << 1;
    // }

    // cout << "\nexit\n";
}
