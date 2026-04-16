#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

using u8 = uint8_t;
using polynomial = std::vector<u8>;
const u8 field = 0xff; // integer : 255

namespace show {
    std::string simplified (const polynomial &rem) {
        std::string os;
        for (int i = 0; i < rem.size(); i++) {
            std::string num = std::to_string(rem[i]);
            os += std::string(3 - num.size(), ' ') + num + " ";
        }
        return os;
    }

    std::string poly (const polynomial &p) {
        std::string os;

        for (int i = 0; i < p.size(); i++) {
            u8 val = p[i], ex = p.size() - i - 1;

            if (val != 0) {
                if (os.size() || val < 0) os += val > 0 ? " + " : " - ";
                os += std::to_string(abs(val));
                if (ex > 0) os += "x";
                if (ex > 1) os += "^" + std::to_string(ex);
            }
        }
        return os;
    }
    std::string bits (const std::string &bits) {
        std::string os;
        for (int i = 0; i < bits.size(); i += 8) {
            // cout << bitset<8>(bits.substr(i, 8)) << " ";
            os += std::to_string(stoi(bits.substr(i, 8), nullptr,2)) + " ";
        }
        return os;
    }
};

namespace gf256 {
    const unsigned char LOG[256] = {
        0xff, 0x00, 0x01, 0x19, 0x02, 0x32, 0x1a, 0xc6, 0x03, 0xdf, 0x33, 0xee, 0x1b, 0x68, 0xc7, 0x4b,
        0x04, 0x64, 0xe0, 0x0e, 0x34, 0x8d, 0xef, 0x81, 0x1c, 0xc1, 0x69, 0xf8, 0xc8, 0x08, 0x4c, 0x71,
        0x05, 0x8a, 0x65, 0x2f, 0xe1, 0x24, 0x0f, 0x21, 0x35, 0x93, 0x8e, 0xda, 0xf0, 0x12, 0x82, 0x45,
        0x1d, 0xb5, 0xc2, 0x7d, 0x6a, 0x27, 0xf9, 0xb9, 0xc9, 0x9a, 0x09, 0x78, 0x4d, 0xe4, 0x72, 0xa6,
        0x06, 0xbf, 0x8b, 0x62, 0x66, 0xdd, 0x30, 0xfd, 0xe2, 0x98, 0x25, 0xb3, 0x10, 0x91, 0x22, 0x88,
        0x36, 0xd0, 0x94, 0xce, 0x8f, 0x96, 0xdb, 0xbd, 0xf1, 0xd2, 0x13, 0x5c, 0x83, 0x38, 0x46, 0x40,
        0x1e, 0x42, 0xb6, 0xa3, 0xc3, 0x48, 0x7e, 0x6e, 0x6b, 0x3a, 0x28, 0x54, 0xfa, 0x85, 0xba, 0x3d,
        0xca, 0x5e, 0x9b, 0x9f, 0x0a, 0x15, 0x79, 0x2b, 0x4e, 0xd4, 0xe5, 0xac, 0x73, 0xf3, 0xa7, 0x57,
        0x07, 0x70, 0xc0, 0xf7, 0x8c, 0x80, 0x63, 0x0d, 0x67, 0x4a, 0xde, 0xed, 0x31, 0xc5, 0xfe, 0x18,
        0xe3, 0xa5, 0x99, 0x77, 0x26, 0xb8, 0xb4, 0x7c, 0x11, 0x44, 0x92, 0xd9, 0x23, 0x20, 0x89, 0x2e,
        0x37, 0x3f, 0xd1, 0x5b, 0x95, 0xbc, 0xcf, 0xcd, 0x90, 0x87, 0x97, 0xb2, 0xdc, 0xfc, 0xbe, 0x61,
        0xf2, 0x56, 0xd3, 0xab, 0x14, 0x2a, 0x5d, 0x9e, 0x84, 0x3c, 0x39, 0x53, 0x47, 0x6d, 0x41, 0xa2,
        0x1f, 0x2d, 0x43, 0xd8, 0xb7, 0x7b, 0xa4, 0x76, 0xc4, 0x17, 0x49, 0xec, 0x7f, 0x0c, 0x6f, 0xf6,
        0x6c, 0xa1, 0x3b, 0x52, 0x29, 0x9d, 0x55, 0xaa, 0xfb, 0x60, 0x86, 0xb1, 0xbb, 0xcc, 0x3e, 0x5a,
        0xcb, 0x59, 0x5f, 0xb0, 0x9c, 0xa9, 0xa0, 0x51, 0x0b, 0xf5, 0x16, 0xeb, 0x7a, 0x75, 0x2c, 0xd7,
        0x4f, 0xae, 0xd5, 0xe9, 0xe6, 0xe7, 0xad, 0xe8, 0x74, 0xd6, 0xf4, 0xea, 0xa8, 0x50, 0x58, 0xaf
    };
    // The obvious implementation of gf256::mul is EXP[(LOG[x] + LOG[y]) % 255],
    // but if we double the exp array, so that it is 512 elements long,
    // we can drop the relatively expensive % 255 of the calculation.
    const unsigned char EXP[512] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26,
        0x4c, 0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9, 0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0,
        0x9d, 0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35, 0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23,
        0x46, 0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0, 0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1,
        0x5f, 0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc, 0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0,
        0xfd, 0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f, 0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2,
        0xd9, 0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce,
        0x81, 0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93, 0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc,
        0x85, 0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9, 0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54,
        0xa8, 0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa, 0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73,
        0xe6, 0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e, 0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff,
        0xe3, 0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4, 0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41,
        0x82, 0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6,
        0x51, 0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef, 0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09,
        0x12, 0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5, 0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16,
        0x2c, 0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83, 0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x01,

