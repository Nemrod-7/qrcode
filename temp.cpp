#include <iostream>
#include <fstream>
#include <set>
#include <map>

#include "include/image.hpp"

using namespace std;

static std::string showgrid (const std::vector<std::vector<int>> &grid) {
    std::string os;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            os += (grid[i][j] == 0) ? "  " : "# ";
        }
        os += '\n';
    }
    return os;
}

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

int main () {

    const std::string path = "pictures/";
    std::vector<std::vector<int>> qr;

    for (int i = 1; i < 8; i++) {
        std::string name = path + "test" + std::to_string(i) + ".pnm";
        std::ifstream file (name, std::ios::in | std::ios::binary);


        std::cout << name << "\n";
        if (!file) {
            std::cout << "no file to parse.\n";
        } else {
            Image img = Image::from_file(name);
            img = simpl_thresh(img);
            img = crop(img);
            qr = to_vec(img);


            std::cout << showgrid(qr) << "\n\n";
            file.close();
        }
    }

    cout << "\nexit\n";
}
