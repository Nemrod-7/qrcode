#include <iostream>
#include <vector>

using namespace std;

using u8 = char;
using polynomial = vector<u8>;

string show (const polynomial &p) {
    string os;

    for (int i = p.size() - 1; i >= 0; i--) {
        int coef = p[i];

        if (coef != 0) {
            if (os.size() || coef < 0) os += coef > 0 ? " + " : " - ";
            if (i == 0 || coef < -1 || coef > 1 ) os += to_string(abs(coef));
            if (i > 0) os += "x";
            if (i > 1) os += "^" + to_string(i);
        }
    }

    return os;
}

polynomial make(int num, const int x) {
    polynomial p(8);

    while (num > 0) {

        if (num < x) {
            p[0] = num;
            num = 0;
        } else {
            int ex = x, pow = 1;

            while (ex * x <= num) {
                ex *= x;
                pow++;
            }

            p[pow]++;
            num -= ex;
        }
    }

    return p;
}

polynomial add (const polynomial &a, const polynomial &b) {
    if (a.size() < b.size()) return add(b,a);
    polynomial c = a;

    for (int i = 0; i < b.size(); i++) c[i] += b[i];

    return c;
}
polynomial sub (const polynomial &a, const polynomial &b) {
    polynomial c(std::max(a.size(), b.size()));

    for (int i = 0; i < a.size(); i++) c[i] = a[i];

    for (int i = 0; i < b.size(); i++) c[i] -= b[i];

    return c;
}
polynomial mul (const polynomial &a, const polynomial &b) {
    int size = a.size() + b.size() - 1;
    polynomial c(size);

    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < b.size(); j++) {
            c[i + j] += (a[i] * b[i]);
        }
    }

    return c;
}
pair<polynomial, polynomial> divide (const polynomial &dividend, const polynomial &divisor) {

    const int size = dividend.size() - divisor.size()  + 1;
    const int start = dividend.size() - divisor.size() - 1;
    polynomial quo(size), rem = dividend;

    for (int i = rem.size() - 1; i >= start; i--) {
        int coef = rem[i] / divisor.back();
        quo[i - start] = coef;

        for (int j = divisor.size() - 1; j >= 0; j--) {
            rem[i + j - start] -= coef * divisor[j];
        }
    }

    while (rem.back() == 0) rem.pop_back();
    return {quo, rem};
}




int main () {


    polynomial a = {5,7,4,4}, b = {1,2};
    auto [diva, rema] = divide(a,b);

    cout << "\n\n";
    cout << show (a) << "\n";
    cout << show (b) << "\n";
    cout << "---------------------\n";
    cout << show(diva) << "\n";
    cout << show(rema) << "\n";

}
