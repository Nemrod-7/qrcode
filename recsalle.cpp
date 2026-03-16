#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;
using u8 = unsigned short int;

const int white = 255;
const int black = 0;

struct Image { // Image structure
       int width, height;
       std::vector<u8> pixels;

       Image() {};
       Image (int w, int h, u8 pixval) { width = w, height = h, pixels = vector<u8>(h * w, pixval);}; // pixel data constant
       Image (int w, int h, initializer_list<u8> data) { // pixel data as given
          width = w, height = h, pixels = data;
       };
};
class Utils { // display utilities
    public :
        // static void img (const Image &img) {
        //     for (int y = 0; y < img.height; ++y) {
        //         for (int x = 0; x < img.width; ++x) {
        //
        //             if (img.pixels[y * img.width + x])
        //                 cout << setw(3) << "#";
        //             else
        //                 cout << setw(3) << " ";
        //         }
        //
        //         cout << endl;
        //     }
        // }
        // static void values (const Image &img) {
        //     for (int y = 0; y < img.height; ++y) {
        //         for (int x = 0; x < img.width; ++x) {
        //             cout << setw(4) << img.pixels[y * img.width + x];
        //         }
        //
        //         cout << endl;
        //     }
        // }

        static Image from_file (string name) {

            ifstream iss (name, ios::in | ios::binary);
            string type, tmp, w, h;
            int width, height, val, lin;

            iss >> type;
            // iss >> tmp;
            // if (tmp == "#")
            //     getline (iss, tmp, '\n');
            iss >> w;
            iss >> h;
            // iss >> tmp;

            width = stoi (w), height = stoi (h);
            Image img (width, height, 0);
            u8 pixel, *out = img.pixels.data();

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (type == "P5")
                        iss.read ((char*) &pixel, 1);
                    if (type == "P2") {
                        iss >> tmp;
                        pixel = stoi (tmp);
                    }

                    if (pixel > 255) {
                        cout << "invalid value";
                        return {0,0,{}};
                    }

                    // cout << pixel << ' ';
                    out[y * width + x] = pixel;
                }
            }

            return img;
        }

};

Image simpl_thresh (const Image &src) {

    Image img (src.width, src.height, 0);
    u8 dark = 255, light = 0;

    img.pixels = src.pixels;

    for (auto &it : img.pixels) {
        light = max (it, light);
        dark = min (it, dark);


    }
    u8 thre = (dark + light) * 0.5;

    for (auto &it : img.pixels) {
        it = (it < thre) ? 0 : 255;
    }

    return img;
}
vector<int> detect_border(const Image &pic) {
  int minx = 999, maxx = 0;
  int miny = 999, maxy = 0;

  for (int y = 0, last = false; y < pic.height; y++) {
      int dark = 0;

      for (int x = 0; x < pic.width; x++) {
          if (pic.pixels[y * pic.width + x] == black) {
              dark++;
          }
      }

      if (dark < pic.width / 5) {
          if (last == false) { maxy = std::max(maxy, y); }
          last = true;
      } else {
          if (last == true) miny = std::min(miny, y);
          last = false;
      }
  }

  for (int x = 0, last = false; x < pic.width; x++) {
      int dark = 0;

      for (int y = 0; y < pic.height; y++) {
          if (pic.pixels[y * pic.width + x] == black) {
              dark++;
          }
      }

      if (dark < pic.height / 5) {
          if (last == false) maxx = std::max(maxx, x);
          last = true;
      } else {
          if (last == true) { minx = std::min(minx, x); }
          last = false;
      }
  }

  return {minx, miny, maxx, maxy};
}

int main() {

    // ...
    // Step 5: Apply threshold
    // Step 6: Detect border
    // Step 7: Rescale image
    // Step 8: Rotate image

    Image pic;
    // 255 == white, 0 == black
    // vector<u8> line = {0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1, 0,0,0};
    int minv = 9999, len = 1;
    pic = Utils::from_file ("pictures/ys2XE.pgm");
    pic = simpl_thresh(pic);
    // printf("width %i height %i\n", pic.width, pic.height);
    const vector<int> border = detect_border(pic);
    printf("x : %i %i\n", border[1], border[3]);
    printf("y : %i %i\n", border[0], border[2]);
    map<int,int> cell;
    // get the minimal size of a bitcell
    for (int y = border[1]; y < border[3]; y++) {
      for (int x = border[0]; x < border[2]; x++) {
          const int ix = y * pic.width + x;
          // printf("%i ", pic.pixels[ix]);
          if (pic.pixels[ix] != pic.pixels[ix - 1]) {
              cell[len]++;
              len = 1;
          } else {
              len++;
          }
          // printf ("%i ", pic.pixels[i * src.width + j]);
      }
      // printf("\n");
    }

    printf("minimum size %i\n", minv);

    printf("end");
  }
