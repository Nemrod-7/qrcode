#include <iostream>
#include <vector>
#include <filesystem>

#include "include/qr.hpp"
// #include "include/grid.hpp"
// #include "include/utils.hpp"
// #include "include/image.hpp"

const std::string app_version = "V0.3";
enum {NONE, ENCODING, DECODING};

namespace prog {

    const std::string txt = "Usage: qrcode [options] <input string> <file>...\n"
        "standard and micro qr code encoder and decoder.\n\n"
        "only numeric, alphanumeric and byte mode are available on this version.\n"
        "qrcode [-h] [-v] \n"
        "       [[-e] [-r <ecc>] [-m <mask>] [-t <text>] [-o <filename>]]\n"
        "       [[-d] [-i <filename>] [-o <filename>]]\n\n"
        "options : \n"
        "  -h, --help                         display this help and exit.\n"
        "  -v  --version                      display the version and exit.\n"
        "  -r  --error <level>                set the error correction level.\n"
        "  -m  --mask  <mask>                 force mask.\n"
        "  -t  --text                         <input text>.\n"
        "  -o  --output                       <output file>.\n"
        "  -e, --encode <text>                encode text for the selected level and save the qrcode to a file.\n"
        "  -d, --decode <file>                decode the png file and save the text to a file.\n"
        "  -v, --version                      display this version and exit.\n\n"
        "encode : if no file is submitted, then the qrcode will be sent to the terminal.\n"
        "         if no error correction level is selected, Ecc 'Quartile' will be selected by default.\n\n"
        "decode : works only from png, pnm or pgm files.\n\n"
        "error correction level : \n"
        "     L : tolerate  7 % of data loss.\n"
        "     M : tolerate 15 % of data loss.\n"
        "     Q : tolerate 25 % of data loss.\n"
        "     H : tolerate 30 % of data loss.\n"
        "The maximum qr version available is 22.\n"
        " \n"
        "ex : qrcode  -e -r high https://github.com/ link.pnm => encode the <text> with ecc level H and return the qr code into a <link.pnm> file.\n";

    std::string file;
    std::vector<std::string> input;
    int mask = 0, mode = NONE;
    char level = 'Q';

    char get_level (const std::string &arg) {
        if (arg == "L" || arg == "l" || arg == "low" || arg == "LOW") {
            return 'L';
        } else if (arg == "M" || arg == "m" || arg == "medium" || arg == "MEDIUM") {
            return 'M';
        } else if (arg == "Q" || arg == "q" || arg == "quartile" || arg == "QUARTILE") {
            return 'Q';
        } else if (arg == "H" || arg == "h" || arg == "high" || arg == "HIGH") {
            return 'H';
        } else {
            std::cout << "Unrecognized error correction level.\n";
        }
        return 'N';
    }

