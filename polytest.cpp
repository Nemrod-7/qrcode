#include <iostream>
#include <vector>

using namespace std;

using u8 = unsigned char;
using polynomial = std::vector<u8>;


void trim (vector<int>& poly) {
    while (!poly.empty() && abs(poly[0]) < 1e-9)
        poly.erase(poly.begin());
}

vector<int> mul (const vector<int> &a, const vector<int> &b) {
    vector<int> poly (a.size() + b.size() - 1);

    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < b.size(); j++) {
            poly[i+j] += a[i] * b[j];
        }
    }

    return poly;
}
vector<int> add (const vector<int> &a, const vector<int> &b) {
    int size = max(a.size(), b.size());
    vector<int> sum(size);

    for (int i = a.size() - 1, j = size - 1; i >= 0; i--, j--) {
        sum[j] += a[i];
    }

    for (int i = b.size() - 1, j = size - 1; i >= 0; i--, j--) {
        sum[j] += b[i];
    }

    return sum;
}
vector<int> sub (const vector<int> &a, const vector<int> &b) {
    const int size = std::max(a.size(), b.size());
    vector<int> sum(size);

    for (int i = a.size() - 1, j = size - 1; i >= 0; i--, j--) {
        sum[j] += a[i];
    }

    for (int i = b.size() - 1, j = size - 1; i >= 0; i--, j--) {
        sum[j] -= b[i];
    }

    return sum;
}

pair<vector<int>, vector<int>> divide (const std::vector<int> &dividend, const std::vector<int> &divisor) {

    // int asize = dividend.size() - 1, bsize = divisor.size() - 1;
    const int size = (dividend.size() - divisor.size() + 1);
    std::vector<int> quotient(size, 0), remain = dividend;

    for (int i = 0; i < size; ++i) {
        int coef = remain[i] / divisor[0];
        quotient[i] = coef;

        for (int j = 0; j < divisor.size(); ++j) {
            remain[i + j] -= coef * divisor[j];
        }
    }

    trim(remain);
    return {quotient, remain};
}

string show (const vector<int> &p) {
    string os;

    for (int i = 0; i < p.size(); i++) {
        if (p[i] == 0) continue;

        int exp = p.size() - i - 1;
        if (os.size()) os += (p[i] > 0 ? " + " : " - ");
        os += to_string(abs(p[i]));
        if (exp > 0) os += "x";
        if (exp > 1) os += "^" + to_string(exp);
    }

    return os;
}

int main () {

      // (x^5 - x^4 + x^2 + 2) / (x^2 + 1)

      vector<int> a = {1,-1,0,1,0,2}, b = {1,0,1};
      auto [diva, rema] = divide(a,b);

      cout << "\n\n";

      cout << show(diva) << "\n";
      cout << show(rema) << "\n";
}
