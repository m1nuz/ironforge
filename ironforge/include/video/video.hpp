#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <ironforge_common.hpp>
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
    };

    struct _config {
        texture_filtering filtering;
    };

    __must_ckeck auto init(const std::string &title, int32_t w, int32_t h, bool fullscreen, bool vsync) -> result;
    auto cleanup() -> void;
    auto present(const std::vector<gl330::command_buffer *> &buffers) -> void;
    auto get_string(result r) -> const char *;
    auto is_extension_supported(const char *extension) -> bool;
    auto get_info() -> const char *;
    auto is_debugging() -> bool;

    auto process() -> void;

    /*auto make_texture_2d(const video::texture_info &info) -> texture;
    auto make_texture_2d(const image_data &data) -> texture;
    auto make_texture_2d(const std::string &name, const image_data &data) -> texture;*/
    auto make_texture_cube(const std::string &name, const std::string (&names)[6]) -> texture;
    auto make_vertices_source(const std::vector<vertices_data> &data, const vertices_desc &desc, std::vector<vertices_draw> &draws) -> vertices_source;

    auto default_white_texture() -> texture;
    auto default_black_texture() -> texture;
    auto default_check_texture() -> texture;
    auto default_red_texture() -> texture;
    auto get_texture(const char *name, const texture &default_tex = default_check_texture()) -> texture;

    auto query_texture(texture &tex, const texture_desc *desc) -> texture;

    auto make_program(const gl::program_info &info) -> program;
    auto get_shader(const char *name) -> program;

    extern _config config;
    extern int32_t max_uniform_components;    
} // namespace video
