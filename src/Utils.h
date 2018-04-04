#ifndef Utils_H
#define Utils_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

namespace Utils {
    inline uint8_t ParseCharsToHex(const char h, const char l) {
        uint8_t hi = h > '9' ? h - 'A' + 10 : h - '0';
        uint8_t lo = l > '9' ? l - 'A' + 10 : l - '0';
    
        uint8_t out= (hi << 4 ) | lo;
        
        return out;
    }
}
 // namespace utils

#endif