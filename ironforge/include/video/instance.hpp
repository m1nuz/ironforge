#pragma once

#include <SDL2/SDL_video.h>

#include <vector>
#include <unordered_map>
#include <string>

#include <video/texture.hpp>
#include <video/framerate.hpp>
#include <video/glyphs.hpp>
#include <video/types.hpp>

namespace video {

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

        std::unordered_map<std::string, size_t>         fonts_mapping;
        std::vector<font_t>                             fonts;

        std::unordered_map<std::string, texture>        textures;
        std::unordered_map<std::string, program>        programs;
        std::unordered_map<std::string, mesh>           meshes;
        std::vector<memory_buffer>                      buffers;
        std::vector<gl::vertex_array>                   arrays;
    };

    typedef struct instance_type instance_t;

} // namespace video
