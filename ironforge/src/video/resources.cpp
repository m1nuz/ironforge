#include <core/application.hpp>
#include <core/assets.hpp>
#include <video/video.hpp>
#include <xxhash.h>

namespace video {
    struct texture_desc {
        std::string name;
        texture     tex;
        uint32_t    usage;
        uint64_t    hash;
        uint64_t    name_hash;
    };

    std::vector<texture_desc> textures;

    auto init_resources() -> void {
        textures.reserve(100);

        application::debug(application::log_category::video, "%\n", "Init resources");

        auto white_name = "white-map";
        auto im = video::imgen::make_color(128, 128, {1, 1, 1, 1}); // white
        textures.push_back({white_name, gl::create_texture_2d(im), 1, XXH64(im.pixels, im.width * im.height * 3, 0), XXH64(white_name, strlen(white_name), 0)});
        delete[] im.pixels;

        auto black_name = "black-map";
        im = video::imgen::make_color(128, 128, {0, 0, 0, 0}); // black
        textures.push_back({black_name, gl::create_texture_2d(im), 1, XXH64(im.pixels, im.width * im.height * 3, 0), XXH64(black_name, strlen(black_name), 0)});
        delete[] im.pixels;

        auto check_name = "check-map";
        im = video::imgen::make_check(128, 128, 0x08, {0, 0, 0, 0}); // check
        textures.push_back({check_name, gl::create_texture_2d(im), 1, XXH64(im.pixels, im.width * im.height * 3, 0), XXH64(check_name, strlen(check_name), 0)});
        delete[] im.pixels;
    }

    auto cleanup_resources() -> void {
        for (auto &td : textures)
            gl::destroy_texture(td.tex);

        textures.clear();
    }

    auto make_texture_2d(const texture_info &info) -> texture {
        texture_desc desc;
        desc.tex = gl::create_texture_2d(info);

        textures.push_back(desc);

        return desc.tex;
    }

    auto make_texture_2d(const image_data &data) -> texture {
        texture_desc desc;
        desc.tex = gl::create_texture_2d(data);

        textures.push_back(desc);

        return desc.tex;
    }

    auto make_vertices_source(const std::vector<vertices_data> &data, const vertices_desc &desc) -> vertices_source {
    }

    auto get_texture(const char *name) -> texture {
        // TODO: find already created texture

        auto imd = assets::get_image(name);

        if (!imd.pixels)
            return texture{}; // TODO: return one of valid texture

        return make_texture_2d(imd);
    }

    auto default_white_texture() -> texture {
        return textures[0].tex;
    }

    auto default_black_texture() -> texture {
        return textures[1].tex;
    }

    auto default_check_texture() -> texture {
        return textures[2].tex;
    }
} // namespace video
