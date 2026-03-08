#include <algorithm>
#include "gf256.hpp"

namespace bit {
    bool chk (uint64_t num, uint64_t ix) { return num >> ix &1ull; }
    uint64_t set (uint64_t num, uint64_t ix) { return num | 1ull << ix; }
    uint64_t tog (uint64_t num, uint64_t ix) { return num ^ 1ull << ix; }
    uint64_t clr (uint64_t num, uint64_t ix) { return num & ~(1ull << ix); }

    uint64_t cnt (uint64_t num) {
        uint64_t cnt = 0;

        do { cnt += num &1; } while (num >>= 1);

        return cnt;
    }
};

std::string int2bin (const std::vector<u8> &v) {
    std::string bin;
    for (auto &it : v) bin += std::bitset<8>(it).to_string();
    return bin;
}
int bin2int(const std::string &src) { return stoi(src, nullptr, 2); }
//////////////////////////////////reed-solomon//////////////////////////////////
// polynomial forney (const polynomial &synd, const polynomial &errpos, int errSize) { // formey algorithm
//
//     polynomial syn = synd;
//     u8 termX, termY;
//
//     for (int i = 0; i < errpos.size(); i++) {
//         // termX = errSize - 1 - errpos[i];
//         termX = gf256::EXP[errSize - 1 - errpos[i]];
//
//         for (int j = 0; j < syn.size() - 1; j++) {
//             // termY = gf256::mul(syn[j], termX);
//             // termY ^= syn[j + 1];
//             // termY = termY;
//             syn[j] = gf256::mul(syn[j], termX) ^ syn[j + 1];
//         }
//         syn.pop_back();
//     }
//
//     return syn;
// }

int shift (int codewords, int generator, int ec, int dc) {
    for (int i = dc - 1; i >= 0; i--) {
        if (codewords >> (i + (ec - dc)) & 1) {
            codewords ^= generator << i;
        }
    }
    return codewords;
}

///////////////////////////////// BHC code /////////////////////////////////////
int gen_format_info (int ecc, int mask) {
    // (15,5) BCH code for t = 3 Suitable for hardware-style or embedded implementations
    // generate the information error code
    // standard narrow-sense primitive BCH over GF(2) generator polynomial
    // g(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1 (binary : 10100110111 integer : 1335 hex : 0x537)

    // (Bose–Chaudhuri–Hocquenghem code) [5 bits][     10 bits    ]
    // the qr format info is in the form [ info ][error correction]
    // [ecc (2 bits) | mask (3 bits)][ BCH code (10 bits) ]

    // gen : 01010 0110111000 ( polynomial padded to match the information size (ie : 15 bits))
    // num : 01100 0000000000 (ex for ecc 1 and mask 4 -> [01100][00000 00000])
    // For decoding (syndrome computation + Berlekamp–Massey), additional GF(16) arithmetic is required
    const int data_bits = ((ecc << 3) | mask) << 10;
    const int codewords = shift(data_bits, 0x537, 15, 5);
    // mask it (xor) with 101010000010010 (integer : 21522 hex :  0x5412)
    return (data_bits | codewords) ^ 0x5412;
}
int dec_format_info (int data) {
    // BHC error correction
    // Only because we have only 32 possible codes (ie : 2^5), sophisticated algorithms
    // such as Berlekamp-Massey for decoding BCH codes would be overkill in this case.
    // it's much easier to simply try each one and pick the one with the smallest distance.
    int form = -1, maxv = 15;

    for (int test = 0; test < 32; test++) {
        const int code = (test << 10) ^ shift(test << 10, 0x537, 15, 5);
        const int dist = bit::cnt(data ^ code);

        if (dist < maxv) {
            maxv = dist;
            form = test;
        } else if (dist == maxv) { // if there is more than one possibility ?
            form = -1;
        }
    }

    return form;
}

//////////////////////////////// golay code ////////////////////////////////////
int gen_golay_code (int version) {
    // golay error code [6 bits ][ 12 bits             ]
    // version string :[version][error correction code]
    // g(x) : x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1 (binary : 1111100100101 integer : 7973 hex : 0x1f25)
    const int data_bits = version << 12;
    return data_bits | shift(data_bits, 0x1f25, 18, 6);
}
int dec_golay_code(int data) {
    int maxv = 18, vers = -1;

    for (int test = 0; test < 64; test++) {
        const int code = (test << 12) ^ shift(test << 12, 0x1f25, 18, 6);
        const int dist = bit::cnt(data ^ code);

        if (dist < maxv) {
            maxv = dist;
            vers = test;
        } else if (dist == maxv) { // if there is more than one possibility ?
            vers = -1;
        }
    }

    return vers;
}

////////////////////////////////// RS code /////////////////////////////////////

polynomial get_bits2 (const std::string &bits, int total) {
    polynomial p(total);

    for (int i = 0; (i < total) && ((i * 8) < bits.size()); i++) {
        p[i] = stoi(bits.substr(i * 8, 8), nullptr, 2);
    }
    return p;
}
polynomial generator2 (int degree) {
    polynomial poly = {1};
    // Generate an irreducible generator polynomial
    for (int i = 0; i < degree; i++) {
        poly = gf256::mul(poly, polynomial({1, gf256::EXP[i]}));
    }

    return poly;
}

