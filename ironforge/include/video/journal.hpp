#pragma once

#include <core/journal.hpp>

namespace video {

    namespace journal {

        constexpr char VIDEO_TAG[] = "video";

        template<typename Arg, typename... Args>
        inline auto critical(const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            game::journal::critical(VIDEO_TAG, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
        }

        template<typename Arg, typename... Args>
        inline auto error(const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            game::journal::error(VIDEO_TAG, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
        }

        template<typename Arg, typename... Args>
        inline auto warning(const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            game::journal::warning(VIDEO_TAG, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
        }

        template<typename Arg, typename... Args>
        inline auto info(const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            game::journal::info(VIDEO_TAG, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
        }

        template<typename Arg, typename... Args>
        inline auto debug(const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            game::journal::debug(VIDEO_TAG, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
        }

        template<typename Arg, typename... Args>
        inline auto verbose(const std::string& fmt, Arg&& arg, Args&&... args) -> void {
            game::journal::verbose(VIDEO_TAG, fmt, std::forward<Arg>(arg), std::forward<Args>(args)...);
        }

    } // namespace journal

} // namespace video
