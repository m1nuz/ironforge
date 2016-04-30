#pragma once

#include <string>

namespace application {
    auto append_settings(const std::string &name) -> bool;

    auto int_value(const std::string &key, int default_value = 0) -> int32_t;
    auto real_value(const std::string &key, double default_value = 0.0) -> double;
    auto bool_value(const std::string &key, bool default_value = false) -> bool;
    auto text_value(const std::string &key, const std::string &default_value = std::string{}) -> const std::string&;
} // namespace application
