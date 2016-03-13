#pragma once

#include <type_traits>
#include <string>

#include <cstdint>
#include <xxhash.h>

namespace utils {
    template <typename T>
    inline std::string to_hex(T value) {
        static_assert(std::is_integral<T>::value, "Not integral");

        static const char* const digits = "0123456789abcdef";

        std::string s;
        s.reserve(16);

        T v = value;
        while (v != 0) {
            s = digits[v % 16] + s;
            v >>= 4;
        }

        return s;
    }

    inline auto xxhash64(const std::string &s, uint64_t seed = 0) -> uint64_t {
        return XXH64(s.c_str(), s.size(), seed);
    }

    inline auto xxhash64(const void *ptr, size_t size, uint64_t seed = 0) -> uint64_t {
        return XXH64(ptr, size, seed);
    }
} // namespace utils
