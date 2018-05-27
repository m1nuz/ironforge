#pragma once

#include <stdint.h>
#include <SDL2/SDL_surface.h>
#include <video/image_gen.hpp>

namespace video {

    struct atlas {
        struct node;

        node            *root;
        SDL_Surface     *surface;
        int32_t         padding;
    };

    auto create_atlas(int width, int height, int padding) -> atlas;
    auto destroy_atlas(atlas &_atlas) -> void;
    auto insert_surface(atlas &_atlas, SDL_Surface *_surface) -> SDL_Rect;
    auto insert_image(atlas &_atlas, image_data &_image) -> SDL_Rect;
    auto get_atlas_texture(atlas &_atlas) -> image_data;

} // namespace video