polynomial find_err (const polynomial &errpos, int size) { // locate the message error

    polynomial errlist;
    polynomial errpoly = {1};
    polynomial temp = {1};

    // generate the error locator polynomial - Berklekamp-Massey algorithm
    for (int posSynd = 0; posSynd < errpos.size(); posSynd++ ) {
        u8 synterm = errpos[posSynd];
        temp.push_back(0);

        for (int pos = 1; pos < errpoly.size(); pos++) {
            u8 polyterm = errpoly[errpoly.size() - pos - 1];
            polyterm = gf256::mul(polyterm, errpos[posSynd - pos]);
            synterm ^= polyterm;
        }

        if (synterm != 0) {
            if (temp.size() > errpoly.size()) {
                polynomial tNewP = gf256::rescale(temp, synterm);
                temp = gf256::rescale(errpoly, gf256::div(1, synterm));
                errpoly = tNewP;
            }
            polynomial tempValu = gf256::rescale(temp, synterm);
            errpoly = gf256::add(errpoly, tempValu);
        }
    }

    // count the number of errors
    int cnt = errpoly.size() - 1;

    if ((cnt * 2) > errpos.size()) {
        std::cout << "Too many errors to correct";
        return {};
    } else {
        std::cout << "Error count : " << cnt << "\n";
    }

    // calculate the polynomial zeroes
    for (int pos = 0; pos < size; pos++) {
        u8 errZed = gf256::evaluate(errpoly, gf256::EXP[255 - pos]);

        if (errZed == 0) {
            errZed = size - pos - 1;
            errlist.push_back(errZed);
        }
    }

    if (errlist.size() != cnt) {
        std::cout << "\nCould not locate the errors";
        return {};
    } else {
        return errlist;
    }
}
polynomial correct (polynomial blocks, const polynomial &syn, const polynomial &errors) {

    polynomial locator = {1};
    polynomial errLoci;
    // prepare the locator polynomial
    for (auto &err : errors) {
        u8 errTerm = blocks.size() - err - 1;
        errTerm = gf256::EXP[errTerm];
        locator = gf256::mul(locator, {errTerm, 1});
    }
    // error evaluator polynomial
    // errEval = syn[0: errors.size()];
    polynomial eval = {syn.begin(), syn.begin() + errors.size()};
    // eval.reverse();
    std::reverse(eval.begin(), eval.end());
    eval = gf256::mul(eval, locator);

    int tMark = eval.size() - errors.size();
    // eval = eval[tMark: eval.size()];
    eval = {eval.begin() + tMark, eval.end()};

    // the error locator polynomial, minus even terms
    // errLoci = locator[locator.size() % 1 : locator.size() : 2];

    for (int i = 0; i < locator.size(); i += 2) {
        errLoci.push_back(locator[i]);
    }

    for (auto &err : errors) {
        u8 errbyte = err - blocks.size() + 256;
        errbyte = gf256::EXP[errbyte];
        u8 adjbyte = gf256::mul(errbyte, errbyte);
        adjbyte = gf256::evaluate(errLoci, adjbyte);

        u8 errvalu = gf256::evaluate(eval, errbyte);
        u8 msgbyte = gf256::mul(errbyte, adjbyte);

        blocks[err] ^= gf256::div(errvalu, msgbyte);
    }

    return blocks;
}

polynomial rs_encode (const polynomial &data, int degree) {
    if ((data.size() + degree) > 255) return {}; // msg too long : max size 255
    const int dc = data.size() - degree;
    const polynomial gen = generator2(degree);
    polynomial edc = data;
    // cout << show::poly(gen) << "\n";
    for (int i = 0; i < dc; i++) {
        u8 coef = edc[i];
        for (int j = 0; j < gen.size(); j++) {
            edc[i + j] ^= gf256::mul(gen[j], coef);
        }
    }

    for (int i = 0; i < dc; i++) {
        edc[i] = data[i];
    }

    return edc;
}
polynomial rs_decode (const polynomial &data, int degree) {

    // polynomial erased;
    // count the number of erasures
    // for (int i = 0; i < data.size(); i++) {
    //     if (data[i] < 0) {
    //         data[i] = 0;
    //         erased.push_back(i);
    //     }
    // }
    //
    // if (erased.size() > degree) {
    //     std::cout << "\nToo many erasures\n";
    //     return {};
    // }
    // prepare the syndrome polynomial
    const polynomial synd = gf256::syndrome(data, degree);

    if (*max_element(synd.begin(), synd.end()) == 0) {
        std::cout << "\nThe data has no errors\n";
        return data;
    }
    // const polynomial locator = forney(synd, erased, data.size()); // prepare the error locator polynomial
    // const polynomial errors = find_err(locator, data.size()); // locate the message errors
    const polynomial errors = find_err(synd, data.size()); // locate the message errors

    if (errors.size() == 0) {
        std::cout << "\nCould not find any errors\n";
        return {};
    } else {
        std::cout << "\nLocated errors: " << show::simplified(errors) << "\n";
    }

    // for (const auto &it : erased) errors.push_back(it);
    return correct(data, synd, errors);
}

// polynomial getEDC(const std::string &bits, int total) {
//     const int ec = total - bits.size() / 8;
//     const polynomial gen = gf256::generator(ec);
//     polynomial p = gf256::rest(get_bits(bits, total), gen);
//
//     reverse(p.begin(), p.end());
//
//     return p;
// }
