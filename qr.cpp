#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <bitset>

#include "include/qr.hpp"
#include "include/rs.hpp"
#include "include/grid.hpp"
#include "include/utils.hpp"
#include "include/image.hpp"

/* to implement :
    ECI encoding and decoding
    FNC1 encoding and decoding
    KANJI encoding and decoding
    Version information Golay code encoding and decoding => done
    Format information decoding => done
*/
////////////////////////////////////////////////////////////////////////////////
std::vector<int> filter (const std::vector<int> &line) {
    std::vector<int> pos = {0, line[0]};
    std::map<int,int> hist;
    int maxv = 0; // maximum length between 2 cells ?

    for (int i = 1; i < line.size(); i++) {
        int rate = line[i] - line[i-1];

        hist[rate]++;
        maxv = std::max(maxv, rate);

        if (rate > 3) pos.push_back(line[i]);
    }

    // for (auto &[value, freq] : hist) {
    //     std::cout << value << " : " << freq << "\n";
    // }

    return pos;
}
std::vector<int> scan (const Image &pic) {
    std::set<int> line;
int maxv = 0, minv = 9999;
    // printf("width %i height %i\n", pic.width, pic.height);

    for (int y = 0; y < pic.height; y++) {
        for (int x = 1; x < pic.width; x++) {
            int ix = y * pic.width + x;
            if (pic.pixels[ix] != pic.pixels[ix - 1] ) {
                line.insert(x);
            }
        }
    }

    for (int x = 0; x < pic.width; x++) {
        for (int y = 1; y < pic.height; y++) {
            int ix = y * pic.width + x, px = (y - 1) * pic.width + x;
            if (pic.pixels[ix] != pic.pixels[px]) {
                line.insert(y);
            }
        }
    }

    std::vector<int> pos = filter({line.begin(), line.end()});
    pos.push_back(pic.width);

    return pos;
}
std::vector<std::vector<int>> to_vec (const Image &pic) { // place all qr data bits on a grid
  const std::vector<int> pos = scan(pic);
  const int size = pos.size() - 1;
  std::vector<std::vector<int>> qr (size, std::vector<int>(size)) ;
  // std::cout << "size : " << size << '\n';
  for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
          int bl = 0, wh = 0;

          for (int y = pos[i]; y < pos[i + 1]; y++) {
              for (int x = pos[j]; x < pos[j + 1]; x++) {
                  const int pix = pic.pixels[y * pic.width + x];

                  pix == black ? bl++ : wh++;
              }
          }
          qr[i][j] = bl > wh ? 1 : 0;
      }
  }

  return qr;
}

/////////////////////////////// general ////////////////////////////////////
std::string getbinary (const std::string &bits, int st, int nbits) {
    return std::to_string( bin2int(bits.substr(st, nbits)) );
}
std::vector<int> getbyte (const std::string &bits, int st, int nd, int nbits) {
    std::vector<int> v;
    for (int i = st; i < nd; i += nbits) {
        v.push_back(bin2int(bits.substr(i, nbits)));
    }
    return v;
}

