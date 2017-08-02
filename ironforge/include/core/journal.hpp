#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace game {
    namespace journal {
        enum class verbosity : int { // verbosity
            verbose,
            debug,
            info,
            warning,
            error,
            critical
        };

        using std::to_string;

        constexpr char _GAME[] = "game";
        constexpr char _SYSTEM[] = "system";
        constexpr char _AUDIO[] = "audio";
        constexpr char _VIDEO[] = "video";
        constexpr char _RENDER[] = "render";
        constexpr char _INPUT[] = "input";
        constexpr char _SCENE[] = "scene";
        constexpr char _UI[] = "ui";

        constexpr inline const std::string& to_string(const std::string &s) {
            return s;
        }

        auto add_tag(const std::string &tag, const verbosity verbosity_level) -> void;
        auto add_storage(std::function<int (const std::string &, const verbosity, const void *, size_t)> cb) -> void;
        auto add_storage(const std::string &tag, std::function<int (const std::string &, const verbosity, const void *, size_t)> cb) -> void;
        auto write(const std::string &tag, const verbosity v, const std::string &message) -> void;

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

        template<typename Arg, typename... Args>
        inline auto critical(const std::string &tag, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            std::string message;
            format(message, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(tag, verbosity::critical, message);
        }

        template<typename Arg, typename... Args>
        inline auto error(const std::string &tag, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            std::string message;
            format(message, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(tag, verbosity::error, message);
        }

        template<typename Arg, typename... Args>
        inline auto warning(const std::string &tag, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            std::string message;
            format(message, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(tag, verbosity::warning, message);
        }

        template<typename Arg, typename... Args>
        inline auto info(const std::string &tag, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            std::string message;
            format(message, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(tag, verbosity::info, message);
        }

        template<typename Arg, typename... Args>
        inline auto debug(const std::string &tag, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            std::string message;
            format(message, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(tag, verbosity::debug, message);
        }

        template<typename Arg, typename... Args>
        inline auto verbose(const std::string &tag, const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            std::string message;
            format(message, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(tag, verbosity::verbose, message);
        }

        auto setup_default(const std::string &log_path) -> void;

        auto console_storage(const std::string &tag, const verbosity v, const void *d, size_t s) -> int;
        auto single_file_storage(const std::string &path, const std::string &tag, const verbosity v, const void *d, size_t s) -> int;
        auto multi_file_storage(const std::unordered_map<std::string, std::string> &files, const std::string &tag, const verbosity v, const void *d, size_t s) -> int;
    } // namespace journal
}