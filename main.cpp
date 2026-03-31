#include <iostream>
#include <vector>
#include <filesystem>

#include "include/qr.hpp"
#include "include/utils.hpp"
#include "include/image.hpp"

const std::string app_version = "V0.3";

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

        finder(pattern, 3, 3);

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
