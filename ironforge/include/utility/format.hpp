#pragma once

#include <functional>
#include <string>

namespace utility {

    using std::to_string;

    constexpr inline const std::string& to_string(const std::string &s) {
        return s;
    }

    inline auto format(std::string& message, const std::string& fmt) -> void {
        message += fmt;
    }

    template<typename Arg, typename... Args>
    inline auto format(std::string& message, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
        for (auto it = fmt.begin(); it != fmt.end(); ++it) {
            if (*it == '%') {
                message += to_string(std::forward<Arg>(arg));
                format(message, {std::next(it), fmt.end()}, std::forward<Args>(args)...);

                return;
            }

            message += *it;
        }
    }

} // namespace utility