int roundup (double d) {
    const int i = static_cast<int>(d);
    return (d - i) > 0 ? i + 1 : i;
}
bool set_mask (int level, int x, int y) {
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

int evaluate (const std::vector<std::vector<int>> &grid) {
    int maxv = 0;
    int hor = 0, ver = 0;
    // cout << Infos::grid(grid);
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            hor = (j > 0 && grid[i][j-1] != grid[i][j]) ? 0 : hor + 1;
            ver = (j > 0 && grid[j - 1][i] != grid[j][i]) ? 0 : ver + 1;

            if (hor > 5 || ver > 5) maxv += 1;
            bool flag = true;

            for (int l = 0; l < 2; l++) {
                for (int k = 0; k < 2; k++) {
                    if ((i + l) < grid.size() && (j + k) < grid.size()) {
                        if (grid[i][j] != grid[i + l][j + k]) flag = false;
                    }
                }
            }
            // cout << flag << " ";
            if (flag == true) maxv += 3;
        }

    }

    return maxv;
}
int choose_mask (const std::vector<std::vector<int>> &grid) {

    const auto path = grid_pos(grid);
    int maxv = 9999, mask = 0;

    for (int i = 0; i < 8; i++) {
        std::vector<std::vector<int>> copy = grid;

        for (auto &[x,y] : path) {
            copy[x][y] = copy[x][y] ^ set_mask(i, x, y);
        }
        int pts = evaluate(copy);

        if (pts < maxv) {
            maxv  = pts;
            mask = i;
        }
    }

    return mask;
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
      std::string tmp;
      const int end = static_cast<int>(size / 3);

      for (int i = 0; i < end; i++) {
          tmp = getbinary(bits, i * 10, 10);

          txt += std::string((3 - tmp.size()), '0') + tmp;
      }

      switch (size % 3) {
          case 1 : txt += getbinary(bits, end * 10, 4); break;
          case 2 : txt += getbinary(bits, end * 10, 7); break;
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

////////////////////////////// QR specific ///////////////////////////////////
int QR::get_len (int version, int mode) {
    if (version < 10) {
        return length_bits[0][mode];
    } else if (version < 27) {
        return length_bits[1][mode];
    } else {
        return length_bits[2][mode];
    }
}
int QR::get_mode (const std::string &src) {

    enum MODE mode = NUMERIC;

    for (auto &it : src) {
        if (isdigit (it)) mode = std::max (mode, NUMERIC);//mode = max (mode , 1); // numeric
        if (isupper (it)) mode = std::max (mode, ALPHANUM); // alphanumeric
        if (islower (it)) mode = std::max (mode, BYTE); // byte
        // if (iseci (it)) mode = max (mode, ECI); // ECI
        // if (iskanji (it)) mode = max (mode, KANJI); // kanji
    }

    return mode;
}
int QR::get_version (const std::vector<std::vector<int>> &qr) {
    int byte[2] = {0,0};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 6; j++) {
            byte[0] |= qr[j][qr.size() - 11 + i] << (i + j * 3);
            byte[1] |= qr[qr.size() - 11 + i][j] << (i + j * 3);
        }
    }
    // Golay Error Detection And Correction Code
    if (byte[0] != byte[1]) {
        int a = dec_golay_code(byte[0]);
        int b = dec_golay_code(byte[1]);

        if (a != b) {
            std::cout << "\ninformation data corrupted.\n";
            return -1;
        }
    }

    for (int i = 1; i < 41; i++) {
        if (versinfo[i] == byte[0]) {
            return i;
        }
    }
    return -1;
}

