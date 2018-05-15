#pragma once

#include <memory>

#include <core/common.hpp>
#include <core/json.hpp>
#include <video/video.hpp>
#include <video/vertices.hpp>
#include <renderer/phong_model.hpp>
#include <ui/types.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

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
        // TODO: think about add/remove from ubo mechanism

        virtual auto reset() -> void = 0;
        virtual auto present(video::instance_t &in, const glm::mat4 &proj, const glm::mat4 &view) -> void = 0;

        virtual auto dispath(video::instance_t &vi, const ui::draw_command_t &c) -> void = 0;

        virtual ~instance() = default;
    };

    ///
    /// Create renderer factory method
    ///
    [[nodiscard]] auto create_renderer(std::string_view type, video::instance_t &vi, const json &info) -> std::unique_ptr<instance>;
} // namespace renderer
