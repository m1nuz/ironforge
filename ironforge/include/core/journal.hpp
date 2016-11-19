#pragma once

#include <functional>
#include <string>

namespace game {
    namespace journal {
        enum class category : int { // categories
            game,
            system,
            audio,
            video,
            render,
            input,
            scene,
            ui
        };

        enum class priority : int { // priorities
            verbose,
            debug,
            info,
            warning,
            error,
            critical
        };

        using std::to_string;

        inline const std::string& to_string(const std::string &s) {
            return s;
        }

        auto add_storage(std::function<int (const category, const priority, const void *, size_t)> cb) -> void;
        auto write(const category c, const priority p, const std::string &message) -> void;

        inline auto output(std::string& message, const std::string& format) -> void {
            message += format;
        }

        template<typename Arg, typename... Args>
        inline auto output(std::string& message, const std::string& format, Arg&& arg, Args&&... args) -> void {
            for (auto it = format.begin(); it != format.end(); ++it) {
                if (*it == '%') {
                    message += to_string(std::forward<Arg>(arg));
                    output(message, {std::next(it), format.end()}, std::forward<Args>(args)...);

                    return;
                }

                message += *it;
            }
        }

        template<typename Arg, typename... Args>
        inline auto critical(const category c, const std::string& format, Arg&& arg, Args&&... args) -> void {
            std::string message;
            output(message, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(c, priority::critical, message);
        }

        template<typename Arg, typename... Args>
        inline auto error(const category c, const std::string& format, Arg&& arg, Args&&... args) -> void {
            std::string message;
            output(message, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(c, priority::error, message);
        }

        template<typename Arg, typename... Args>
        inline auto warning(const category c, const std::string& format, Arg&& arg, Args&&... args) -> void {
            std::string message;
            output(message, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(c, priority::warning, message);
        }

        template<typename Arg, typename... Args>
        inline auto info(const category c, const std::string& format, Arg&& arg, Args&&... args) -> void {
            std::string message;
            output(message, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(c, priority::info, message);
        }

        template<typename Arg, typename... Args>
        inline auto debug(const category c, const std::string& format, Arg&& arg, Args&&... args) -> void {
            std::string message;
            output(message, format, std::forward<Arg>(arg), std::forward<Args>(args)...);
            write(c, priority::debug, message);
        }        

        auto console_storage(const category c, const priority p, const void *d, size_t s) -> int;
        auto file_storage(const category c, const priority p, const void *d, size_t s) -> int;
    } // namespace journal
}
