#pragma once

#include <cstdlib>
#include <cstdint>

#include <iostream>
#include <utility>
#include <thread>
#include <mutex>

#include <ironforge_common.hpp>

namespace application {

    enum class result : int32_t {
        success = 0,
        failure,
        error_init_sdl,
        error_init_ttf,        
        error_init_game,
    };

    enum class log_category : uint32_t { // categories
        application,
        system,
        game,
        audio,
        video,
        render,
        input,
        scene,
        ui,
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

    constexpr auto timestep = 0.002f;

    __must_ckeck auto init(const std::string &title, const std::string &startup_script) -> result;
    __must_ckeck auto exec() -> result;
    auto quit() -> void;
    auto cleanup() -> void;
    auto get_string(result r) -> const char *;

    extern log_priority categories[static_cast<int>(log_category::max_category)];
    extern const char *category_names[static_cast<int>(log_category::max_category)];

    inline auto output(std::ostream& out, const std::string& format) -> void {
        std::mutex out_mutex;
        std::lock_guard<std::mutex> guard(out_mutex);
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

    // CSI colors
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    template<typename Arg, typename... Args>
    inline auto error(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cerr << "\x1b[31;1m" << "ERROR: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cerr, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cerr << "\x1b[0m";
    }

    template<typename Arg, typename... Args>
    inline auto warning(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cerr << "\x1b[33m"  << "WARNING: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cerr, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cerr << "\x1b[0m";
    }

    template<typename Arg, typename... Args>
    inline auto info(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cout << "\x1b[0m"  << "INFO: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cout, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cout << "\x1b[0m";
    }

    template<typename Arg, typename... Args>
    inline auto debug(log_category category, const std::string& format, Arg&& arg, Args&&... args) -> void {
        if (categories[static_cast<int>(category)] < log_priority::info)
            return;

        std::cout << "\x1b[32m"  << "DEBUG: (" << category_names[static_cast<int>(category)] << ") ";
        output(std::cout, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
        std::cout << "\x1b[0m";
    }
} // namespace application
