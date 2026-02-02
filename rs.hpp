#include <algorithm>

#include "gf256.hpp"

int bin2int(const std::string &src) { return stoi(src, nullptr, 2); }
//////////////////////////////////reed-solomon//////////////////////////////////
polynomial get_bits2 (const std::string &bits, int total) {
    polynomial p(total);

    for (int i = 0; i * 8 < bits.size(); i++) {
        p[i] = stoi(bits.substr(i * 8, 8), nullptr, 2);
    }
    return p;
}
polynomial generator2 (int degree) {
    polynomial poly = {1};

    for (int i = 0; i < degree; i++) {
        poly = gf256::mul(poly, polynomial({1, gf256::EXP[i]}));
    }

    return poly;
}

polynomial forney (const polynomial &synd, const polynomial &errpos, int errSize) { // formey algorithm

    polynomial syn = synd;
    u8 termX, termY;

    for (int i = 0; i < errpos.size(); i++) {
        termX = errSize - 1 - errpos[i];
        termX = gf256::EXP[termX];

        for (int j = 0; j < syn.size() - 1; j++) {
            // termY = gf256::mul(syn[j], termX);
            // termY ^= syn[j + 1];
            // termY = termY;
            syn[j] = gf256::mul(syn[j], termX) ^ syn[j + 1];
        }
        syn.pop_back();
    }

    return syn;
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
        // std::cout << "Error count { ", cnt, len(errpos));
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
    polynomial errLoci;

    for (int i = 0; i < locator.size(); i += 2) {
        errLoci.push_back(locator[i]);
    }

    // start correcting
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

polynomial rs_encode (const polynomial &mdc, int ec) {
    const int dc = mdc.size() - ec;
    const polynomial gen = generator2(ec);
    polynomial edc = mdc;
    // cout << show::poly(gen) << "\n";
    for (int i = 0; i < dc; i++) {
        u8 ch = edc[i];
        for (int j = 0; j < gen.size(); j++) {
            edc[i + j] ^= gf256::mul(gen[j], ch);
        }
    }

    for (int i = 0; i < dc; i++) {
        edc[i] = mdc[i];
    }

    return edc;
}
polynomial rs_decode (polynomial blocks, int errSize) {

    polynomial erased;
    // count the number of erasures
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks[i] < 0) {
            blocks[i] = 0;
            erased.push_back(i);
        }
    }

    if (erased.size() > errSize) {
        std::cout << "\nToo many erasures\n";
        return {};
    }
    // prepare the syndrome polynomial
    const polynomial synd = gf256::syndrome(blocks, errSize);

    if (*max_element(synd.begin(), synd.end()) == 0) {
        std::cout << "\nThe message has no errors\n";
        return blocks;
    }

    const polynomial locator = forney(synd, erased, blocks.size()); // prepare the error locator polynomial
    polynomial errlist = find_err(locator, blocks.size()); // locate the message errors

    if (errlist.size() == 0) {
        std::cout << "\nCould not find any errors\n";
        return {};
    } else {
        // cout << "Located errors: ", errlist;
    }

    for (const auto &it : erased) errlist.push_back(it);

    return correct(blocks, synd, errlist);
}

// polynomial get_bits (const std::string &bits, int total) {
//     polynomial p(total);
//
//     for (int i = 0, index = total - 1; i < bits.size(); i += 8, index--) {
//         p[index] = stoi(bits.substr(i, 8), nullptr,2);
//     }
//     return p;
// }
// polynomial getEDC(const std::string &bits, int total) {
//     const int ec = total - bits.size() / 8;
//     const polynomial gen = gf256::generator(ec);
//     polynomial p = gf256::rest(get_bits(bits, total), gen);
//
//     reverse(p.begin(), p.end());
//
//     return p;
// }