        0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26, 0x4c,
        0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9, 0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x9d,
        0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35, 0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23, 0x46,
        0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0, 0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1, 0x5f,
        0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc, 0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0xfd,
        0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f, 0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2, 0xd9,
        0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce, 0x81,
        0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93, 0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc, 0x85,
        0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9, 0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54, 0xa8,
        0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa, 0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73, 0xe6,
        0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e, 0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff, 0xe3,
        0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4, 0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41, 0x82,
        0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6, 0x51,
        0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef, 0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09, 0x12,
        0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5, 0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16, 0x2c,
        0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83, 0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x01, 0x01
    };

    inline u8 add (u8 a, u8 b) { return a ^ b; }
    inline u8 sub (u8 a, u8 b) { return a ^ b; }
    inline u8 mul (u8 a, u8 b) { return a && b ? EXP[LOG[a] + LOG[b]] : 0; }
    inline u8 div (u8 a, u8 b) { return EXP[(LOG[a] + LOG[b] * 254) % 255]; }

    u8 evaluate (const polynomial &p, u8 arg) {
        // Evaluates a polynomial in GF(2^p) given the value for x. This is based on Horner's scheme for maximum efficiency.
        // p : polynomial argument, arg : independant variable, val : dependent value
        u8 val = p[0];

        for (int i = 1; i < p.size(); i++) {
            val = gf256::mul(val, arg) ^ p[i];
        }

        return val;
    }

    polynomial add (const polynomial &a, const polynomial &b) {
        const int size = std::max(a.size(), b.size());
        const int ov = size - b.size();
        polynomial res (size);

        for (int i = 0; i < a.size(); i++) {
            res[i + size - a.size()] = a[i];
        }

        for (int i = 0; i < b.size(); i++) {
            res[i + ov] = add(res[i + ov], b[i]);
        }

        return res;
    }
    polynomial mul (const polynomial &p1, const polynomial &p2) {
        polynomial poly (p1.size() + p2.size() - 1);

        for (u8 i = 0; i < p1.size(); i++) {
            for (u8 j = 0; j < p2.size(); j++) {
                poly[i+j] ^= mul(p1[i],p2[j]);
            }
        }
        return poly;
    }

    polynomial mul_nxt (const polynomial &p1, const polynomial &p2) {
        // '''Multiply two polynomials, inside Galois Field (but the procedure is generic). Optimized function by precomputation of log.'''
        polynomial poly (p1.size() + p2.size() - 1);
        std::vector<int> log;

        for (int i = 0; i < p1.size(); i++) {
            log.push_back( LOG[p1[i]] );
        }

        for (u8 j = 0; j < p2.size(); j++) {
            int qj = p2[j];

            if (qj != 0) {
                int lq = LOG[qj];

                for (u8 i = 0; i < p1.size(); i++) {
                    if (p1[i] != 0) {
                        poly[i + j] ^= EXP[log[i] + lq];
                    }
                }
            }
        }
        return poly;
    }

    polynomial rescale (const polynomial &p, u8 z) { // polynomial scaling by z
        polynomial res (p.size());

        for (int i = 0; i < p.size(); i++) {
            res[i] = gf256::mul(p[i], z);
        }

        return res;
    }
    polynomial syndrome(const polynomial &p, int size) { // generate the syndrome polynomial
        polynomial syn (size);

        for (int i = 0; i < size; i++) {
            syn[i] = gf256::evaluate(p, gf256::EXP[i]);
        }

        return syn;
    }
};

void generate_table(const int mod = 0x11d) {

    for (int i = 0, exp = 1; i < 256; i++) {
        if (exp > 255) exp ^= mod ;
        // LOG[exp] = i;
        // EXP[i] = EXP[i + 255] = exp;
        // printf("%i %i\n", EXP[i], exp);
        exp <<= 1;
    }
}

