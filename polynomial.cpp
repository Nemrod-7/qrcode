#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

using u8 = uint8_t;

class polynomial {
    public :
        std::vector<u8> m_data;

        polynomial(int num) : m_data(num) { }

        void operator = (const std::vector<u8> &p) { m_data = p; }

        std::string show () {
            std::string os;

            for (int i = m_data.size() - 1; i >= 0; i--) {
                int coef = m_data[i];

                if (coef != 0) {
                    if (os.size() || coef < 0) os += coef > 0 ? " + " : " - ";
                    if (i == 0 || coef < -1 || coef > 1 ) os += std::to_string(std::abs(coef));
                    if (i > 0) os += "x";
                    if (i > 1) os += "^" + std::to_string(i);
                }
            }

            return os;
        }
};

int main() {

}