std::vector<std::vector<int>> QR::make (int version) {
      const int size = 17 + version * 4;
      std::vector<std::vector<int>> grid(size, std::vector<int>(size, 2));

      for (int i = 0; i < 8; i++) {
          grid[i][8] = grid[8][i] = 0; // informations for the first eye
          grid[size - i - 1][8] = 0;   // informations for the second eye
          grid[8][size - i - 1] = 0;   // informations for the third eye

          for (int j = 0; j < 8; j++) { // make blank zones
              grid[i][j] = 0;            // first eye
              grid[size - i - 1][j] = 0; // second eye
              grid[j][size - i - 1] = 0; // third eye
          }
      }

      finder(grid, 3, 3);
      finder(grid, 3, size - 4);
      finder(grid, size - 4, 3);

      // if (version > 1)
      for (const auto &x : placement[version]) {
          for (const auto &y : placement[version]) {
              if ((x == 6 && y == 6) || (x == 6 && y == size - 7) || (x == size - 7 && y == 6) ) {
                  continue;
              }
              align(grid, x, y);
          }
      }

      if (version >= 7) {
          int byte = versinfo[version];

          // write version information
          for (int i = 0; i < 3; i++) {
              for (int j = 0; j < 6; j++) {
                  bool bit = byte >> (i + j * 3)&1;
                  grid[j][size - 11 + i] = bit;
                  grid[size - 11 + i][j] = bit;
              }
          }
      }

      // make timing pattern
      for (int i = 7; i < size - 7; i++) {
          grid[6][i] = grid[i][6] = i % 2 == 0;
      }

      grid[8][8] = 0;
      grid[4 * version + 9][8] = 1;  // dark module
      return grid;

}
std::vector<std::vector<int>> QR::write (const std::string &msg, int ecc) { // up to version 7

    int version = 0;
    const int mode = get_mode(msg); // 4 bits

    while (true) {
        if (capacity[version][mode][ecc] >= msg.size()) break;
        else version++;
    }

    std::cout << "\n\n---encoding---\n\n";
    std::cout << "Version    : " << version << "\n";
    std::cout << "Mode       : " << info::mode[mode] << "\n";

    if (version == 0) return {};

    const int ndata = codewords[ecc][version]; // total of data codewords
    const int nb = err_blocks[ecc][version];   // number of ecc blocks
    const int ec = ecsize[ecc][version];       // number of ecc codewords by block
    const int dc = roundup(ndata / (double) nb); // number of data codeword for each ecc block
    const int rem = ndata % nb, lim = (nb - rem), end = ndata / nb;

    std::cout << "Ecc mode   : " << info::level[ecc] << "\n";
    std::cout << "Ecc  cdwd  : " << ec * nb << "\n";
    std::cout << "Data cdwd  : " << dc * nb << "\n";
    std::cout << "Codewords  : " << (ndata + ec * nb) << "\n";

    // std::cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n";
    // std::cout << "Ecc blocks : " << nb << "\n";
    auto grid = make(version);
    const auto path = grid_pos(grid);
    const auto info = info_pos(17 + version * 4);

    // write mode
    std:: string bits = std::bitset<4>(mode).to_string();
    // write message size
    bits += std::string(get_len(version, mode) - 8,'0');
    bits += std::bitset<8>(msg.size()).to_string();
    // write message size
    bits += encode(msg, info::mode[mode]);
    // pad the bits with zeros until it's size is a multiple of a byte;
    // const int fill = capacity[version][mode][ecc] - msg.size() + 4;
    // for (int i = 0; i < std::min(4, fill); i++) bits += '0';
    while(bits.size() % 8 != 0) bits += '0';

    for (int i = 0; bits.size() < ndata * 8; i++) {
        bits += std::bitset<8>(padding[i % 2]).to_string();
    }

    // -=- RS Error Correction Code encoding -=-

    std::vector<int> bytes(ndata + ec * 4);

    for (int block = 0; block < nb; block++) {
        const int dc = block < lim ? end : end + 1;
        const std::string sub = bits.substr(block * dc * 8, dc * 8);
        const polynomial rs_code = rs_encode(get_bits2(sub, ec + dc), ec);
        std::cout << "block " << block << " :: " << show::simplified(rs_code) << "\n";

        for (int i = 0; i < dc; i++) {
            int ii = i * nb + block + ((block >= lim && i == dc - 1) ? rem : 0) ;
            bytes[ii] = rs_code[i];
        }

        for (int i = 0; i < ec; i++) {
            int ii = i * nb + block  + ndata;
            bytes[ii] = rs_code[i + dc];
        }
    }

    bits = "";
    for (auto &it : bytes) {
        bits += std::bitset<8>(it).to_string();
    }

    // write bits on the grid
    for (int i = 0; i < path.size(); i++) {
        auto &[x,y] = path[i];
        grid[y][x] = (i < bits.size()) ? bits[i] - 48 : 0 ;
    }


    const int mask = choose_mask(grid);
    std::cout << "Mask       : " << mask << "\n";

    // apply mask
    for (auto &[x,y] : path) {
        grid[y][x] ^= set_mask(mask, x, y);
    }
    // to implement : BCH (15 5) encoder
    // mask it (xor) with 101010000010010 (integer : 21522 hex :  0x5412)
    // std::string infos = (gen_format_info((ecc << 3) | mask) ^ 0x5412);

    std::cout << "format     : " << information[ecc][mask] << "\n";
    // write format infos on the grid
    for (int i = 0; i < info.size(); i++) {
        auto &[x,y] = info[i];
        grid[x][y] = information[ecc][mask][i / 2] - '0';
    }

    // std::cout << Infos::show(ecc, mode, version);
    // std::cout << Infos::grid(grid);
    return grid;
}
std::string QR::read (const std::vector<std::vector<int>> &qr) { // up to version 7

    const int version = std::max(0ul, ((qr.size() - 17) / 4));

    if (version == 0) return "";
    if (version >= 7) {
        int version_to_check = get_version(qr);

        if (version_to_check == -1) {
            std::cout << "error reading version information.\n";
        }
    }

    const auto info = info_pos(qr.size());
    std::string fortemp[2];
    std::string src, bits;
    std::cout << "\n\n---decoding---\n\n";
    std::cout << "Version    : " << version << "\n";

    // read format infos
    for (int i = 0; i < info.size(); i++) {
        auto [x,y] = info[i];
        fortemp[i % 2] += qr[x][y] + '0';
    }
    int format = dec_format_info(bin2int(fortemp[0]) ^ 0x5412);

    if (fortemp[0] != fortemp[1]) {
        const int b = dec_format_info(bin2int(fortemp[1]) ^ 0x5412);
        std::cout << "The format information contains errors.\n";

        if (format != b) {
          std::cout << "can't decode qr : too much damage.\n";
          return "";
        }
    } else {
        std::cout << "The format information has no errors\n";
    }

    std::cout << "for1 : " << fortemp[0] << "\n";
    const int ecc = (format >> 3);
    const int mask = (0b111 & format);

    const int ndata = codewords[ecc][version]; // total of data codewords
    const int ec = ecsize[ecc][version];   // number of ecc codewords by block
    const int nb = err_blocks[ecc][version]; // number of ecc blocks
    const int rem = ndata % nb, lim = (nb - rem), end = ndata / nb;

    std::cout << "Mask       : " << mask << "\n";
    std::cout << "Ecc mode   : " << info::level[ecc] << "\n";
    std::cout << "Ecc blocks : " << nb << "\n";

    if (nb == 0) {
        std::cout << "Invalid QRcode.\n";
        return "";
    }
    const int dc = roundup(ndata / (double) nb);               // number of data codeword for each ecc block

    // read qr code bits
    for (auto &[x,y] : grid_pos(make(version))) {
        src += (set_mask(mask, x, y) ^ qr[y][x]) + '0';
    }

    // reed-solomon decoding :
    // [              block 1            ][              block ...           ]
    // [[data polynomial][ecc polynomial]] [[data polynomial][ecc polynomial]]
    for (int block = 0; block < nb; block++) {
        const int dc = block < lim ? end : end + 1;
        polynomial rs_code(ec + dc);

        for (int i = 0; i < dc; i++) {
            int ii = i * nb + block + ((block >= lim && i == dc - 1) ? rem : 0) ;
            rs_code[i] = stoi(src.substr(ii * 8, 8), nullptr, 2);
        }

        for (int i = 0; i < ec; i++) {
            int ii = i * nb + block + ndata;
            rs_code[i + dc] = stoi(src.substr(ii * 8, 8), nullptr, 2);
        }
        std::cout << "Ecc block " << block << " :: " ;
        std::cout << show::simplified(rs_code) << "\n" ;

        rs_code = rs_decode(rs_code, ec);

        if (rs_code.size() == 0)  {
            std::cout << "can't decode qr : too much damage.\n";
            return "";
        }

        for (int i = 0; i < dc; i++) {
            bits += std::bitset<8>(rs_code[i]).to_string();
        }
    }

    const int mode = bin2int(bits.substr(0, 4)); // the message mode is inscribed in the first 4 bits
    const int nbit = get_len(version, mode); // the message size is inscribed in the 8th, 9th or 10th following bits (depending of the version and te mode)
    const int mlen = bin2int(bits.substr(4, nbit));

    std::cout << "Mode       : " << info::mode[mode] << "\n";
    std::cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n\n";

    bits = bits.substr(4 + nbit);

    return decode(bits, info::mode[mode], mlen);
}

