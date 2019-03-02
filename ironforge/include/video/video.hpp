#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

#include <core/json.hpp>
#include <core/journal.hpp>

#include <video/errors.hpp>
#include <video/vertices.hpp>
#include <video/vertices_gen.hpp>
#include <video/command.hpp>
#include <video/command_buffer.hpp>
#include <video/buffer.hpp>
#include <video/vertex_array.hpp>
#include <video/texture.hpp>
#include <video/image_gen.hpp>
#include <video/vertex_array.hpp>
#include <video/shader.hpp>
#include <video/program.hpp>
#include <video/pipeline.hpp>
#include <video/sampler.hpp>
#include <video/renderbuffer.hpp>
#include <video/framebuffer.hpp>
#include <video/state.hpp>
#include <video/framerate.hpp>
#include <video/stats.hpp>
#include <video/glyphs.hpp>
#include <video/atlas.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace video {
    /* specific gl APIs */
    namespace gles2 {
        constexpr int major_version = 2;
        constexpr int minor_version = 0;
    } // namespace gles2

    namespace gles31 {
        constexpr int major_version = 3;
        constexpr int minor_version = 1;
    } // namespace gles31

    namespace gl330 {
        constexpr int major_version = 3;
        constexpr int minor_version = 3;

        constexpr auto api_name = "opengl";
        constexpr auto api_version = (major_version * 100) + (minor_version * 10);

        struct command_buffer;
        struct buffer;              // Buffer Object
        struct shader;              // Shader Object
        struct program;             // Program Object
        struct vertex_array;        // Vertex Array Object
        struct query;               // Query Object
        struct pipeline;            // Program Pipeline Object
        struct transform_feedback;  // Transform Feedback Object
        struct sampler;             // Sampler Object
        struct texture;             // Texture Object
        struct renderbuffer;        // Renderbuffer Object
        struct framebuffer;         // Framebuffer Object                
    } // namespace gl330

    namespace gl450 {
        constexpr int major_version = 4;
        constexpr int minor_version = 5;
    } // namespace gl450

    namespace vulkan1 {
        constexpr int major_version = 1;
        constexpr int minor_version = 0;
    }

    /* general api */
    namespace gl = gl330;
    namespace vk = vulkan1;
    // namespace d3d

    using texture = gl::texture;
    using program = gl::program;
    using memory_buffer = gl::buffer;
    using command_queue = gl::command_buffer;
    using framebuffer = gl::framebuffer;
    struct vertices_source;
    struct mesh;

    enum class result : int32_t {
        success,
        failure,
        error_create_window,
        error_create_context
    };

    using video_result = std::variant<instance_t, std::error_code>;

    ///
    /// \brief is_extension_supported
    /// \param extension
    /// \return
    ///
    [[nodiscard]] auto is_extension_supported(const char *extension) -> bool;

    ///
    /// \brief Initialize video resources
    /// \param asset
    /// \param info
    /// \return
    ///
    [[nodiscard]] auto init(assets::instance_t &asset, const json &info) -> video_result;

    ///
    /// \brief Present command queue to video renderer
    /// \param[inout] vi Video context
    /// \param[in] buffers commands queues
    ///
    auto present(instance_t &vi, const std::vector<command_queue *> &buffers) -> void;

    ///
    /// \brief Cleanup video resources
    /// \param[inout] vi Video context
    ///
    auto cleanup(instance_t &vi) -> void;

    auto process_resources(assets::instance_t &asset, instance_t &inst) -> void;

    auto create_texture(assets::instance_t &asset, instance_t &inst, const json &info) -> texture;
    auto create_program(assets::instance_t &asset, instance_t &inst, const json &info) -> program;
    auto create_mesh(assets::instance_t &asset, instance_t &vi, const json &info) -> std::optional<mesh>;

    auto make_texture_2d(instance_t &vi, const std::string &name, const image_data &data, const uint32_t flags) -> texture;
    auto make_texture_cube(instance_t &vi, const std::string &name, const std::string (&names)[6]) -> texture;
    auto make_vertices_source(instance_t &vi, const std::vector<vertices_data> &data, const vertices_desc &desc, std::vector<vertices_draw> &draws) -> vertices_source;

    auto get_texture(instance_t &vi, const std::string &name) -> texture;
    auto get_heightmap(instance_t &vi, const std::string &name) -> heightmap_t;
    auto get_shader(instance_t &vi, const std::string &name) -> program;
    auto get_font(instance_t &vi, const std::string &name) -> uint32_t;

    inline bool is_ok(const video_result &res) {
        return std::holds_alternative<instance_t>(res);
    }

} // namespace video

#include <video/instance.hpp>
#include <video/types.hpp>
