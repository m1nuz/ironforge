#pragma once

#include <memory>
#include <core/common.hpp>
#include <video/video.hpp>
#include <video/vertices.hpp>
#include <ui/command.hpp>
#include <renderer/phong_model.hpp>

#include <json.hpp>

using json = nlohmann::json;

namespace renderer {
    // TODO: think about static dispatching

    ///
    /// Renderer interface
    ///
    struct instance {
        // virtual auto is_phong_spported() -> bool = 0;
        virtual auto append(const phong::ambient_light &light) -> void = 0;
        virtual auto append(const phong::directional_light &light) -> void = 0;
        virtual auto append(const phong::point_light &light) -> void = 0;
        virtual auto append(const phong::material &material) -> void = 0;
        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw) -> void = 0;
        virtual auto append(const glm::mat4 &model) -> void = 0;
        virtual auto append(const video::texture &cubemap, uint32_t flags) -> void = 0;
        virtual auto append(int32_t font, const std::string &text, const glm::vec2 &pos, const glm::vec4 &color) -> void = 0;
        // TODO: think about add/remove from ubo mechanism

        virtual auto reset() -> void = 0;
        virtual auto present(video::instance_t &in, const glm::mat4 &proj, const glm::mat4 &view) -> void = 0;

        virtual auto dispath(const ui::command &c) -> void = 0;

        virtual ~instance() = default;
    };

    ///
    /// Create renderer factory method
    ///
    [[nodiscard]] auto create_renderer(std::string_view type, video::instance_t &in, const json &info) -> std::unique_ptr<instance>;
} // namespace renderer