int main () {

    //  (Needs perspective correction, de-warping, re-clocking bad distortions, etc.)
    std::vector<std::vector<int>> qr;
    std::string msg = "https://jbirnick.github.io/";
    // msg = "https://qrcode.com/";
    qr = QR::write(msg, QR::H);

    std::string txt = QR::read(qr);
    std::cout << "text : [" << txt << "]\n";


    Image pic;
    // pic = Image::from_file ("pictures/Micro_QR_Example.pnm");
    // pic = Image::from_file ("pictures/ys2XE.pgm");
    pic = Image::from_file ("pictures/ex2.pnm");
    pic = simpl_thresh(pic);
    pic = crop(pic);

    qr = to_vec(pic);
    
    if (qr.size() < 20) {
        // Micro QR

    } else {
        // QR code
        std::vector<std::vector<int>> pattern(7, std::vector<int>(7));
        const int size = qr.size();
        int index = 0;
        finder(pattern, 3, 3);

        while (!identify(0, 0, qr, pattern) || !identify(size - 7, 0, qr, pattern) || !identify(0, size - 7, qr, pattern)) {
            if (index > 4) {
                std::cout << "can't find orientation.\n";
                break;
            }
            qr = rotate(qr);
            index++;
        }

        std::string txt = QR::read(qr);
        std::cout << "text : [" << txt << "]";
    }


    std::cout << "\nexit\n";
}
