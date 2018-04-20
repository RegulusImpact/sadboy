#ifndef Utils_H
#define Utils_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

namespace Utils {
    static const uint8_t BIT_0 = (1 << 0);
    static const uint8_t BIT_1 = (1 << 1);
    static const uint8_t BIT_2 = (1 << 2);
    static const uint8_t BIT_3 = (1 << 3);
    static const uint8_t BIT_4 = (1 << 4);
    static const uint8_t BIT_5 = (1 << 5);
    static const uint8_t BIT_6 = (1 << 6);
    static const uint8_t BIT_7 = (1 << 7);

    inline uint8_t ParseCharsToHex(const char h, const char l) {
        uint8_t hi = h > '9' ? h - 'A' + 10 : h - '0';
        uint8_t lo = l > '9' ? l - 'A' + 10 : l - '0';

        uint8_t out= (hi << 4 ) | lo;

        return out;
    }

    inline int32_t TwosComplement(uint32_t x) {
        return static_cast<int32_t>(x);
    }
}
 // namespace utils

#endif
