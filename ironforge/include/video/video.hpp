#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <ironforge_common.hpp>
#include <video/vertices.hpp>
#include <video/vertices_gen.hpp>
#include <video/command.hpp>
#include <video/command_buffer.hpp>
#include <video/buffer.hpp>
#include <video/vertex_array.hpp>
#include <video/texture.hpp>
#include <video/image_gen.hpp>

namespace video {
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

    namespace gl = gl330;
    namespace vk = vulkan1;

    using texture = gl::texture;

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

    __must_ckeck auto init(const std::string &title, int32_t w, int32_t h, bool vsync) -> result;
    auto cleanup() -> void;
    auto present(std::vector<gl::command_buffer *> &&buffers) -> void;
    auto get_string(result r) -> const char *;
    auto is_extension_supported(const char *extension) -> bool;
    auto get_info() -> const char *;

    auto make_texture_2d(const video::texture_info &info) -> texture;
    auto make_texture_2d(const image_data &data) -> texture;
    auto make_vertices_source(const std::vector<vertices_data> &data, const vertices_desc &desc) -> vertices_source;

    auto get_texture(const char *name) -> texture;
    auto default_white_texture() -> texture;
    auto default_black_texture() -> texture;
    auto default_check_texture() -> texture;
} // namespace video
