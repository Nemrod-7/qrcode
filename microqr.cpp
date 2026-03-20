#include <iostream>
#include <cstdint>
#include <vector>

#include "include/grid.hpp"
#include "include/rs.hpp"

using namespace std;

// const int capacity[5][2] = { ?????
//     // [data modules] [data capacity]
//     { 36,  5},
//     { 80, 10},
//     {132, 17},
//     {192, 24}
// };

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

bool set_mask(int level, int x, int y) {
    switch(level) {
        case 0 : return	(y + x) % 2 == 0;
        case 1 : return	(y) % 2 == 0;
        case 2 : return (x) % 3 == 0;
        case 3 : return (y + x) % 3 == 0;
        case 4 : return ( (y / 2) + (x / 3) ) % 2 == 0;
        case 5 : return ((y * x) % 2) + ((y * x) % 3) == 0;
        case 6 : return ( ((y * x) % 2) + ((y * x) % 3) ) % 2 == 0;
        case 7 : return ( ((y + x) % 2) + ((y * x) % 3) ) % 2 == 0;
        default : return 0;
    }
}
////////////////////////////////////////////////////////////////////////////////

namespace Micro {
////////////////////////////// specifications //////////////////////////////
  enum {NUMERIC, ALPHANUM, BYTE, KANJI};
  enum {M1,M2,M3,M4};
  enum {L,M,Q};

  // the format information forMicro QR differs from the standard QR.
  // the first 3 bits identify the version and the error eorrection used.
  // ie : 000 = (M1 L) 011 = (M3, L)
  // there is no logic behind that. Just take the infor from the two tables below.

  //                  version {M1,M2,M2,M3,M3,M4,M4,M4};
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
    //    L  M  Q    L  M  Q    L  M  Q    L  M  Q
      {{ 5, 0, 0},{ 0, 0, 0},{ 0, 0, 0},{ 0, 0, 0}},
      {{10, 8, 0},{ 6, 5, 0},{ 0, 0, 0},{ 0, 0, 0}},
      {{23,18, 0},{14,11, 0},{ 9, 7, 0},{ 6, 4, 0}},
      {{35,30,21},{21,18,13},{15,13, 9},{ 9, 8, 5}},
  };

  // codewords[ecc][version] -> number of [data codewords] for each version :
  //  L M Q
  const unsigned codewords[3][4] = {
  //  M1, M2, M3, M4,
    {  3,  5, 11, 16 }, // L
    {  0,  4,  9, 14 }, // M
    {  0,  0,  0, 10 }, // Q
  };

  // number of databits
  //  databits[ecc][version] -> number of [databits] possible
  //  M L H Q
  const unsigned databits[4][41] = {
  //M1,  M2,  M3,  M4,
  { 20, 40, 84, 128}, // L
  {  0, 32, 68, 112}, // M
  {  0 , 0,  0,  80}, // Q
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
          grid[0][i] = grid[i][0] = i % 2 == 0;
      }

      return grid;
  }
  std::vector<std::vector<int>> write (const std::string &txt) {
      int version = M1;
      std::vector<std::vector<int>> qr = Micro::make(version);

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
    // const int size = qr.size();
    std::cout << ":: decoding :: " << "\n";
    std::cout << "   micro QR    " << "\n";
    std::cout << "Version    : " << version + 1 << "\n";

    std::string format;
    std::string txt, src, bits;

    // format information zone
    for (int i = 1; i < 8; i++) format += qr[8][i] + '0';
    for (int i = 8; i > 0; i--) format += qr[i][8] + '0';

    const int info = dec_format_info(bin2int(format));
        // std::cout << "The format information contains errors.\n";
        // std::cout << "can't decode qr : too much damage.\n";

        // std::cout << "The format information has no errors\n";

    const int ecc = eclevel[bin2int(format.substr(0,3) )/* ^2 */];
    const int mask = mqmask[bin2int(format.substr(2,3)) /* ^ 5 */];

    std::cout << "Mask       : " << mask << "\n";
    // std::cout << "Ecc mode   : " << Infos::ECC(ecc) << "\n\n";

    // read qr code
    for (auto &[x,y] : grid_pos(Micro::make(version))) {
        src += (set_mask(mask, x, y) ^ qr[y][x]) + '0';
    }

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

    // for (int i = 0; i < dc; i++) {
    //     bits += std::bitset<8>(reed[i]).to_string();
    // }

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

};

int main () {

    //  (This is heuristical. Needs perspective correction, de-warping, re-clocking bad distortions, etc.)
    cout << "\n\n\n";
    //        data  ,   ecc
    // M1 =  2 bytes,  3 bytes,
    // M2 =  4 bytes,  6 bytes ? L
    // M3 =  7 bytes, 10 bytes,
    // M4 = 10 bytes, 14 bytes ?

    // L=1, M=0, Q=3, H=2
    //const int ecc = 1, mask = 4;
    //const int data = bitset<15>(information[ecc][mask]).to_ulong() ^ 0x5412;

    std::vector<std::vector<int>> micro = {
    {1,1,1,1,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,1,1,1,1},
    {1,0,1,1,1,0,1,0,0,0,1,1,1,1,1},
    {1,0,1,1,1,0,1,0,0,1,1,0,0,1,1},
    {1,0,1,1,1,0,1,0,0,1,1,0,0,0,1},
    {1,0,0,0,0,0,1,0,1,1,0,0,1,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,1,1,0,0,1,1,1,0,1,0,0},
    {0,0,0,1,1,1,0,0,1,0,1,0,1,1,1},
    {1,1,1,0,0,1,1,0,0,0,1,0,1,0,1},
    {0,0,1,0,0,1,1,1,0,0,1,1,0,1,0},
    {1,0,1,1,1,1,0,0,1,0,1,0,0,0,0},
    {0,0,1,1,0,1,1,1,1,1,0,0,1,0,1},
    {1,1,1,1,1,1,0,0,0,1,0,1,1,1,1}};

    /*
    numeric   : all
    alphanum  : > M1
    byte      : > M2
    kanji     : > M2

    */


    Micro::read(micro);

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

    // for (int i = 1; i < 5; i++) {
    //     auto qr = make(i);
    //
    //     for (int i = 0; i < qr.size(); i++) {
    //         for (int j = 0; j < qr.size(); j++) {
    //             cout << qr[i][j] << " ";
    //         }
    //         cout << '\n';
    //     }
    //     // micro_read(qr);
    // }


}
