#pragma once

#include <cstdint>
#include <string>
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

    struct _config {
        texture_filtering filtering;
    };

    struct instance_type {
        SDL_Window      *window = nullptr;
        SDL_GLContext   graphic_context = nullptr;
        int             w = 0;
        int             h = 0;
        texture_filtering tex_filtering;
        frame_info      stats_info = {};
    };

    typedef struct instance_type instance_t;

    [[nodiscard]] auto init(instance_t &inst, const std::string &title, const int32_t w, const int32_t h, const bool fullscreen, const bool vsync, const bool debug) -> result;
    auto reset(instance_t &inst, const int32_t w, const int32_t h, const bool fullscreen, const bool vsync, const bool debug) -> result;
    auto present(instance_t &in, const std::vector<gl::command_buffer *> &buffers) -> void;
    auto get_string(const result r) -> const char *;
    auto is_extension_supported(const char *extension) -> bool;
    auto get_info() -> const char *;

    auto process(instance_t &inst) -> void;

    /*auto make_texture_2d(const video::texture_info &info) -> texture;
    auto make_texture_2d(const image_data &data) -> texture;*/
    auto make_texture_2d(const std::string &name, const image_data &data, const uint32_t flags) -> texture;
    auto make_texture_cube(assets::instance_t &asset, const std::string &name, const std::string (&names)[6]) -> texture;
    auto make_vertices_source(const std::vector<vertices_data> &data, const vertices_desc &desc, std::vector<vertices_draw> &draws) -> vertices_source;

    auto default_white_texture() -> texture;
    auto default_black_texture() -> texture;
    auto default_check_texture() -> texture;
    auto default_red_texture() -> texture;
    auto get_texture(assets::instance_t &asset, const char *name, const texture &default_tex = default_check_texture()) -> texture;

    auto query_texture(texture &tex, const texture_desc *desc) -> void;
    auto query_texture(texture &tex) -> void;

    auto get_heightmap(const char *name) -> heightmap_t;

    auto make_program(assets::instance_t &asset, const gl::program_info &info) -> program;
    auto get_shader(const char *name) -> program;

    auto init_resources(instance_t &inst, assets::instance_t &asset) -> void;
    auto cleanup_resources() -> void;

    using json = nlohmann::json;
    using video_result = std::variant<instance_t, std::error_code>;

    inline bool is_ok(const video_result &res) {
        return std::holds_alternative<instance_t>(res);
    }

    [[nodiscard]] auto init_once(assets::instance_t &asset, const json &info) -> video_result;
    auto cleanup_once(instance_t &in) -> void;

    extern _config config;
    extern int32_t max_uniform_components;    
} // namespace video
