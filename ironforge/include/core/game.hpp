#pragma once

#include <cstdint>
#include <variant>
#include <system_error>
#include <string_view>

#include <core/errors.hpp>
#include <core/common.hpp>
#include <core/journal.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include <core/assets.hpp>
#include <ui/ui.hpp>

namespace game {
    constexpr auto timestep = 0.002f;

    ///
    /// \brief Application instance
    ///
    typedef struct instance_type {
        using scene_type = scene::instance_t;
        using scenes_type = std::vector<scene_type>;

        instance_type() = default;

        auto& empty_scene() {
            static scene_type _empty;
            return _empty;
        }

        auto& current_scene() {
            if (scenes.empty())
                return empty_scene();

            if (current_scene_index < scenes.size())
                return scenes[current_scene_index];

            return empty_scene();
        }

        scenes_type                                     scenes;
        scenes_type::size_type                          current_scene_index = 0;
        std::unique_ptr<renderer::instance>             render;
        video::instance_t                               vi;
        ui::context                                     uic;
        imui::context_t                                 imui;
        assets::instance_t                              asset_instance;
        uint64_t                                        current_time = 0ull;
        uint64_t                                        last_time = 0ull;
        uint64_t                                        timesteps = 0ull;
        float                                           delta_accumulator = 0.0f;
        bool                                            running = true;
    } instance_t;

    using game_result = std::variant<instance_t, std::error_code>;

    ///
    /// \brief Exit from application
    ///
    auto quit() -> void;

    ///
    /// \brief Create application instance
    /// \param[in] conf_path Configuration file path
    /// \param[in] fullpath_only If true, get config by absolute path, else from base directory
    /// \return
    ///
    [[nodiscard]] auto create(std::string_view conf_path, const bool fullpath_only = false) -> game_result;

    [[nodiscard]] inline bool is_ok(const game_result& res) {
        return std::holds_alternative<instance_t>(res);
    }

    [[nodiscard]] inline auto what(const game_result& res) -> std::string {
        using namespace std;

        if (holds_alternative<error_code>(res))
            return "(" + string{get<error_code>(res).category().name()} + ") " + get<error_code>(res).message();

        return "Nothing";
    }

    ///
    /// \brief Launch application
    /// \param[in] res
    /// \return system exit code
    ///
    [[nodiscard]] auto launch(instance_t &app) -> int;
} // namespace game
