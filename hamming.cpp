#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

int main () {
    const string ecc[3] = {
      "thisabcd",
      "thatbcde",
      "corncdef"
    };
    vector<int> eccb = {0xbc,0x2a,0x90,0x13,0x6b,0xaf,0xef,0xfd,0x4b,0xe0};
    vector<int> data = {0x40,0xd2,0x75,0x47,0x76,0x17,0x32,0x06,0x27,0x26,0x96,0xc6,0xc6,0x96,0x70,0xec};

    data = {0x27,0x54,0x77,0x61,0x73,0x20,0x62,0x72,0x69,0x6c,0x6c,0x69,0x67};
    // string msg = "thosbcde";
    // int maxv = 0, index = 0;

    for (int i = 0; i < data.size(); i++) {
        printf("%i %i\n", data[i], eccb[i % eccb.size()]);

    //     int dist = 0;
    //     for (int j = 0; j < 8; j++) {
    //         if (ecc[i][j] == msg[j]) {
    //             dist++;
    //             printf("%c", ecc[i][j]);
    //         }
    //     }
    //
    //     if (dist > maxv) {
    //         maxv = dist;
    //         index = i;
    //     }
    //     printf("\n");
    }


    // printf("%s\n", ecc[index]);
}
