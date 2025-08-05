#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

enum { NUMERIC, ALPHANUM, BYTE, KANJI};
std::string alnum = "|123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

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

    return v;
}
int main () {

    int mode = NUMERIC;
    string msg = "2501";
    string bits;

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

    bits += "000";
    cout << endl;
    string txt;
    int len = msg.size();

    if (mode == NUMERIC) {

      int nbits = 10;
        int start = 0, end = (msg.size() * nbits) / 3;

        for (int i = start; i < end; i += nbits) {
            int ii = stoi(bits.substr(i, nbits), nullptr, 2);

            txt += std::to_string(ii);
        }

    }
    else if (mode == ALPHANUM) {
      int nbits = 11;
    int start = 0, end = (msg.size() * nbits) / 2;

    for (int i = 0; i < end; i += nbits) {
        int ii = stoi(bits.substr(i, nbits), nullptr, 2);

        txt += alnum[ii / 45] + alnum[ii % 45];
    }

  } else if (mode == BYTE) {
    int nbits = 8;
    int start = 0, end = (msg.size() * nbits) ;

    for (int i = 0; i < end; i += nbits) {
        int ii = stoi(bits.substr(i, nbits), nullptr, 2);

        txt += to_string(ii);
    }
  }

    cout << endl;

}
