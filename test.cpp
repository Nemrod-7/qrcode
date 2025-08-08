#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

enum MODE {END,NUMERIC,ALPHANUM,STRUCTURED,BYTE,FNC1,_6_,ECI, KANJI};

std::string alnum = "|123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

const int length_bits[3][9] = {
    {0,10, 9,0, 8,0,0,0, 8}, // version  1- 9
    {0,12,11,0,16,0,0,0,10}, // version 10-26
    {0,14,13,0,16,0,0,0,12}  // version 27-40
};


int getlen (int version, int mode) {
  if (version < 10) {
      return length_bits[0][mode];
  } else if (version < 27) {
      return length_bits[1][mode];
  } else {
      return length_bits[1][mode];
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

std::vector<int> getbyte (std::string bits, int st, int nd, int nbits) {
    std::vector<int> v;
    for (int i = st; i < nd; i += nbits) {
        int ii = stoi(bits.substr(i, nbits), nullptr, 2);
        v.push_back(ii);
    }
    return v;
}
std::string decode (const std::string &bits, int mode, int size) {
    std::string txt;

    if (mode == NUMERIC) {
        for (auto &it : getbyte(bits, 0, (size * 10) / 3, 10)) {
            txt += std::to_string(it);
        }
    } else if (mode == ALPHANUM) {
        for (auto &it : getbyte(bits, 0, (size * 11) / 2, 11)) {
            txt += alnum[it / 45] + alnum[it % 45];
        }
    } else if (mode == BYTE) {
        for (auto &it : getbyte(bits, 0, (size * 8) , 8)) {
            txt += it;
        }
    }

    return txt;
}

int main () {

    int version = 1;
    int mode = NUMERIC;
    string msg = "250190";
    string bits = std::bitset<4>(mode).to_string();

    bits += std::string(getlen(version, mode) - 8, '0') + std::bitset<8>(msg.size()).to_string();

    std::string num = "   ";
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

    bits += "000";
    bits += std::bitset<8>(236).to_string();
    bits += std::bitset<8>(17).to_string();
    bits += std::bitset<8>(236).to_string();
    ////////////////////////////////////////////////////////////////////////////
    // mode = getbyte(bits, 0,4,4)[0];

    int nbits = getlen(version, mode);
    int size = getbyte(bits, 4, 4 + nbits, nbits)[0];

    bits = bits.substr(4 + nbits);
    string txt = decode(bits, mode, size);

    
    cout << endl;

}