int gf_mult_noLUT (int x, int y, int prim = 0, int field = 256, bool carryless = true) {
    // Galois Field integer multiplication using Russian Peasant Multiplication algorithm (faster than the standard multiplication + modular reduction).
    // If prim is 0 and carryless == False, then the function produces the result for a standard integers multiplication (no carry-less arithmetics nor modular reduction).
    int r = 0;

    while (y != 0) {
        if (y & 1) {
            r = carryless ? (r ^ x) : (r + x) ;
        }
        y >>= 1;
        x <<= 1;
        if ((prim > 0) && (x & field)) {
            x ^= prim;
        }
    }
    return r;
}

// The QR code specification says to use byte-wise modulo 100011101 arithmetic (integer : 285 hex : 0x11d).
std::pair<std::vector<int>, std::vector<int>> init_tables(int prim = 0x11d, int alpha = 2) {
    // Precompute the logarithm and anti-log tables for faster computation later, using the provided primitive polynomial.
    // These tables are used for multiplication/division since addition/substraction are simple XOR operations inside GF of characteristic 2.
    // The basic idea is quite simple: since b**(log_b(x), log_b(y)) == x * y given any number b (the base or generator of the logarithm), then we can use any number b to precompute logarithm and anti-log (exponentiation) tables to use for multiplying two numbers x and y.
    // That's why when we use a different base/generator number, the log and anti-log tables are drastically different, but the resulting computations are the same given any such tables.
    // For more infos, see https://en.wikipedia.org/wiki/Finite_field_arithmetic#Implementation_tricks
    //
    // 'generator' is the generator number (the "increment" that will be used to walk through the field by multiplication, this must be a prime number). This is basically the base of the logarithm/anti-log tables. Also often noted "alpha" in academic books.
    // 'prim' is the primitive/prime (binary) polynomial and must be irreducible (ie, it can't represented as the product of two smaller polynomials).
    // It's a polynomial in the binary sense: each bit is a coefficient, but in fact it's an integer between field_charac+1 and field_charac*2, and not a list of gf values.
    // The prime polynomial will be used to reduce the overflows back into the range of the Galois Field without duplicating values (all values should be unique).
    // See the function find_prime_polys() and: http://research.swtch.com/field and http://www.pclviewer.com/rs2/galois.html
    // note that the choice of generator or prime polynomial doesn't matter very much: any two finite fields of size p^n have identical structure,
    // even if they give the individual elements different names (ie, the coefficients of the codeword will be different, but the final result will be the same:
    // you can always correct as many errors/erasures with any choice for those parameters).
    // That's why it makes sense to refer to all the finite fields, and all decoders based on Reed-Solomon, of size p^n as one concept: GF(p^n).
    // It can however impact sensibly the speed (because some parameters will generate sparser tables).
    // 'c_exp' is the exponent for the field's characteristic GF(2^c_exp)

    std::vector<int> gf_exp (field * 2); // # anti-log (exponential) table. The first two elements will always be [GF256int(1), generator]
    std::vector<int> gf_log (field + 1); // # log table, log[0] is impossible and thus unused

    // # For each possible value in the galois field 2^8, we will pre-compute the logarithm and anti-logarithm (exponential) of this value
    // # To do that, we generate the Galois Field F(2^p) by building a list starting with the element 0 followed by the (p-1) successive powers of the generator a : 1, a, a^1, a^2, ..., a^(p-1).
    int x = 1;

    for (int i = 0; i < field; i++) {
      // we could skip index 255 which is equal to index 0 because of modulo: g^255==g^0 but either way, this does not change the later outputs (ie, the ecc symbols will be the same either way)
          gf_exp[i] = x;
          gf_log[x] = i;
          gf_exp[i + field] = x;
          x = gf_mult_noLUT(x, alpha, prim, field + 1);
          //     # If you use only generator==2 or a power of 2, you can use the following which is faster than gf_mult_noLUT():
          //     x <<= 1; # multiply by 2 (change 1 by another number y to multiply by a power of 2^y)
          //     if x & 0x100: # similar to x >= 256, but a lot faster (because 0x100 == 256)
          //         x ^= prim # substract the primary polynomial to the current value (instead of 255, so that we get a unique set made of coprime numbers), this is the core of the tables generation
    }

    return {gf_log, gf_exp};
}

// def rwh_primes1(n):
//     # http://stackoverflow.com/questions/2068372/fastest-way-to-list-all-primes-below-n-in-python/3035188#3035188
//     ''' Returns  a list of primes < n '''
//     sieve = [True] * (n/2)
//     for i in xrange(3,int(n**0.5)+1,2):
//         if sieve[i/2]:
//             sieve[i*i/2::i] = [False] * ((n-i*i-1)/(2*i)+1)
//     return [2] + [2*i+1 for i in xrange(1,n/2) if sieve[i]]

