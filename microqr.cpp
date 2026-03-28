#include <iostream>
#include <cstdint>
#include <vector>
#include <bitset>

#include "include/grid.hpp"
#include "include/rs.hpp"
#include "include/qr.hpp"

using namespace std;

//////////////////////////////////// general /////////////////////////////////////
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

std::vector<int> getbyte (const std::string &bits, int st, int nd, int nbits) {
    std::vector<int> v;
    for (int i = st; i < nd; i += nbits) {
        int ii = stoi(bits.substr(i, nbits), nullptr, 2);
        v.push_back(ii);
    }
    return v;
}
bool set_mask(int level, int x, int y) {
    switch(level) {
        case 0  : return (y + x) % 2 == 0;
        case 1  : return (y) % 2 == 0;
        case 2  : return (x) % 3 == 0;
        case 3  : return (y + x) % 3 == 0;
        case 4  : return ((y / 2) + (x / 3) ) % 2 == 0;
        case 5  : return ((y * x) % 2) + ((y * x) % 3) == 0;
        case 6  : return (((y * x) % 2) + ((y * x) % 3) ) % 2 == 0;
        case 7  : return (((y + x) % 2) + ((y * x) % 3) ) % 2 == 0;
        default : return 0;
    }
}

std::string encode (const std::string &msg, const std::string &mode) {
    std::string bits;

    if (mode == "NUMERIC") {
        std::string num = "   ";

        for (int i = 0; i < msg.size(); i += 3) {
            num[0] = msg[i+0];
            num[1] = (i + 1) < msg.size() ? (msg[i+1]) : 0;
            num[2] = (i + 2) < msg.size() ? (msg[i+2]) : 0;

            if (num[1] == 0) {
                bits += std::bitset<4>(std::stoi(num)).to_string();
            } else if (num[2] == 0) {
                bits += std::bitset<7>(std::stoi(num)).to_string();
            } else {
                bits += std::bitset<10>(std::stoi(num)).to_string();
            }
        }
    } else if (mode == "ALPHANUM") {
        const int size = msg.size() % 2 == 0 ? msg.size() : msg.size() - 1;

        for (int i = 0; i < size; i += 2) {
            int i1 = alnum.find(msg[i+0]);
            int i2 = alnum.find(msg[i+1]);
            bits += std::bitset<11>(i1 * 45 + i2).to_string();
        }
        if (size % 2 != 1) bits += std::bitset<6>(alnum.find(msg.back())).to_string();
    } else if (mode == "BYTE") {
        for (int i = 0; i < msg.size(); i++) {
            bits += std::bitset<8>(msg[i]).to_string();
        }
    } else if (mode == "KANJI") {

    } else if (mode == "ECI") {
      /*
      The Extended Channel Interpretation  protocol defined in the
      AIM Inc International Technical Specification Extended Channel Interpretations,
      allows the output data stream to ahave interpretations different from
      that of The default character set.
      The default interpretation for QR code is ECI 000003 representing
      the ISO/IEC 8859-1 character set.
      */

    } else if (mode == "FNC1") {
        /*
          FNC1 mode is used for messages constaining specific data formats.
          In the first position it designates data formated in accordance
          to GS1 General specifications.
          In the second position it designates data formatted in accordance with a
          specific industry application previously agreed with AIM Inc.
        */
    }

    return bits;
}
std::string decode (const std::string &bits, const std::string &mode, int size) {
    std::string txt;

    if (mode == "NUMERIC") {
        for (auto &it : getbyte(bits, 0, (size * 10) / 3, 10)) {
            txt += std::to_string(it);
        }
    } else if (mode == "ALPHANUM") {
        const int end = size / 2 * 11;

        for (auto &it : getbyte(bits, 0, end, 11)) {
            txt += alnum[it / 45];
            txt += alnum[it % 45];
        }

        if (size % 2 != 0) {
            int i1 = stoi(bits.substr(end , 6), nullptr, 2);
            if (i1 < alnum.size()) txt += alnum[i1];
        }
    } else if (mode == "BYTE") {
        for (auto &it : getbyte(bits, 0, (size * 8) , 8)) {
            txt += it;
        }
    } else if (mode == "KANJI") {

    } else if (mode == "ECI") {
      /*
      The Extended Channel Interpretation  protocol defined in the
      AIM Inc International Technical Specification Extended Channel Interpretations,
      allows the output data stream to ahave interpretations different from
      that of The default character set.
      The default interpretation for QR code is ECI 000003 representing
      the ISO/IEC 8859-1 character set.
      */

    } else if (mode == "FNC1") {
        /*
          FNC1 mode is used for messages constaining specific data formats.
          In the first position it designates data formated in accordance
          to GS1 General specifications.
          In the second position it designates data formatted in accordance with a
          specific industry application previously agreed with AIM Inc.
        */
    }


    return txt;
}

