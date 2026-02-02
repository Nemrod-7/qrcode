#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>

#include "qr.hpp"
#include "rs.hpp"

using namespace std;

void showbits (const string &bits) {
    for (int i = 0; i < bits.size(); i += 8) {
        cout << stoi(bits.substr(i, 8), nullptr,2) << " ";
    }
    cout << endl;
}

int btoi (const std::vector<int> &bin, int st, int nd) {
    int num = 0;
    for (int i = 0; i < nd; i++) num |= bin[i + st] << i;
    return num;
}
int btoi (const std::string &bin, int st, int nd) {
    return stoi(bin.substr(st, nd), nullptr, 2);
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

std::string encode (const std::string &msg, int mode) {
    std::string bits;

    if (mode == NUMERIC) {
        for (int i = 0; i < msg.size(); i += 3) {
            char i1 = msg[i+0];
            char i2 = (i + 1) < msg.size() ? (msg[i+1]) : 0;
            char i3 = (i + 2) < msg.size() ? (msg[i+2]) : 0;
            std::string num = {i1,i2,i3};

            switch(num.size()) {
                case 1 : bits += std::bitset<4>(stoi(num)).to_string(); break;
                case 2 : bits += std::bitset<2>(stoi(num)).to_string(); break;
                case 3 : bits += std::bitset<10>(stoi(num)).to_string(); break;
            }
        }
    } else if (mode == ALPHANUM) {
        for (int i = 0; i < msg.size(); i += 2) {
            int i1 = alnum.find(msg[i+0]);
            int i2 = alnum.find(msg[i+1]);

            bits += std::bitset<11>(i1 * 45 + i2).to_string();
        }
    } else if (mode == BYTE) {
        for (int i = 0; i < msg.size(); i++) {
            bits += std::bitset<8>(msg[i]).to_string();
        }
    } else if (mode == KANJI) {

    }

    return bits;
}

void mark(std::vector<std::vector<int>> &grid, const std::vector<std::vector<int>> &wm, int x, int y) {
    for (int i = 0; i < wm.size(); i++) {
        for (int j = 0; j < wm.size(); j++) {
            grid[i + y][j + x] = wm[i][j];
        }
    }
}
std::vector<std::vector<int>> mk_grid (int version) {
    const int size = 17 + version * 4;

    const std::vector<std::vector<int>> wmark = { // version version 2 and above
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,1,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}};

    const std::vector<std::vector<int>> finder = {
        {1,1,1,1,1,1,1},
        {1,0,0,0,0,0,1},
        {1,0,1,1,1,0,1},
        {1,0,1,1,1,0,1},
        {1,0,1,1,1,0,1},
        {1,0,0,0,0,0,1},
        {1,1,1,1,1,1,1} };
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

    // make the finder patterns
    mark(grid,finder,0,0);
    mark(grid,finder,size - 7,0);
    mark(grid,finder,0,size-7);

    if (version > 1) mark(grid,wmark,size-9,size-9);
    // make timing pattern
    for (int i = 7; i < size - 7; i++) {
        grid[6][i] = grid[i][6] = i % 2 == 0;
    }

    grid[8][8] = 0;
    grid[4 * version + 9][8] = 1;  // dark module
    return grid;
}
std::vector<std::pair<int,int>> scan (const std::vector<std::vector<int>> &grid) {
    const int size = grid.size();
    int index = size - 1, cycle = 0;
    std::vector<std::pair<int,int>> zone;

    while (index > 0) {
        if (index == 6) index = 5;

        for (int i = 0; i < size; i++) {
            int y = cycle % 2 ? i : (size - 1 - i);

            for (int x = index; x > (index-2); x--) {
                if (grid[y][x] == 2) {
                    zone.push_back({x,y});
                }
            }
        }
        index -= 2, cycle++;
    }

    return zone;
}

std::vector<u8> bitstobyte (const std::string &bits, int start, int end) {
    std::vector<u8> v;
    for (int i = start; i < end; i += 8) {
        v.push_back(stoi(bits.substr(i, 8), nullptr, 2));
    }
    return v;
}

