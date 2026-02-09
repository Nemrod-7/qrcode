#include <iostream>
#include <vector>
#include <bitset>

#include "rs.hpp"
#include "grid.hpp"

// using u8 = unsigned short int;
// using polynomial = std::vector<u8>;


using namespace std;

// std::string int2bin (const std::vector<u8> &v) {
//     std::string bin;
//     for (auto &it : v) bin += std::bitset<8>(it).to_string();
//     return bin;
// }
// int bin2int(const std::string &src) { return stoi(src, nullptr, 2); }

// polynomial get_bits2 (const std::string &bits, int total) {
//     polynomial p(total);
//
//     for (int i = 0; (i < total) && ((i * 8) < bits.size()); i++) {
//         p[i] = stoi(bits.substr(i * 8, 8), nullptr, 2);
//     }
//     return p;
// }

std::vector<u8> getbyte (std::string bits, int st, int nd, int nbits) {
    std::vector<u8> v;
    for (int i = st; i < nd; i += nbits) {
        v.push_back(stoi(bits.substr(i, nbits), nullptr, 2));
    }
    return v;
}
//////////////////////////////////reed-solomon//////////////////////////////////

int main () {

    std::cout << "\n\n\n";
    // cout << "0x" << hex << setfill('0') << setw(2) << EXP[i]  << ", ";
    const int version = 10;
    const int size = 17 + version * 4;

    // grid qr = mk_grid(version);
    // polynomial code(get_bits2(sub1 + sub2, dc + ec));

    for (int i = 7; i < 8; i++) {
        string num = bitset<6>(i).to_string();
        string src = bitset<18>(versinfo[i]).to_string();

        std::vector<u8> poly = getbyte(src, 0, src.size(), 6);
        // rs_decode(poly, 2);

        for (auto it : poly) {
            cout << to_string(it) << " ";
        }
        cout << "\n";
    }


    std::cout << "\nexit\n";
}