////////////////////////////////////////////////////////////////////////////////
int MQR::get_mode (const std::string &src) {
    const std::string str[4] = {"NUMERIC", "ALPHANUM", "BYTE", "KANJI"};
    enum Mode mode = NUMERIC;

    for (auto &it : src) {
        if (isdigit (it)) mode = std::max (mode, NUMERIC);//mode = max (mode , 1); // numeric
        if (isupper (it)) mode = std::max (mode, ALPHANUM); // alphanumeric
        if (islower (it)) mode = std::max (mode, BYTE); // byte
        // if (iseci (it)) mode = max (mode, ECI); // ECI
        // if (iskanji (it)) mode = max (mode, KANJI); // kanji
    }

    return mode;
}
int MQR::get_info_symbol (int version, int ecc) {
    if (version == M1) {
        return  0;
    } else if (version == M2 && ecc == L) {
        return  1;
    } else if (version == M2 && ecc == M) {
        return  2;
    } else if (version == M3 && ecc == L) {
        return  3;
    } else if (version == M3 && ecc == M) {
        return  4;
    } else if (version == M4 && ecc == L) {
        return  5;
    } else if (version == M4 && ecc == M) {
        return  6;
    } else if (version == M4 && ecc == Q) {
        return  7;
    }
    return -1;
}
std::vector<std::vector<int>> MQR::make (int version) { // M1, M2, M3, M4
    const int size = 2 * version + 11;
    std::vector<std::vector<int>> grid(size, std::vector<int>(size, 2));
    finder(grid, 3, 3);

    for (int i = 0; i < 8; i++) {
        grid[7][i] = grid[i][7] = 0; // quiet zone
        grid[8][i + 1] = grid[i + 1][8] = 0; // format information zone
    }

    for (int i = 8; i < size; i++) { // timing patttern
        grid[0][i] = grid[i][0] = (i % 2 == 0);
    }

    return grid;
}
std::vector<std::vector<int>> MQR::write (const std::string &txt, int ecc) {

    int version = M1;
    const int mode = get_mode(txt);

    while (true) {
        if (capacity[version][mode][ecc] >= txt.size()) break;
        else version++;
    }

    std::cout << "\n\n---encoding---\n\n";
    std::cout << "Version    : M" << version + 1 << "\n";
    std::cout << "Mode       : " << info::mode[mode] << "\n";
    std::cout << "Ecc mode   : " << info::level[ecc] << "\n";

    const int sym = get_info_symbol(version, ecc);
    const int ec = ecsize[ecc][version];   // number of ecc codewords by block
    const int dc = codewords[ecc][version];
    std::string bits;
    // const int max_bits = ndata * 8;
    std::vector<std::vector<int>> qr = MQR::make(version);
    const auto path = grid_pos(qr, true);

    if (version == M1) {

    } else if (version == M2) {
      bits = std::bitset<1>(mode).to_string();
    } else if (version == M3) {
      bits = std::bitset<2>(mode).to_string();
    } else if (version == M4) {
      bits = std::bitset<3>(mode).to_string();
    }

    // std::cout << ec <<  " " << dc << "\n";
    bits += std::bitset<4>(txt.size()).to_string();
    bits += encode(txt, info::mode[mode]);

    // special terminator for MQR
    // if (version == M1) {
    //     bits += std::bitset<3>(0).to_string();
    // } else if (version == M2) {
    //     bits += std::bitset<5>(0).to_string();
    // } else if (version == M3) {
    //     bits += std::bitset<7>(0).to_string();
    // } else if (version == M4) {
    //     bits += std::bitset<9>(0).to_string();
    // }
    bits += std::string(version * 2 + 3, '0');

    while(bits.size() % 8 != 0) bits += '0';
    // ??? !! ???
    // for (int i = 0; bits.size() < max_bits; i++) {
    //     bits += std::bitset<8>(padding[i % 2]).to_string();
    // }

    // -=- RS Error Correction Codes -=-
    polynomial data = get_bits2(bits, ec + dc);
    polynomial reed = rs_encode(data, ec);

    // std::cout << show::simplified(data) << "\n";
    // std::cout << show::simplified(reed) << "\n";

    for (int i = dc; i < reed.size(); i++) {
        bits += std::bitset<8>(reed[i]).to_string();
    }

    for (int i = 0; i < path.size(); i++) {
        auto &[x,y] = path[i];
        qr[y][x] = (i < bits.size()) ? bits[i] - 48 : 0 ;
    }

    const int mask = 1;
    const int bch = gen_format_info(sym << 2 | mask) ^ 0x4445;

    for (int i = 0; i < 8; i++) {
        qr[i + 1][8] = (bch >> i) & 1;
        qr[8][i + 1] = (bch >> (14-i)) & 1;
    }

    for (auto &[x,y] : path) {
      qr[y][x] ^= set_mask(mqmask[mask], x, y);
    }
    // cout << grid(qr);
    return qr;
}
std::string MQR::read (const std::vector<std::vector<int>> &qr) {

    const int version = (qr.size() - 11) / 2;
    std::cout << ":: decoding :: " << "\n";
    std::cout << "   micro QR    " << "\n";
    std::cout << "Version    : M" << version + 1 << "\n";

    std::string str;
    std::string src, bits;

    // format information zone
    for (int i = 1; i < 8; i++) { str += qr[8][i] + '0'; }
    for (int i = 8; i > 0; i--) { str += qr[i][8] + '0'; }

    // xor with 100010001000101
    const int format = dec_format_info(bin2int(str) ^ 0x4445) ;

    // for (int i = 0; i < 32; i++) {
    //     int fmt = gen_format_info(i);
    //
    //     if (fmt == (bin2int(str) ^ 0x4445)) {
    //       printf("Debug :: ");
    //       printf("%05b %05b\n", format, i);
    //
    //     }
    // }
    // printf(" %015b %015b\n", format, bin2int(str) ^ 0x4445);
    if (vrlevel[(format >> 2)] != version) {
        std::cout << "format error.\n";
        // std::cout << vrlevel[(format >> 2)];
        return "";
    }
    // printf("%015b\n", format);

    const int ecc = eclevel[(format >> 2)];
    const int mask = 0b11 & format;
    const int ec = ecsize[ecc][version];
    const int dc = codewords[ecc][version];
    std::cout << "Mask       : " << mask << "\n";
    std::cout << "Ecc mode   : " << ecc << "\n\n";

    // read qr code
    for (auto &[x,y] : grid_pos(MQR::make(version), true)) {
        src += (set_mask(mqmask[mask], x, y) ^ qr[y][x]) + '0';
    }

    std::cout << ec <<  " " << dc << "\n";
    std::cout << "Decoding ecc block.\n" ;
    // std::cout << "DEBUG :: " << dc + ec << "::\n";
    const polynomial code (get_bits2(src, dc + ec));
    const polynomial reed = rs_decode(code, ec);

    // std::cout << show::simplified(code) << "\n" ;
    // std::cout << show::simplified(reed) << "\n" ;

    if (reed.size() == 0)  {
        std::cout << "can't decode qr : too much damage.\n";
        return "";
    }

    for (int i = 0; i < dc; i++) {
        bits += std::bitset<8>(reed[i]).to_string();
    }

    const int mode = bin2int(bits.substr(0, version));

    // if (version == M1) {
    //     mode = bin2int(bits.substr(0, 0));
    // } else if (version == M2) {
    //     mode = bin2int(bits.substr(0, 1));
    // } else if (version == M3) {
    //     mode = bin2int(bits.substr(0, 2));
    // } else if (version == M4) {
    //     mode = bin2int(bits.substr(0, 3));
    // }
    const int nbit = length_bits[version][mode]; // the message size is inscribed in the 8th, 9th or 10th following bits (depending of the version and te mode)
    const int mlen = bin2int(bits.substr(4, nbit));

    std::cout << "Mode       : " << info::mode[mode] << "\n";
    std::cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n\n";

    bits = bits.substr(4 + nbit);

    return decode(bits, info::mode[mode], mlen);
    return "";
}

