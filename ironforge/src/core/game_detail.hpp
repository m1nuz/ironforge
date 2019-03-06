#pragma once

#include <core/assets.hpp>
#include <core/json.hpp>

namespace game {

    namespace detail {

        //auto read_ui_styles( assets::instance_t& asset, json ui_json) -> std::unordered_map<std::string, imui::box_style>;

        ///
        /// \brief Returns base application path.
        /// Base path mean where executable file located.
        ///
        auto get_base_path() noexcept -> const std::string&;

        ///
        /// \brief Returns prefered application path.
        /// Prefered path mean where application could write data.
        ///
        auto get_pref_path() noexcept -> const std::string&;

    } // namespace detail

} // namespace game
