#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>

using u8 = unsigned char;
enum {black = 0, white = 255 };


struct Image { // Image structure

    int width, height;
    std::vector<u8> pixels;

    Image() {};
    Image (int w, int h, u8 pixval) { width = w, height = h, pixels = std::vector<u8>(h * w, pixval);}; // pixel data constant
    Image (int w, int h, std::initializer_list<u8> data) { width = w, height = h, pixels = data; };

    static Image from_file(const std::string &name) {
        std::ifstream iss (name, std::ios::in | std::ios::binary);
        std::string format, tmp, w, h;

        iss >> format;
        // iss >> tmp;
        // if (tmp == "#")
        //     getline (iss, tmp, '\n');
        iss >> w;
        iss >> h;
        iss >> tmp; // max value

        const int width = stoi (w), height = stoi (h);
        const int channels = (format == "P3" || format == "P6") ? 3 : 1;
        const size_t dataSize = static_cast<size_t>(width * height * channels);
        Image img (width, height, 255);
        u8 pixel, *out = img.pixels.data();

        std::cout << "format : " << format << "\n";
        std::cout << "width  : " << w << "\n";
        std::cout << "height : " << h << "\n";
        std::cout << "maxval : " << tmp << "\n";

        if (format == "P1" || format == "P2") {
            for (size_t i = 0; i < dataSize; ++i) {
                iss >> pixel;
                out[i] = static_cast<unsigned char>(pixel);
            }
        } else if (format == "P3") {
            std::vector<u8> pixels(dataSize);

            for (size_t i = 0; i < dataSize; i++) {
                iss >> pixel;
                pixels[i] = static_cast<unsigned char>(pixel);
            }

            for (int i = 1; i < dataSize; i += 3) {
                out[i / 3] = pixels[i];
            }
        } else if (format == "P5") {
            iss.read(reinterpret_cast<char*>(out), dataSize);
        } else if (format == "P6") {
            std::vector<u8> pixels(dataSize);
            iss.read(reinterpret_cast<char*>(pixels.data()), dataSize);

            for (int i = 1; i < dataSize; i += 3) {
                out[i / 3] = pixels[i];
            }
        }

        return img;
    }
    static void to_file(const std::string &name, const Image &pic) {
        std::ofstream oss (name);

        oss << "P5" << "\n";
        oss << pic.width << " ";
        oss << pic.height << "\n";
        oss << 255 << "\n";

        oss.write(reinterpret_cast<const char *>(pic.pixels.data()), pic.pixels.size());
        oss.close();
    }
};

Image simpl_thresh (const Image &src) {

    Image img (src.width, src.height, 0);
    u8 dark = black, light = white;

    img.pixels = src.pixels;

    for (auto &it : img.pixels) {
        light = std::min (it, light);
        dark = std::max (it, dark);
    }
    u8 thre = (dark + light) * 0.5;

    for (auto &it : img.pixels) {
        it = (it < thre) ? 0 : 255;
    }

    return img;
}
Image crop (const Image &pic) {

    int minx = 0, miny = 0;
    int maxx = pic.width - 1, maxy = pic.height - 1;

    do {
        int cell = 0;

        for (int x = 0; x < pic.width; x++) {
            if (pic.pixels[miny * pic.width + x] == black) {
                cell++;
            }
        }

        if (cell > 6) break;
    } while (miny++ < pic.height);

    do {
        int cell = 0;

        for (int y = 0; y < pic.height; y++) {
            if (pic.pixels[y * pic.width + minx] == black) {
                cell++;
            }
        }
        // printf("cell %i \n", cell);
        if (cell > 6) break;
    } while (minx++ < pic.width);

    do {
        int cell = 0;
        for (int x = 0; x < pic.width; x++) {
            if (pic.pixels[maxy * pic.width + x] == black) {
                cell++;
            }
        }
        if (cell > 6) break;
    } while (maxy-->0);

    do {
        int cell = 0;
        for (int y = 0; y < pic.height; y++) {
            if (pic.pixels[y * pic.width + maxx] == black) {
                cell++;
            }
        }
        if (cell > 6) break;
    } while (maxx-->0);

    // printf("%i %i %i %i\n", minx, miny, maxx, maxy);
    int nh = maxy - miny, nw = maxx - minx;
    Image next;

    for (int y = miny; y < maxy; y++) {
        for (int x = minx; x < maxx; x++) {
            next.pixels.push_back(pic.pixels[y * pic.width + x]);
        }
    }
    next.width = nw, next.height = nh;
    // printf("width %i height %i \n", next.width, next.height);
    return next;
}

///////////////////////////// rescale (abandoned) //////////////////////////////////////////
int best_scale (const Image &pic) {
  int scale = 10;

  // get best scale value
  for (int i = 2; i < 10; i++) {
      if (pic.height % i == 0) scale = i;
      // printf("%i %i\n", i, pic.height % i);
  }

  return scale;
}
std::vector<std::vector<int>> rescale (const Image &pic) {
  const int scale = best_scale(pic);
  const int nw = pic.width / scale, nh = pic.height / scale;
  std::vector<std::vector<int>> qr (nh, std::vector<int>(nw)) ;

  const int x2 = scale * scale;
  const int med = 255 / 2;
  // scan(pic);

  for (int y = 0; y < nh; y++) {
      for (int x = 0; x < nw; x++) {
          const int sx = x * scale, sy = y * scale;
          int kernel = 0;

          for (int i = 0; i < scale; i++) {
              for (int j = 0; j < scale; j++) {
                  int pix = pic.pixels[(sy + i) * pic.width + (sx + j) ];

                  kernel += pix;
              }
          }
          qr[y][x] = (kernel / x2) < med ? 1 : 0;
      }
  }

  return qr;
}
////////////////////////////////////////////////////////////////////////////////
