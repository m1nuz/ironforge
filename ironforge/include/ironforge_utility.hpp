#pragma once

#include <type_traits>
#include <string>

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