int main () {

    //  (This is heuristical. Needs perspective correction, de-warping, re-clocking bad distortions, etc.)
    //        data  ,   ecc
    // M1 =  2 bytes,  3 bytes,
    // M2 =  4 bytes,  6 bytes ? L
    // M3 =  7 bytes, 10 bytes,
    // M4 = 10 bytes, 14 bytes ?

    const std::vector<std::vector<int>> wiki = {
        {1,1,1,1,1,1,1,0,1,0,1,0,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
        {1,0,1,1,1,0,1,0,0,1,1,1,1,0,1},
        {1,0,1,1,1,0,1,0,0,0,0,1,1,1,0},
        {1,0,1,1,1,0,1,0,1,1,1,1,0,1,0},
        {1,0,0,0,0,0,1,0,1,0,0,1,0,1,1},
        {1,1,1,1,1,1,1,0,1,0,1,1,0,1,1},
        {0,0,0,0,0,0,0,0,1,1,1,0,1,1,0},
        {1,1,1,1,0,1,1,0,0,1,1,0,1,1,0},
        {0,0,0,1,1,0,1,1,0,1,0,1,0,1,0},
        {1,1,1,0,0,1,1,1,1,0,1,1,0,0,0},
        {0,0,0,1,1,1,1,1,0,0,0,1,1,0,1},
        {1,1,0,0,0,0,0,0,1,0,1,1,1,0,1},
        {0,0,1,1,1,0,0,0,0,0,1,1,1,1,1},
        {1,0,0,1,0,1,1,0,1,0,1,0,0,0,1}
    };

    auto qr = MQR::write("01234567",  MQR::L);
    // MQR::write("WIKIPEDIA.ORG",  MQR::L);

    std::string txt = MQR::read(qr);



    cout << "\nexit\n";
}
