#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "include/utils.hpp"
#include "include/image.hpp"
#include "include/qr.hpp"

// using namespace std;

////////////////////////////////////////////////////////////////////////////////
std::vector<int> filter(const std::vector<int> &line) {
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
////////////////////////////////////////////////////////////////////////////////


int main() {

    Image pic;
    // pic = Image::from_file ("pictures/Micro_QR_Example.pnm");
    // pic = Image::from_file ("pictures/ys2XE.pgm");
    pic = Image::from_file ("pictures/ex2.pnm");
    pic = simpl_thresh(pic);
    pic = crop(pic);

    std::vector<std::vector<int>> qr = to_vec(pic);
    // std::vector<std::vector<int>> qr  = rescale(pic);

    std::cout << Infos::grid(qr);

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

        std::string txt = qr_read(qr);

        std::cout << "text : [" << txt << "]";
    }



    std::cout << "\nend\n";
}
