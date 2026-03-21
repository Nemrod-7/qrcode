#include <iostream>
#include <cstdint>
#include <vector>
#include <bitset>

#include "include/grid.hpp"
#include "include/rs.hpp"

using namespace std;

const std::string alnum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";
const unsigned int padding[2] = {0xec, 0x11}; // bin: 11101100 00010001

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

            switch(num.size()) {
                case 1 : bits += std::bitset<4>(std::stoi(num)).to_string(); break;
                case 2 : bits += std::bitset<2>(std::stoi(num)).to_string(); break;
                case 3 : bits += std::bitset<10>(std::stoi(num)).to_string(); break;
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

namespace MQr {
////////////////////////////// specifications //////////////////////////////
  enum {NUMERIC, ALPHANUM, BYTE, KANJI};
  enum {M1,M2,M3,M4};
  enum {L,M,Q};

  namespace info {
      const std::string ecc[4] = {"Low", "Medium", "Quartile"};
      const std::string mode[4] = {"NUMERIC", "ALPHANUM", "BYTE", "KANJI"};
  };

  // the format information forMQr QR differs from the standard QR.
  // the first 3 bits identify the version and the error eorrection used.
  // ie : 000 = (M1 L) 011 = (M3, L)
  // there is no logic behind that. Just take the infor from the two tables below.

  const unsigned vrlevel[8] = {M1,M2,M2,M3,M3,M4,M4,M4};
  const unsigned eclevel[8] = { L, L, M, L, M, L, M, Q};

  // the fourth and fifth data bits of the format information contain
  // the data mask pattern reference, only mask 1, 4, 6 and 7 are allowed
  // for the micro qr version.
  const unsigned mqmask[4] = { 1, 4, 6, 7};

  //                                         [         data codewords         ] [            ecc codewords             ]
  // the qr binary data is encoded like this [[[mode][data length][data][0000]] [ecc block 1][ecc block 2][ecc block...]

  // the message received (numeric, alphanumeric, byte or kanji) is encoded and the error correction code chosen determine the size needed and so the size of the grid.
  // this table is used just to determine the version of the micro qr code and nothing else. : capacity[version][mode][ecc] -> maximum of encoded [data] codewords allowed .
  //
  const unsigned capacity[4][4][3] = {
    //  NUMERIC,    ALPHANUM,    BYTE,    KANJI
    //   L  M  Q    L  M  Q    L  M  Q    L  M  Q
      {{ 5, 0, 0},{ 0, 0, 0},{ 0, 0, 0},{ 0, 0, 0}},
      {{10, 8, 0},{ 6, 5, 0},{ 0, 0, 0},{ 0, 0, 0}},
      {{23,18, 0},{14,11, 0},{ 9, 7, 0},{ 6, 4, 0}},
      {{35,30,21},{21,18,13},{15,13, 9},{ 9, 8, 5}},
  };

  // codewords[ecc][version] -> number of [data codewords] for each version :
  const unsigned codewords[3][4] = {
  //  M1, M2, M3, M4,
    {  3,  5, 11, 16 }, // L
    {  0,  4,  9, 14 }, // M
    {  0,  0,  0, 10 }, // Q
  };

  //  ecsize[ecc][version] -> number of [ecc codwords] needed for each block.
  const unsigned ecsize[3][4] = {
  //  M1, M2, M3, M4,
    {  2,  5,  6,  8 }, // L
    {  0,  6,  8, 10 }, // M
    {  0,  0,  0, 14 }, // Q
  };

  // the number of bits necessary to record the size of the txt
  // depends of the version and the mode :
  //   N A B K
  const unsigned int length_bits[4][4] = {
      {3,0,0,0}, // M1
      {4,3,0,0}, // M2
      {5,4,4,3}, // M3
      {6,5,5,4}, // M4
  };


  std::vector<std::vector<int>> make (int version) { // M1, M2, M3, M4
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
  std::vector<std::vector<int>> write (const std::string &txt, int ecc) {

      int version = M1;
      const int mode = ALPHANUM;

      while (true) {
          if (capacity[version][mode][ecc] >= txt.size()) break;
          else version++;
      }

      std::string bits;
      const std::vector<std::vector<int>> qr = MQr::make(version);
      // bits = std::bitset<2>(get_mode(txt)).to_string();
      bits = std::bitset<2>(mode).to_string();
      bits = std::bitset<4>(txt.size()).to_string();

      for (const auto &ch : txt) {
          bits += encode(txt, info::mode[mode]);
      }

      if (version == 1) {
        // const int mode = 0;
        // const int ecc = L;

      } else if (version == 2) {
        // const int mode = 0;

      } else if (version == 3) {
        // const int mode = 00;
        // const int ecc = L;

      } else if (version == 4) {
        // const int mode = 000;
      }

      return qr;
  }


std::string read (const std::vector<std::vector<int>> &qr) {

    const int version = (qr.size() - 11) / 2;
    std::cout << ":: decoding :: " << "\n";
    std::cout << "   micro QR    " << "\n";
    std::cout << "Version    : M" << version + 1 << "\n";

    std::string str;
    std::string src, bits;

    // format information zone
    for (int i = 1; i < 8; i++) str += qr[8][i] + '0';
    for (int i = 8; i > 0; i--) str += qr[i][8] + '0';

    // xor with 100010001000101
    const int format = dec_format_info(bin2int(str) ^ 0x4445) ;

    if (vrlevel[(format >> 2)] != version) {
        std::cout << "format error.\n";
        return "";
    }
    // printf("%015b\n", format);

    const int ecc = eclevel[(format >> 2)];
    const int mask = mqmask[(0b11 & format)];
    const int ec = ecsize[ecc][version];
    const int dc = codewords[ecc][version];
    std::cout << "Mask       : " << mask << "\n";
    std::cout << "Ecc mode   : " << ecc << "\n\n";

    // read qr code
    for (auto &[x,y] : grid_pos(MQr::make(version), true)) {
        src += (set_mask(mask, x, y) ^ qr[y][x]) + '0';
    }

    std::cout << ec <<  " " << dc << "\n";
    std::cout << "Decoding ecc block.\n" ;
    // std::cout << "DEBUG :: " << dc + ec << "::\n";
    const polynomial code (get_bits2(src, dc + ec));
    const polynomial reed = rs_decode(code, ec);

    std::cout << show::simplified(code) << "\n" ;
    std::cout << show::simplified(reed) << "\n" ;

    if (reed.size() == 0)  {
        std::cout << "can't decode qr : too much damage.\n";
        return "";
    }

    for (int i = 0; i < dc; i++) {
        bits += std::bitset<8>(reed[i]).to_string();
    }

    const int mode = bin2int(bits.substr(0, 4)); // the message mode is inscribed in the first 4 bits
    const int nbit = length_bits[version][mode]; // the message size is inscribed in the 8th, 9th or 10th following bits (depending of the version and te mode)
    const int mlen = bin2int(bits.substr(4, nbit));

    std::cout << "Mode       : " << info::mode[mode] << "\n";
    std::cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n\n";

    bits = bits.substr(4 + nbit);

    return decode(bits, info::mode[mode], mlen);
    return "";
}

};

int main () {

    //  (This is heuristical. Needs perspective correction, de-warping, re-clocking bad distortions, etc.)
    cout << "\n\n\n";
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

    MQr::write("wikipedia.org",  MQr::L);
    // std::string txt = MQr::read(wiki);

    for (int ecc = 0; ecc < 4; ecc++) {
        for (int mask = 0; mask < 8; mask++) {
            // if (((data_bits >> 8) &1) == 0) {
            //     data_bits |= 1 << 8;
            // }
            // const int data_bits = bitset<15>(information[ecc][mask]).to_ulong();
            // const int codewords = shift(data_bits ^ 0x5412, 0x537, 15, 5);

        }
    }
    // int i = 0;
    // auto grid = MQr::make(MQr::M3);
    //
    // for (auto &[x,y] : grid_pos(grid, true)) {
    //     grid[y][x] = i++  ;
    // }
    // for (int i = 0; i < grid.size(); i++) {
    //     for (int j = 0; j < grid.size(); j++) {
    //         printf("%3i ", grid[i][j]);
    //     }
    //
    //     printf("\n");
    // }

    cout << "\nexit\n";
}
