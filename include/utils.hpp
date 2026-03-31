#pragma once

#include <vector>
#include <map>
#include <filesystem>

#include "image.hpp"
#include "grid.hpp"

// using grid = std::vector<std::vector<int>>;
enum {NONE, ENCODING, DECODING};

namespace prog {

    const std::string txt = "Usage: qrcode [OPTION]... [FILE]\n"
        "standard and micro qr code encoder and decoder.\n\n"
        "only numeric, alphanumeric and byte mode are available on this version.\n"

        "options : \n"
        "  -h, --help                         display this help and exit\n"
        "  -w, --encode <text> <ecc> <file>   encode text for the selected level and save the qrcode to a file.\n"
        "  -d, --decode <file> <file>         decode the png file and save the text to a file.\n\n"
        "  -v, --version                      display this version and exit\n"
        "encode : if no file is submitted, then the qrcode will be sent to the terminal.\n"
        "         if no error correction level is selected, Ecc 'Quartile' will be selected by default.\n\n"
        "decode : works only from png, pnm or pgm files.\n\n"
        "error correction level : \n"
        "     L : tolerate  7 % of data loss\n"
        "     M : tolerate 15 % of data loss\n"
        "     Q : tolerate 25 % of data loss\n"
        "     H : tolerate 30 % of data loss\n"
        "The maximum qr version available is 22.\n"
        " \n"
        "ex : qrcode -e high https://github.com/ link.pnm => encode the <text> with ecc level H and return the qr code into a pnm file.\n";

    std::vector<std::string> input;
    int mode = NONE;
    char level = 'Q';

    void parse(int argc, char* argv[]) {
        const std::vector<std::string>  args(argv + 1, argv + argc);

        if (argc > 64) {
            throw std::runtime_error("too many input parameters");
        } else if (argc < 2) {
            throw std::runtime_error(txt);
            // throw std::runtime_error("try 'qrcode --help' for more information");
        }

        for (const auto &arg : args) {
            if (arg == "-h" || arg == "--help") {
                std::cout << txt;
                return;
            } else if (arg == "-e" || arg == "--encode") {
                mode = ENCODING;
            } else if (arg == "-d" || arg == "--decode") {
                mode = DECODING;
            } else if (arg == "L" || arg == "l" || arg == "low" || arg == "LOW") {
                level = 'L';
            } else if (arg == "M" || arg == "m" || arg == "medium" || arg == "MEDIUM") {
                level = 'M';
            } else if (arg == "Q" || arg == "q" || arg == "quartile" || arg == "QUARTILE") {
                level = 'Q';
            } else if (arg == "H" || arg == "h" || arg == "high" || arg == "HIGH") {
                level = 'H';
            } else if (arg[0] == '-') {
                throw std::runtime_error(std::string(arg) + ": No such option");
            } else {
                input.push_back(arg);
            }
        }

        if (prog::mode == NONE) {
            throw std::runtime_error("qrcode: no mode selected.");
        } else if (prog::mode == ENCODING) {
            if (input.size() == 0) {
                throw std::runtime_error("qrcode: no text to encode.");
            }

        } else if (prog::mode == DECODING) {
            if (!input.size()) {
                throw std::runtime_error("qrcode: no file to decode.");
            }

            if (!std::filesystem::exists(input[0])) {
                throw std::runtime_error(std::string(input[0]) + ": No such file");
            }
        }
    }
};
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
