
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;


int shift (int codewords, int generator, int ec, int dc) {

    for (int i = dc - 1; i >= 0; i--) {
        if (codewords >> (i + (ec - dc)) & 1) {
            codewords ^= generator << i;
            printf("%015b\n", codewords);
        }
    }

    return codewords;
}

// BCH(15,5) parameters
const int N = 15; // Codeword length
const int K = 5;  // Message length

// Perform polynomial division over GF(2)
vector<int> poly_div(const vector<int> &dividend, const vector<int> &divisor) {
    vector<int> remainder = dividend;
    int div_deg = divisor.size() - 1;

    for (size_t i = 0; i <= remainder.size() - divisor.size(); i++) {
        if (remainder[i] == 1) {
            for (size_t j = 0; j < divisor.size(); j++) {
                remainder[i + j] ^= divisor[j]; // XOR for GF(2)
            }
        }
    }
    // Return last div_deg bits as remainder
    return vector<int>(remainder.end() - div_deg, remainder.end());
}

vector<int> bch_encode(const vector<int>& message) {
    if (message.size() != K) {
        throw invalid_argument("Message length must be 5 bits.");
    }

    // Generator polynomial for BCH(15,5) in binary form (degree 10)
    // Represents x^10 + x^8 + x^5 + x^4 + x^2 + x + 1
    const vector<int> generator = {1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1};

    // Append (N-K) zeros for multiplication by x^(N-K)
    vector<int> shifted = message;
    shifted.resize(K + (N - K), 0);

    // Get remainder from division by generator polynomial
    vector<int> remainder = poly_div(shifted, generator);
    vector<int> codeword = message;
    codeword.insert(codeword.end(), remainder.begin(), remainder.end());
    return codeword;
}

int main() {

  printf("\n\n\n");

    try {
        // Example message: 10101
        vector<int> message = {1, 0, 1, 0, 1};
        vector<int> codeword = bch_encode(message);

        cout << "Message: ";
        for (int bit : message) cout << bit;
        cout << "\nEncoded BCH(15,5) codeword: ";
        for (int bit : codeword) cout << bit;
        cout << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
