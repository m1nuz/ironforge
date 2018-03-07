#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

#include <SDL2/SDL_video.h>

#include <core/common.hpp>
#include <core/journal.hpp>
#include <video/errors.hpp>
#include <video/screen.hpp>
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

#include <json.hpp>

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

        enum class command_type : uint32_t;
        struct command;
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

    struct vertices_source {
        gl::vertex_array    array;
        gl::buffer          vertices;
        gl::buffer          elements;
    };

    enum class result : int32_t {
        success,
        failure,
        error_create_window,
        error_create_context
    };

    enum class texture_filtering {
        bilinear,
        trilinear,
        anisotropic,
        max_filtering
    };

    struct mesh {
        vertices_desc   desc;
        vertices_source source;
        vertices_draw   draw;
    };

    struct instance_type {
        instance_type() = default;

        SDL_Window                                      *window = nullptr;
        SDL_GLContext                                   graphic_context = nullptr;
        int                                             w = 0;
        int                                             h = 0;
        float                                           aspect_ratio = 0.f;
        texture_filtering                               texture_filter = texture_filtering::bilinear;
        uint32_t                                        texture_level = 0;
        frame_info                                      stats_info = {};

        std::string                                     vendor;
        std::string                                     renderer;
        std::string                                     version;
        std::string                                     shading_language_version;
        int32_t                                         max_supported_anisotropy = 0;
        int32_t                                         max_uniform_components = 0;

        std::unordered_map<std::string, font_t>         fonts;
        std::unordered_map<std::string, texture>        textures;
        std::unordered_map<std::string, program>        programs;
        std::unordered_map<std::string, mesh>           meshes;
        std::vector<memory_buffer>                      buffers;
        std::vector<gl::vertex_array>                   arrays;
    };

    typedef struct instance_type instance_t;

    using video_result = std::variant<instance_t, std::error_code>;
    using json = nlohmann::json;

    ///
    /// \brief is_extension_supported
    /// \param extension
    /// \return
    ///
    auto is_extension_supported(const char *extension) -> bool;

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
    /// \param in
    ///
    auto cleanup(instance_t &vi) -> void;

    ///
    /// \brief Get vendor, renderer, version and shading language version
    /// \param[inout] inst
    /// \return Info string
    ///
    auto get_info(instance_t &inst) -> std::string;

    auto process(assets::instance_t &asset, instance_t &inst) -> void;

    auto create_texture(assets::instance_t &asset, instance_t &inst, const json &info) -> texture;
    auto create_program(assets::instance_t &asset, instance_t &inst, const json &info) -> program;
    auto create_mesh(assets::instance_t &asset, instance_t &vi, const json &info) -> std::optional<mesh>;

    auto make_texture_2d(instance_t &vi, const std::string &name, const image_data &data, const uint32_t flags) -> texture;
    auto make_texture_cube(instance_t &vi, const std::string &name, const std::string (&names)[6]) -> texture;
    auto make_vertices_source(instance_t &vi, const std::vector<vertices_data> &data, const vertices_desc &desc, std::vector<vertices_draw> &draws) -> vertices_source;

    auto get_texture(instance_t &vi, const std::string &name) -> texture;
    auto get_heightmap(const std::string &name) -> heightmap_t;
    auto get_shader(instance_t &vi, const std::string &name) -> program;    

    inline bool is_ok(const video_result &res) {
        return std::holds_alternative<instance_t>(res);
    }
} // namespace video
