#pragma once
#
#include <map>
#include "image.hpp"
#include "grid.hpp"

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
int cell_size (const Image &pic) {
    int size = 1, maxv = 0;
    std::map<int,int> cell;

    // get the minimal size of a bitcell
    for (int y = 0; y < pic.height; y++) {
        for (int x = 1; x < pic.width; x++) {
            const int ix = y * pic.width + x;
            if (pic.pixels[ix] != pic.pixels[ix - 1]) {
                cell[size]++;
                size = 1;
            } else {
                size++;
            }
        }
    }

    for (auto &[value,freq] : cell) {
        if (freq > maxv) {
            maxv = freq;
            size = value;
        }
    }

    return size;
}
std::vector<std::vector<int>> to_vec (const Image &pic) { // place all qr data bits on a vector 
    const int size = cell_size(pic);
    const int mid = (size * size) / 2;
    const std::vector<int> border = detect_border(pic);
    std::vector<std::vector<int>> qr;
    // printf("width %i height %i\n", pic.width, pic.height);
    for (int y = border[1]; y < border[3]; y += size) {
        std::vector<int> line;

        for (int x = border[0]; x < border[2]; x += size) {
            int cell = 0;

            for (int i = y; i < y + size; i++) {
                for (int j = x; j < x + size; j++) {
                    if (pic.pixels[i * pic.width + j] == black) {
                        cell++;
                    }
                }
            }

            line.push_back(cell > mid ? 1 : 0);
        }
        qr.push_back(line);
    }

    return qr;
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
bool recon (int x, int y, const std::vector<std::vector<int>> &qr, const std::vector<std::vector<int>> &pattern) {
    int cell = 0;

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (qr[i + y][j + x] == pattern[i][j]) cell++; 
        }
    }

    return cell > 40 ? true : false;
}
std::vector<std::vector<int>> detect (const Image &pic) {

    std::vector<std::vector<int>> qr = to_vec(pic);
    std::vector<std::vector<int>> pattern(7, std::vector<int>(7)); 
    const int size = qr.size();

    finder(pattern, 3, 3);

    while (!recon(0, 0, qr, pattern) || !recon(size - 7, 0, qr, pattern) || !recon(0, size - 7, qr, pattern)) {
        qr = rotate(qr);
    }

    return qr;
}