    void parse(int argc, char* argv[]) {
        const std::vector<std::string>  args(argv + 1, argv + argc);

        if (argc > 20) {
            throw std::runtime_error("too many input parameters");
        } else if (argc < 2) {
            // throw std::runtime_error(txt);
            throw std::runtime_error("try 'qrcode --help' for more information");
        }

        for (int i = 0; i < args.size(); i++) {
            // std::cout << "[" << args[i] << "]";
            if (args[i] == "-h" || args[i] == "--help") {
                std::cout << txt;
                return;
            } else if (args[i] == "-e" || args[i] == "--encode") {
                mode = ENCODING;
            } else if (args[i] == "-d" || args[i] == "--decode") {
                mode = DECODING;
            } else if (args[i] == "-r" || args[i] == "--error") {
                level = get_level(args[i + 1]);
                i += 1;
            } else if (args[i] == "-m" || args[i] == "--mask") {
                mask = stoi(args[i+1]);
                i += 1;
            } else if (args[i] == "-o" || args[i] == "--output") {
                file = args[i + 1];
                i += 1;
            } else if (args[i] == "-i" || args[i] == "--input") {
                file = args[i + 1];
                i += 1;
            } else if (args[i] == "-v" || args[i] == "--version") {
                std::cout << "Version : " << app_version << "\n";
                return ;
            } else if (args[i][0] == '-') {
                throw std::runtime_error(std::string(args[i]) + ": No such option");
            } else if (get_level(args[i]) != 'N') {
                level = get_level(args[i]);
            } else {
                input.push_back(args[i]);
            }
        }

        if (prog::mode == NONE) {
            throw std::runtime_error("qrcode: no mode selected.");
        } else if (prog::mode == ENCODING) {
            if (!input.size()) {
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

int main (int argc, char **argv) {

    try {
        // prog::parse(argc, argv);

    } catch (const std::exception &x) {
        std::cerr << "qrcode: " << x.what() << '\n';
        std::cerr << "Usage: qrcode [options...] ...\n";
        return EXIT_FAILURE;
    }

    prog::mode = ENCODING;
    prog::level = 'H';
    prog::input = {"https://jbirnick.github.io/"};
    // prog::input = {"pictures/ex2.pnm"};

    if (prog::mode == ENCODING) {
        std::string msg = prog::input[0];

        int ecc = QR::Q;
        enum QR::MODE mode = QR::NUMERIC;

        for (auto &it : msg) {
            if (isdigit (it)) mode = std::max (mode, QR::NUMERIC);
            if (isupper (it)) mode = std::max (mode, QR::ALPHANUM);
            if (islower (it)) mode = std::max (mode, QR::BYTE);
        }

        if (prog::level == 'L') ecc = QR::L;
        if (prog::level == 'M') ecc = QR::M;
        if (prog::level == 'Q') ecc = QR::Q;
        if (prog::level == 'H') ecc = QR::H;

        int version = 1;

        while (true) {
            if (QR::capacity[version][mode][ecc] >= msg.size()) break;
            else version++;
        }

        const int capacity = QR::capacity[version][mode][ecc];
        std::vector<std::vector<int>> qr;

        std::cout << "msg size  : " << msg.size() << "\n";
        std::cout << "version   : " << version << "\n";
        std::cout << "capacity  : " << capacity << "\n";
        std::cout << "qr mode   : " << QR::info::mode[mode] << "\n";
        std::cout << "ecc level : " << QR::info::level[ecc] << "\n";

        if (ecc == QR::H || capacity < msg.size()) {
            // qr = QR::write(msg, ecc);
        } else {
            switch (ecc) {
                // case QR::L : qr = MQR::write(msg, MQR::L) ; break;
                // case QR::M : qr = MQR::write(msg, MQR::M) ; break;
                // case QR::Q : qr = MQR::write(msg, MQR::Q) ; break;
            }
        }

    }

    if (prog::mode == DECODING) {
        std::string file = prog::input[0];
        std::string txt;
        std::vector<std::vector<int>> qr;
        std::vector<std::vector<int>> pattern(7, std::vector<int>(7));
        int index = 0;
        // finder(pattern, 3, 3);

        if (qr.size() < 20) { // Micro QR
            // while (!identify(0, 0, qr, pattern)) {
            //     if (index > 4) {
            //         std::cout << "can't find orientation.\n";
            //         break;
            //     }
            //     qr = rotate(qr);
            //     index++;
            // }

            // txt = MQR::read(qr);
        } else { // standard QR
            // while (!identify(0, 0, qr, pattern) || !identify(size - 7, 0, qr, pattern) || !identify(0, size - 7, qr, pattern)) {
            //     if (index > 4) {
            //         std::cout << "can't find orientation.\n";
            //         break;
            //     }
            //     qr = rotate(qr);
            //     index++;
            // }

            // txt = QR::read(qr);
        }



        std::cout << "text : [" << txt << "]\n";
    }


    std::cout << "\nexit\n";
    return EXIT_SUCCESS;
}
