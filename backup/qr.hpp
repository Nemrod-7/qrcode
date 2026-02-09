#include <string>

// enum Mtype{MODE, MODSIZE};
enum MODE {END,NUMERIC,ALPHANUM,STRUCTURED,BYTE,FNC1,_6_,ECI, KANJI};
// const int Mdb[4][2] = {{1, 4},{2, 6},{4, 8},{8, 16}};
const std::string alnum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

const unsigned short int padding[2] = {0xec, 0x11}; // bin: 11101100 00010001

const int err_blocks [4][41] = {
    //  Version: (note that index 0 is for padding, and is set to an illegal value)
    //  1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
    {0, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
    {0, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
    {0, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
    {0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
};

const int eccinfo[4][17] = {
  {0b1010001001,0b1110111110,0b0011100111,0b0111010000,0b1101100010,0b1001010101,0b0100001100,0b0000111011},
  {0b0101011111,0b0001101000,0b1100110001,0b1000000110,0b0010110100,0b0110000011,0b1011011010,0b1111101101},
  {0b0000010010,0b0100100101,0b1001111100,0b1101001011,0b0111111001,0b0011001110,0b1110010111,0b1010100000},
  {0b1111000100,0b1011110011,0b0110101010,0b0010011101,0b1000101111,0b1100011000,0b0001000001,0b0101110110},
};

// the number of bits necessary to record the size of the txt
// depends of the version and the mode :
//    N   AN    B        K
const int length_bits[3][9] = {
    {0,10, 9,0, 8,0,0,0, 8}, // version  1- 9
    {0,12,11,0,16,0,0,0,10}, // version 10-26
    {0,14,13,0,16,0,0,0,12}  // version 27-40
};


class Infos {
    public :
        static std::string ECC (int level) {
            switch(level) {
                case L : return "L"; break;
                case M : return "M"; break;
                case Q : return "Q"; break;
                case H : return "H"; break;
                default : return ""; break;
            }
        }
        static std::string mode (int level) {
            switch(level) {
                case BYTE : return "BYTE"; break;
                case KANJI : return "KANJI"; break;
                case NUMERIC : return "NUMERIC"; break;
                case ALPHANUM : return "ALPHANUM"; break;
                default : return ""; break;
            }
        }

        static std::string grid (std::vector<std::vector<int>> &grid) {
            std::string os;
            for (int i = 0; i < grid.size(); i++) {
                for (int j = 0; j < grid.size(); j++) {
                    os += (grid[i][j] == 0) ? ". " : "# ";
                }
                os += '\n';
            }
            return os;
        }
        static std::string show (int ecc, int mode, int version) {
          std::string os;
          // std::cout << "codewords : " << ((capacity[version][mode][ecc] + 4) - msg.size()) << "\n";
          os += "Ecc : " + Infos::ECC(ecc) + "\n";
          os += "Mode : " + Infos::mode(mode) + "\n";
          os += "Version : " + std::to_string(version) + "\n";
          os += "Capacity : " + std::to_string(capacity[version][mode][ecc]) + "\n";
          return os;
          // std::cout << "data codewords : " << dc << " " << "error codewords : " << ec << "\n";
        }
};