std::vector<int> find_prime_polys(int alpha = 2, int c_exp = 8, bool fast_primes = false, bool single = false) {
//     Compute the list of prime polynomials for the given generator and galois field characteristic exponent.
//     fast_primes will output less results but will be significantly faster.
//     single will output the first prime polynomial found, so if all you want is to just find one prime polynomial to generate the LUT for Reed-Solomon to work, then just use that.
//
//     A prime polynomial (necessarily irreducible) is necessary to reduce the multiplications in the Galois Field, so as to avoid overflows.
//     Why do we need a "prime polynomial"? Can't we just reduce modulo 255 (for GF(2^8) for example)? Because we need the values to be unique.
//     For example: if the generator (alpha) = 2 and c_exp = 8 (GF(2^8) == GF(256)), then the generated Galois Field (0, 1, a, a^1, a^2, ..., a^(p-1)) will be galois field it becomes 0, 1, 2, 4, 8, 16, etc. However, upon reaching 128, the next value will be doubled (ie, next power of 2), which will give 256. Then we must reduce, because we have overflowed above the maximum value of 255. But, if we modulo 255, this will generate 256 == 1. Then 2, 4, 8, 16, etc. giving us a repeating pattern of numbers. This is very bad, as it's then not anymore a bijection (ie, a non-zero value doesn't have a unique index). That's why we can't just modulo 255, but we need another number above 255, which is called the prime polynomial.
//     Why so much hassle? Because we are using precomputed look-up tables for multiplication: instead of multiplying a*b, we precompute alpha^a, alpha^b and alpha^(a+b), so that we can just use our lookup table at alpha^(a+b) and get our result. But just like in our original field we had 0,1,2,...,p-1 distinct unique values, in our "LUT" field using alpha we must have unique distinct values (we don't care that they are different from the original field as long as they are unique and distinct). That's why we need to avoid duplicated values, and to avoid duplicated values we need to use a prime irreducible polynomial.
//
//     Here is implemented a bruteforce approach to find all these prime polynomials, by generating every possible prime polynomials (ie, every integers between field_charac+1 and field_charac*2), and then we build the whole Galois Field, and we reject the candidate prime polynomial if it duplicates even one value or if it generates a value above field_charac (ie, cause an overflow).
//     Note that this algorithm is slow if the field is too big (above 12), because it's an exhaustive search algorithm. There are probabilistic approaches, and almost surely prime approaches, but there is no determistic polynomial time algorithm to find irreducible monic polynomials. More info can be found at: http://people.mpi-inf.mpg.de/~csaha/lectures/lec9.pdf
//     Another faster algorithm may be found at Adleman, Leonard M., and Hendrik W. Lenstra. "Finding irreducible polynomials over finite fields." Proceedings of the eighteenth annual ACM symposium on Theory of computing. ACM, 1986.
//
//     Prepare the finite field characteristic (2^p - 1), this also represent the maximum possible value in this field
    const int root = 2; //# we're in GF(2)
    const int field = (2 << (c_exp - 1)) - 1;
    const int field_nxt = (field << 1) + 1;
    std::vector<int> prim_candidates;
    std::vector<int> correct_primes ;

    if (fast_primes) {
//         prim_candidates = rwh_primes1(field_charac_next); // generate maybe prime polynomials and check later if they really are irreducible
//         prim_candidates = [x for x in prim_candidates if x > field_charac]; // filter out too small primes
    } else { // try each possible prime polynomial, but skip even numbers (because divisible by 2 so necessarily not irreducible)
            for (int i = field + 2; i < field_nxt; i += root) {
                prim_candidates.push_back(i);
            }
    }

    for (auto &prim : prim_candidates) {
        std::vector<bool> seen (field + 1); // memory variable to indicate if a value was already generated in the field (value at index x is set to 1) or not (set to 0 by default)
        bool conflict = false; // flag to know if there was at least one conflict
        // Second loop, build the whole Galois Field
        int x = 1;

        for (int i = 0; i < field; i++) {
            // Compute the next value in the field (ie, the next power of alpha/generator)
            x = gf_mult_noLUT(x, alpha, prim, field + 1);

            // if the value overflowed (above field) or is a duplicate of a previously generated power of alpha, then we reject this polynomial (not prime)
            if (x > field || seen[x] == 1) {
                conflict = true;
                break;
            } else { // else we flag this value as seen (to maybe detect future duplicates), and we continue onto the next power of alpha
                seen[x] = 1;
            }
        }
        // End of the second loop: if there's no conflict (no overflow nor duplicated value), this is a prime polynomial!
        if (!conflict) {
            correct_primes.push_back(prim);
            if (single) return {prim};
        }
    }

    return correct_primes;
}

int main () {
    std::vector<int> poly = find_prime_polys(2,8,false,true);

    for (auto p : poly) {
        printf("%i\n",p);
    }





}
