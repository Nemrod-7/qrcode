#include <cstdint>

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
