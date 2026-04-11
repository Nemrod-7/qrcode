#pragma once

#include <vector>
#include <map>
#include <filesystem>

#include "image.hpp"
#include "grid.hpp"

// using grid = std::vector<std::vector<int>>;
enum {NONE, ENCODING, DECODING};

////////////////////////////////////////////////////////////////////////////////
std::vector<int> detect_border (const Image &pic) {
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

    printf("%i %i %i %i\n", minx, miny, maxx, maxy  );

    return {minx, miny, maxx, maxy};
}
std::vector<std::vector<int>> rotate (const std::vector<std::vector<int>> &matrix) {

    const int size = matrix.size();
    std::vector<std::vector<int>> rotated(size, std::vector<int> (size));

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            rotated[i][j] = matrix[size - j - 1][i];
        }
    }

    return rotated;
}
bool identify (int x, int y, const std::vector<std::vector<int>> &qr, const std::vector<std::vector<int>> &pattern) {
    int cell = 0;

    for (int i = 0; i < pattern.size(); i++) {
        for (int j = 0; j < pattern[i].size(); j++) {
            if (qr[i + y][j + x] == pattern[i][j]) cell++;
        }
    }

    return cell > 40 ? true : false;
}
