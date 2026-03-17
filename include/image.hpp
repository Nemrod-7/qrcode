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
    Image (int w, int h, std::initializer_list<u8> data) { // pixel data as given
        width = w, height = h, pixels = data;
    };

    static Image from_file(const std::string &name) {
        std::ifstream iss (name, std::ios::in | std::ios::binary);
        std::string type, tmp, w, h;
        int width, height, val, lin;

        iss >> type;
        // iss >> tmp;
        // if (tmp == "#")
        //     getline (iss, tmp, '\n');
        iss >> w;
        iss >> h;
        iss >> tmp; // max value 
        // std::cout << tmp;
        width = stoi (w), height = stoi (h);
        Image img (width, height, 0);
        u8 pixel, *out = img.pixels.data();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (type == "P5") {
                    iss.read ((char*) &pixel, 1);
                } else if (type == "P6") {
                    iss.read ((char*) &pixel, 1);
                } else if (type == "P2") {
                    iss >> tmp;
                    pixel = stoi (tmp);
                }

                if (pixel > 255) {
                    std::cout << "invalid value";
                    return {0,0,{}};
                }

                out[y * width + x] = pixel;
            }
        }

        return img;
    }

};

Image simpl_thresh (const Image &src) {

    Image img (src.width, src.height, 0);
    u8 dark = black, light = white;

    img.pixels = src.pixels;

    for (auto &it : img.pixels) {
        light = std::max (it, light);
        dark = std::min (it, dark);


    }
    u8 thre = (dark + light) * 0.5;

    for (auto &it : img.pixels) {
        it = (it < thre) ? 0 : 255;
    }

    return img;
}

