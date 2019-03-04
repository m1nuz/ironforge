#pragma once

#include <vector>
#include <string>
#include <memory>
#include <system_error>
#include <optional>
#include <variant>

#include <SDL2/SDL_events.h>
#include <core/common.hpp>
#include <renderer/renderer.hpp>

#include <scene/instance.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace video {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace scene {

    using load_result = std::variant<instance_t, std::error_code>;

    inline auto is_ok(const load_result &res) noexcept -> bool {
        return std::holds_alternative<instance_t>(res);
    }

    auto cleanup_all(std::vector<instance_t> &scenes) -> void;
    [[nodiscard]] auto load(assets::instance_t &asset, video::instance_t &vi, const std::string &path, const bool directly = false) -> load_result;
    auto update(instance_t &sc, const float dt) -> void;
    auto process_event(instance_t &sc, const SDL_Event &ev) -> void;
    auto present(video::instance_t &vi, instance_t &sc, std::unique_ptr<renderer::instance> &render, const float interpolation) -> void;

    auto cache_model(instance_t &sc, const std::string &name, const model_instance &m) -> bool;
    auto cache_material(instance_t &sc, const std::string &name, const material_instance &m) -> bool;

    auto get_model(instance_t &sc, const std::string &name) -> std::optional<model_instance>;
    auto get_material(instance_t &sc, const std::string &name) -> std::optional<material_instance>;
} // namespace scene
