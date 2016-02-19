#pragma once

#include <cstdlib>
#include <cstdint>

#include <iostream>
#include <utility>

namespace application {

    enum class result : int32_t {
        success = 0,
        failure
    };

    enum class log_category : uint32_t { // categories
        application,
        system,
        audio,
        video,
        render,
        input,
        max_category
    };

    enum class log_priority : uint32_t { // priorities
        verbose,
        debug,
        info,
        warning,
        error,
        critical,
        max_priority
    };

    auto init(const std::string& title) -> result;
    auto exec() -> result;
    auto cleanup() -> void;

    extern log_priority categories[static_cast<int>(log_category::max_category)];
    extern const char *category_names[static_cast<int>(log_category::max_category)];

    inline auto output(std::ostream& out, const std::string& format) -> void {
        out << format;
    }

    template<typename Arg, typename... Args>
    inline auto output(std::ostream& out, const std::string& format, Arg&& arg, Args&&... args) -> void {
        for (auto it = format.begin(); it != format.end(); ++it) {
            if (*it == '%') {
                out << std::forward<Arg>(arg);
                output(out, {std::next(it), format.end()}, std::forward<Args>(args)...);
                return;
            }

            out << *it;
        }
    }

    struct _log_init_categories {
        _log_init_categories() {
            for (auto &c : categories)
                c = log_priority::critical;

            categories[static_cast<int>(log_category::application)] = log_priority::info;
        }
    };

    template<typename Arg, typename... Args>
    inline auto error(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cerr << "\033[91m" << "ERROR: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cerr, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cerr << "\033[0m";
    }

    template<typename Arg, typename... Args>
    inline auto warning(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cerr << "\033[94m"  << "WARNING: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cerr, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cerr << "\033[0m";
    }

    template<typename Arg, typename... Args>
    inline auto info(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cout << "\033[0m"  << "INFO: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cout, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cout << "\033[0m";
    }
} // namespace application