std::vector<std::vector<int>> qr_write (const std::string &msg, int ecc, int mask) {

    const int padding[2] = {0xec, 0x11}; // bin: 11101100 00010001
    int version = 0;
    int size = msg.size(); // 8 bits
    int mode = get_mode(msg); // 4 bits

    std::string info, bits ;
    std::string data = encode(msg, mode);

    while (true) {
        if (capacity[version][mode][ecc] >= msg.size()) break;
        else version++;
    }
    std::vector<std::vector<int>> grid = mk_grid(version);
    auto path = scan(grid);
    const int fill = capacity[version][mode][ecc] - msg.size() + 4;
    const int ec = neblocks[version][ecc]; // n ecc  codewords
    const int dc = ndblocks[version][ecc]; // n data codewords

    info += std::bitset<2>(ecc).to_string();
    info += std::bitset<3>(mask).to_string();
    // info += std::bitset<10>(eccinfo[ecc][mode ?]).to_string();
    bits += std::bitset<4>(mode).to_string();

    if (version < 10) {
        bits += std::string(length_bits[0][mode] - 8,'0') + std::bitset<8>(size).to_string();
    } else if (version < 27) {
        bits += std::string(length_bits[1][mode] - 8,'0') + std::bitset<8>(size).to_string();
    } else {
        bits += std::string(length_bits[1][mode] - 8,'0') + std::bitset<8>(size).to_string();
    }

    // switch (mode) {
    //   case BYTE : bits += std::bitset<8>(size).to_string(); break;
    //   case KANJI : bits += std::bitset<8>(size).to_string(); break;
    //   case NUMERIC : bits += std::bitset<10>(size).to_string(); break;
    //   case ALPHANUM : bits += std::bitset<9>(size).to_string(); break;
    // }

    bits += data;
    for (int i = 0; i < std::min(4, fill); i++) bits += '0';

    for (int i = 0; i < (capacity[version][mode][ecc]  - size); i++) {
        bits += std::bitset<8>(padding[i % 2]).to_string();
    }

    const polynomial edc = rs_encode(get_bits2(bits, ec + dc), ec);
    cout << show::simplified(edc) << "\n";

    for (int i = dc; i < edc.size(); i++) {
        bits += std::bitset<8>(edc[i]).to_string();
    }
    cout << bits.size() << " " << ec + dc << "\n";
    size = (17 + version * 4);

    for (auto &[x,y] : path) {
        grid[x][y] = set_mask(mask, x, y);
    }

    // print format infos on the grid
    for (int i = 0; i < info.size(); i++) {
        bool bit = info[i] - 48;
        grid[8][i] = bit;
        grid[size - i - 1][8] = bit;
    }
    // print bits on the grid
    for (int i = 0; i < bits.size(); i++) {
        auto &[x,y] = path[i];
        grid[y][x] = grid[y][x] ^ (bits[i] - 48);
    }
    Infos::show(ecc, mode, version);
    // Infos::grid(grid);
    return grid;
}
std::string qr_read (const std::vector<std::vector<int>> &qr) {
    const int size = qr.size();
    const int ecc = btoi(qr[8], 0, 2);
    const int mask =  btoi(qr[8], 2, 4);
    const int version = (size - 17) / 4;
    // auto path = scan(free_zone(version));
    const int ec = neblocks[version][ecc]; // n ecc  codewords
    const int dc = ndblocks[version][ecc]; // n data codewords
    std::string msg, bits;

    for (auto &[x,y] : scan(mk_grid(version))) {
        bits += (set_mask(mask, x, y) ^ qr[y][x]) + '0';
    }

    cout << bits.size() << " " << ec + dc << "\n";
    // const polynomial edc = get_bits2(bits, ec + dc);
    // cout << show::simplified(edc) << "\n";
    int mode = btoi(bits,0, 4);
    int mlen = 0;

    // get mode
    switch(mode) {
        case BYTE : mlen = btoi(bits, 4, 8); break;
        case KANJI : mlen = btoi(bits, 4, 8); break;
        case NUMERIC : mlen = btoi(bits, 4, 10); break;
        case ALPHANUM : mlen = btoi(bits, 4, 9); break;
    }

    // decode
    if (mode == NUMERIC) {

    } else if (mode == ALPHANUM) {
        int start = 13, end = (mlen * 11 / 2) + 13;

        for (int i = start; i < end; i += 11) {
            int ii = stoi(bits.substr(i, 11), nullptr, 2);
            msg += alnum[ii / 45] + alnum[ii % 45];
        }

    } else if (mode == BYTE) {
        for (auto ch : bitstobyte(bits, 12, mlen * 8 + 12)) {
            msg += ch;
        }
    } else if (mode == KANJI) {

    }

    Infos::show(ecc, mode, version);
    return msg;
}

int main () {

    std::string msg = "https://www.qrcode.com/";
    // const std::string msg = "Hello, world! 123";
    // msg = "Hi";
    auto qr = qr_write(msg, H, 7);

    std::string txt = qr_read(qr);

    // cout << txt;
}
