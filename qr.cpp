#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>

#include "qr.hpp"
#include "rs.hpp"
#include "grid.hpp"

using namespace std;

/* to implement :
  - KANJI coding and decoding
    ECI encoding and decodin
    Version information Golay code encoding and decoding
    Format information decoding
*/

using u8 = uint8_t;
using grid = std::vector<std::vector<int>>;

std::vector<int> getbyte (std::string bits, int st, int nd, int nbits) {
    std::vector<int> v;
    for (int i = st; i < nd; i += nbits) {
        int ii = stoi(bits.substr(i, nbits), nullptr, 2);
        v.push_back(ii);
    }
    return v;
}

int get_mode (const std::string &src) {

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
int get_len (int version, int mode) {
    if (version < 10) {
        return length_bits[0][mode];
    } else if (version < 27) {
        return length_bits[1][mode];
    } else {
        return length_bits[2][mode];
    }
}
int get_version(const std::vector<std::vector<int>> &qr) {
    int byte[2] = {0,0};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 6; j++) {
            byte[0] |= qr[j][qr.size() - 11 + i] << (i + j * 3);
            byte[1] |= qr[qr.size() - 11 + i][j] << (i + j * 3);
        }
    }
    // Golay Error Detection And Correction Code
    if (byte[0] != byte[1]) {
        // todo implement th corection algorithm
        cout << "\ninformation data corrupted.\n";
    }

    for (int i = 0; i < 41; i++) {
        if (versinfo[i] == byte[0]) {
            return i;
        }
    }
    return 0;
}
int gen_format_info(int ecc, int mask) {
    // generate the information error code
    // generator polynomial x^10 + x^8 + x^5 + x^4 + x^2 + x + 1 (binary form : 10100110111 integer : 1335)

    // the qr format info is in the form [ info ][error correction]
    //                                   [5 bits][     10 bits    ]
    // gen : 010100110111000 ( polynomial padded to match the information size (ie : 15 bits))
    // num : 011000000000000 (ex for ecc 1 and mask 4 -> [01100][00000 00000])
    const int num = ((ecc << 3) | mask) << 10;
    int gen = 1335 << 3; // "10100110111000";
    int res = num;

    // performs 4 division
    for (int i = 0; i < 4; i++) {
        res ^= (gen >> i);
    }
    // mask it (xor) with 101010000010010 (ie : 21522 in integer form (qr specification))
    return (num | res) ^ 21522;
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

std::string encode (const std::string &msg, int mode) {
    std::string bits;

    if (mode == NUMERIC) {
        string num = "   ";
        for (int i = 0; i < msg.size(); i += 3) {
            num[0] = msg[i+0];
            num[1] = (i + 1) < msg.size() ? (msg[i+1]) : 0;
            num[2] = (i + 2) < msg.size() ? (msg[i+2]) : 0;

            switch(num.size()) {
                case 1 : bits += std::bitset<4>(stoi(num)).to_string(); break;
                case 2 : bits += std::bitset<2>(stoi(num)).to_string(); break;
                case 3 : bits += std::bitset<10>(stoi(num)).to_string(); break;
            }
        }
    } else if (mode == ALPHANUM) {
        const int size = msg.size() % 2 == 0 ? msg.size() : msg.size() - 1;

        for (int i = 0; i < size; i += 2) {
            int i1 = alnum.find(msg[i+0]);
            int i2 = alnum.find(msg[i+1]);
            bits += std::bitset<11>(i1 * 45 + i2).to_string();
        }
        if (size % 2 != 1) bits += std::bitset<6>(alnum.find(msg.back())).to_string();
    } else if (mode == BYTE) {
        for (int i = 0; i < msg.size(); i++) {
            bits += std::bitset<8>(msg[i]).to_string();
        }
    } else if (mode == KANJI) {

    }

    return bits;
}
std::string decode (const std::string &bits, int mode, int size) {
    std::string txt;

    if (mode == NUMERIC) {
        for (auto &it : getbyte(bits, 0, (size * 10) / 3, 10)) {
            txt += std::to_string(it);
        }
    } else if (mode == ALPHANUM) {
        const int end = size / 2 * 11;

        for (auto &it : getbyte(bits, 0, end, 11)) {
            txt += alnum[it / 45];
            txt += alnum[it % 45];
        }

        if (size % 2 != 0) {
            int i1 = stoi(bits.substr(end , 6), nullptr, 2);
            if (i1 < alnum.size()) txt += alnum[i1];
        }
    } else if (mode == BYTE) {
        for (auto &it : getbyte(bits, 0, (size * 8) , 8)) {
            txt += it;
        }
    }

    return txt;
}

int evaluate(const std::vector<std::vector<int>> &grid) {
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
int get_mask(std::vector<std::vector<int>> grid) {

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

std::vector<std::vector<int>> qr_write (const std::string &msg, int ecc) { // up to version 7

    int version = 0;
    const int mode = get_mode(msg); // 4 bits

    while (true) {
        if (capacity[version][mode][ecc] >= msg.size()) break;
        else version++;
    }

    cout << "\n\n---encoding---\n\n";
    cout << "Version    : " << version << "\n";
    cout << "Mode       : " << Infos::mode(mode) << "\n";

    if (version == 0) return {};

    const int ndata = codewords[ecc][version]; // total of data codewords
    const int ec = ecsize[ecc][version];   // number of ecc codewords by block
    const int nb = err_blocks[ecc][version]; // number of ecc blocks
    const int dc = ndata / nb;               // number of data codeword for each ecc block

    cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n";
    cout << "Ecc mode   : " << Infos::ECC(ecc) << "\n";
    cout << "Ecc blocks : " << nb << "\n";

    auto grid = mk_grid(version);
    const auto path = grid_pos(grid);
    const auto info = info_pos(17 + version * 4);

    // write mode
    std:: string bits = std::bitset<4>(mode).to_string();
    // write message size
    bits += std::string(get_len(version, mode) - 8,'0');
    bits += std::bitset<8>(msg.size()).to_string();
    // write message size
    bits += encode(msg, mode);
    // pad the bits with zeros until it's size is a multiple of a byte;
    // const int fill = capacity[version][mode][ecc] - msg.size() + 4;
    // for (int i = 0; i < std::min(4, fill); i++) bits += '0';
    while(bits.size() % 8 != 0) bits += '0';
    const int max_bits = ndata * 8;

    for (int i = 0; bits.size() < max_bits; i++) {
        bits += std::bitset<8>(padding[i % 2]).to_string();
    }

    // make the reed-solomon polynomial for each block
    for (int cluster = 0; cluster < nb; cluster++) {
        std::string sub = bits.substr(cluster * dc * 8, dc * 8);
        polynomial data = get_bits2(sub, ec + dc);
        polynomial reed = rs_encode(data, ec);

        cout << "block " << cluster << " :: " << show::simplified(reed) << "\n";
        for (int i = dc; i < reed.size(); i++) {
            bits += std::bitset<8>(reed[i]).to_string();
        }
    }

    // write bits on the grid
    for (int i = 0; i < path.size(); i++) {
        auto &[x,y] = path[i];
        grid[y][x] = (i < bits.size()) ? bits[i] - 48 : 0 ;
    }

    int mask = get_mask(grid);

    cout << "Mask       : " << mask << "\n";

    // apply mask
    for (auto &[x,y] : path) {
        grid[x][y] ^= set_mask(mask, x, y);
    }

    // write format infos on the grid
    for (int i = 0; i < info.size(); i++) {
        auto &[x,y] = info[i];
        grid[x][y] = information[ecc][mask][i / 2] - '0';
    }

    // cout << Infos::show(ecc, mode, version);
    // cout << Infos::grid(grid);
    return grid;
}

std::string qr_read (const std::vector<std::vector<int>> &qr) { // up to version 7

    const int version = std::max(0ul, ((qr.size() - 17) / 4));

    if (version == 0) return "";
    if (version >= 7) {
        int version_to_check = get_version(qr);
    }

    const auto info = info_pos(qr.size());
    std::string format[2];
    std::string txt, src, bits;

    cout << "\n\n---decoding---\n\n";
    cout << "Version    : " << version << "\n" << flush;

    // read format infos
    for (int i = 0; i < info.size(); i++) {
        auto [x,y] = info[i];
        format[i % 2] += qr[x][y] + '0';
    }

    if (format[0] != format[1]) {
        cout << "The format information contains errors\n";
        // todo -> implement ecc check for format informations
    } else {
        cout << "The format information has no errors\n";
    }

    const int ecc = bin2int(format[0].substr(0,2)) ^ 3;
    const int mask = bin2int(format[0].substr(2,3)) ^ 5;

    const int ndata = codewords[ecc][version]; // total of data codewords
    const int ec = ecsize[ecc][version];   // number of ecc codewords by block
    const int nb = err_blocks[ecc][version]; // number of ecc blocks
    const int dc = ndata / nb;               // number of data codeword for each ecc block

    cout << "Mask       : " << mask << "\n";
    cout << "Ecc mode   : " << Infos::ECC(ecc) << "\n\n";

    // read qr code
    for (auto &[x,y] : grid_pos(mk_grid(version))) {
        src += (set_mask(mask, x, y) ^ qr[y][x]) + '0';
    }
    // reed-solomon decoding :
    // [              block 1            ][              block ...           ]
    // [[data polynomial][ecc polynomial]] [[data polynomial][ecc polynomial]]
    for (int block = 0; block < nb; block++) {
        std::string sub1 = src.substr(block * dc * 8, dc * 8); // binary data block
        std::string sub2 = src.substr(ndata * 8 + block * ec * 8, ec * 8); // binary ecc block

        cout << "ecc block " << block;
        polynomial code(get_bits2(sub1 + sub2, dc + ec));
        polynomial reed = rs_decode(code, ec);
        // cout << show::simplified(poly)  << "\n";
        // cout << show::simplified(eccc) ;
        // cout << "\n";
        for (int i = 0; i < dc; i++) {
            bits += std::bitset<8>(reed[i]).to_string();
        }
    }

    const int mode = bin2int(bits.substr(0, 4)); // the message mode is inscribed in the first 4 bits
    const int nbit = get_len(version, mode); // the message size is inscribed in the 8th, 9th or 10th following bits (depending of the version and te mode)
    const int mlen = bin2int(bits.substr(4, nbit));

    cout << "\n";
    cout << "Mode       : " << Infos::mode(mode) << "\n";
    cout << "Capacity   : " << capacity[version][mode][ecc] <<  "\n";

    bits = bits.substr(4 + nbit);
    txt = decode(bits, mode, mlen);

    cout << "[" << txt << "]\n";
    return txt;
}

int main () {


    cout << "\n\n\n";
    std::string msg = "https://jbirnick.github.io/";
    // msg = "https://qrcode.com/";
    // const std::string msg = "Hello, world! 123";
    // msg = "Hi";
    auto qr = qr_write(msg, H);
    std::string txt = qr_read(qr);

    // cout << Infos::grid(qr);

    // cout << "[" << txt << "]\n";
    cout  << "\nexit\n";
}
