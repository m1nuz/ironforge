#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <optional>
#include <variant>

#include <SDL2/SDL_events.h>
#include <core/common.hpp>
#include <renderer/renderer.hpp>

namespace scene {
    struct instance;
}

#include "../../src/scene/model.hpp"
#include "../../src/scene/material.hpp"
#include "../../src/scene/camera.hpp"
#include "../../src/scene/script.hpp"
#include "../../src/scene/physics.hpp"
#include "../../src/scene/input.hpp"
#include "../../src/scene/transform.hpp"

#include <scene/instance.hpp>

namespace scene {
    using load_result = std::variant<instance_t, std::error_code>;

    inline auto is_ok(const load_result &res) noexcept -> bool {
        return std::holds_alternative<instance_t>(res);
    }

    auto reset_all(instance_t &sc) -> bool;
    auto cleanup_all(std::vector<instance_t> &scenes) -> void;
    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance>;
    [[nodiscard]] auto load(const std::string &path) -> load_result;
    auto update(std::unique_ptr<instance> &s, const float dt) -> void;
    auto update(instance_t &sc, const float dt) -> void;
    auto process_event(std::unique_ptr<instance> &s, const SDL_Event &event) -> void;
    auto process_event(instance_t &sc, const SDL_Event &ev) -> void;
    auto present(std::unique_ptr<instance> &s, std::unique_ptr<renderer::instance> &render, const float interpolation) -> void;
    auto present(video::instance_t &vi, instance_t &sc, std::unique_ptr<renderer::instance> &render, const float interpolation) -> void;

    using json = nlohmann::json;
    auto create_entity(instance_t &sc, const json &info) -> instance_t::index_t;
    auto find_entity(instance_t &sc, const std::string &name) -> instance_t::index_t;

    auto cache_model(instance_t &sc, const std::string &name, const model_instance &m) -> bool;
    auto cache_material(instance_t &sc, const std::string &name, const material_instance &m) -> bool;

    auto get_model(instance_t &sc, const std::string &name) -> std::optional<model_instance>;
    auto get_material(instance_t &sc, const std::string &name) -> std::optional<material_instance>;
} // namespace scene
