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

    enum RENDERER_FLAGS {
        SKYBOX_TEXTURE_BIT      = 1 << 0,
        RENDER_TO_TEXTURE_BIT   = 1 << 1,
    };

    // TODO: think about static dispatching

    ///
    /// Renderer interface
    ///
    struct instance {
        virtual ~instance() = default;

        virtual auto append(const phong::ambient_light &light) -> void = 0;
        virtual auto append(const phong::directional_light &light) -> void = 0;
        virtual auto append(const phong::point_light &light) -> void = 0;
        virtual auto append(const phong::material &material) -> void = 0;
        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw, const glm::mat4 &model) -> void = 0;
        virtual auto append(const video::texture &tex, const uint32_t flags) -> void = 0;
        // TODO: think about add/remove from ubo mechanism

        virtual auto reset() -> void = 0;
        virtual auto present(video::instance_t &in, const glm::mat4 &proj, const glm::mat4 &view) -> void = 0;

        virtual auto presented_texture() -> video::texture = 0;

        virtual auto dispath(video::instance_t &vi, const ui::draw_command_t &c) -> void = 0;
    };

    ///
    /// Create renderer method
    ///
    [[nodiscard]] auto create_renderer(std::string_view type, video::instance_t &vi, const uint32_t renderer_falgs) -> std::unique_ptr<instance>;

} // namespace renderer
