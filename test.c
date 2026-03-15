#include <stdio.h>


int hamming_encode (int data) {
  const bool d1 = ((data >> 3) & 1);
  const bool d2 = ((data >> 2) & 1);
  const bool d3 = ((data >> 1) & 1);
  const bool d4 = ((data >> 0) & 1);
  int block = d4 | (d3 << 1) | (d2 << 2) | (d1 << 4);

  block |= (d2 ^ d3 ^ d4) << 6;
  block |= (d1 ^ d3 ^ d4) << 5;
  block |= (d1 ^ d2 ^ d4) << 3;

  return block;
}

int hamming_decode (int data) {
    const bool d1 = ((data >> 4) & 1);
    const bool d2 = ((data >> 2) & 1);
    const bool d3 = ((data >> 1) & 1);
    const bool d4 = ((data >> 0) & 1);
    const int par = ((data >> 5) << 1) | ((data >> 3) & 1) ;
    const int syn = (d1 ^ d2 ^ d4) | ((d1 ^ d3 ^ d4) << 1) | ((d2 ^ d3 ^ d4) << 2);

    switch (par ^ syn)  {
        case 1 : data ^= 1 << 4; break;
        case 2 : data ^= 1 << 5; break;
        case 3 : data ^= 1 << 3; break;
        case 4 : data ^= 1 << 6; break;
        case 5 : data ^= 1 << 2; break;
        case 6 : data ^= 1 << 1; break;
        case 7 : data ^= 1 << 0; break;
    }

    return (d4 | (d3 << 1) | (d2 << 2) | (d1 << 3));
}

int main() {
    printf("\n\n\n");

    for (int i = 0; i < 8; i++) {

    }

    return 0;
}
