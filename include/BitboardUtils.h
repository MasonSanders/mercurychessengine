#ifndef __BITBOARDUTILS_H__
#define __BITBOARDUTILS_H__

#include <cstdint>

inline uint64_t bit(int square) {
    return 1ULL << square;
}

inline bool is_set(uint64_t board, int square) {
    return board & bit(square);
}

inline int file_of(int square) {
    return square % 8;
} 

inline int rank_of(int square) {
    return square / 8;
}

#endif