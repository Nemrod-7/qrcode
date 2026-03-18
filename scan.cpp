#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct Image {
  int width, height;
  std::vector<unsigned char> data;
};

int search_left(int x, int y, int ref, const Image &pic) {

    for (int dx = x; dx < pic.width; dx++) {
        if (pic.data[y + pic.width + dx] != ref) { return dx; }
    }

    return 0;
}
int search_down(int x, int y, int ref, const Image &pic) {

    for (int dy = y; dy < pic.width; dy++) {
        if (pic.data[dy + pic.width + x] != ref) { return dy; }
    }

    return 0;
}
int limit_x(int x, int y, int ref, const Image &pic) {
    int lim = 9999;

    for (int y = 0; y < pic.height; y++) {
        lim = std::min(lim, search_left(x, y, ref, pic));
    }
    return lim;
}
void shrink_right() {

}

int main() {

    int scale = 6; // ie : bit size
    int width = 93, height = 93;

    Image pic;


    const int ref = 0;

    for (int y = 0; y < height; y++) {
      int length = 1;
      int ncell = 0;
      set<int> hist;

      for (int x = 1; x < width; x++) {
      //     int ix = y + pic.width + x;
      //
      //     if (pic.data[ix] != pic.data[ix - 1]) {
      //         hist.insert(length);
      //         length = 1;
      //         ncell++;
      //     } else {
      //         length++;
      //     }
      // }
    }





    std::cout << "\nend\n";
}
