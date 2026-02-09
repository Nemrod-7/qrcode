#include <iostream>
#include <vector>
#include <bitset>

using namespace std;


string encode (const string &src) {
    // input: "hey"
    // --> 104, 101, 121                  // ASCII values
    // --> 01101000, 01100101, 01111001   // binary
    // --> 000111111000111000000000 000111111000000111000111 000111111111111000000111  // tripled
    // --> "000111111000111000000000000111111000000111000111000111111111111000000111"  // concatenated
    string os;

    for (auto &byte : src) {
        for (int i = 7; i >= 0; i--) {
            os += string(3, (byte >> i &1) + '0');
        }
    }

    return os;
}
string decode (const string &src) {
    // input: "100111111000111001000010000111111000000111001111000111110110111000010111"
    // --> 100, 111, 111, 000, 111, 001, ...  // triples
    // -->  0,   1,   1,   0,   1,   0,  ...  // corrected bits
    // --> 01101000, 01100101, 01111001       // bytes
    // --> 104, 101, 121                      // ASCII values
    // --> "hey"
    string os, bin;

    for (int i = 0; i < src.size(); i += 3) {
        bin += src[i];

        if (i % 8 == 5) {
            os += stoi(bin, nullptr, 2);
            bin.clear();
        }
    }

    cout << os;

    return os;
}
int hamming_weight (int x) {

    int weight = 0;

    while (x > 0) {
        weight += x & 1;
        x >>= 1;
    }

    return weight;
}

int hamming_distance (const string &a, const string &b) {
    int weight = 0;

    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) weight++;
    }

    return weight;
}
int qr_check_format(int fmt) {
   int g = 0x537; // 0b10100110111
   for (int i = 4; i >= 0; i--) {
       if (fmt & (1 << ( i + 10))) {
           fmt ^= g << i;
       }
   }

   return fmt;
}


int qr_decode_format(int fmt) {
    int best_fmt = -1;
    int best_dist = 15;

    for (int i = 0;  i < 32; i++) {
      int test_code = (i << 10) ^ qr_check_format(i << 10);
      int test_dist = hamming_weight(fmt ^ test_code);

      if (test_dist < best_dist) {
          best_dist = test_dist;
          best_fmt = i;
      } else if (test_dist == best_dist) { 
          best_fmt = -1;
      }
    }

    return best_fmt;
}
int main () {

    // string code = encode("hey");
    // string text = decode(code);

    vector<string> dictionary = {"thisabcd", "thatbcde", "corncdef"};

    string code = "thisbcde";
    int minv = 8, index = 0;

    for (int i = 0; i < dictionary.size(); i++) {
        int dist = hamming_distance(code, dictionary[i]);

        if (dist < minv) {
            minv = dist;
            index = i;
        }

        // cout << i << " " << j << endl;
    }
    string text = dictionary[index];

    for (int i = 0; i < text.size(); i++) {
        cout << std::bitset<8>(text[i]) << " ";

        // if (dictionary[index][i] != code[i]) {
        //     cout << "[" << code[i] << "]";
        // } else {
        //     cout << code[i];
        // }
    }

}
