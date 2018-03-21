#pragma once

#include <codecvt>
#include <string>
#include <locale>

// TODO: rewrite this shit!
// http://en.cppreference.com/w/cpp/header/cuchar

namespace utility {
    inline std::string to_utf8(const std::u16string &s)
    {
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv;
        return conv.to_bytes(s);
    }

    inline std::string to_utf8(const std::u32string &s)
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.to_bytes(s);
    }

    inline std::u16string to_utf16(const std::string &s)
    {
        std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
        return convert.from_bytes(s);
    }

    inline std::u32string to_utf32(const std::string &s)
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.from_bytes(s);
    }
} // namespace utility
