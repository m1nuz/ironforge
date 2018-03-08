#include <cassert>
#include <video/atlas.hpp>

namespace video {

    struct atlas::node {
        node() = default;

        node(int _x, int _y, int _width, int _height)
            : left{nullptr}, right{nullptr}, x{_x}, y{_y}, width{_width}, height{_height}, usage{false} {

        }

        atlas::node     *left = nullptr;
        atlas::node     *right = nullptr;

        int             x = 0;
        int             y = 0;
        int             width = 0;
        int             height = 0;
        bool            usage = false;
    };

    static auto insert_child_surface(atlas::node *node, SDL_Surface *surface, int padding) -> atlas::node* {
        assert(node != nullptr);

        if ((node->left != nullptr) || (node->right != nullptr)) {
            auto lr = insert_child_surface(node->left, surface, padding);

            if (lr == nullptr)
                lr = insert_child_surface(node->right, surface, padding);

            return lr;
        }

        int image_w = surface->w + padding * 2;
        int image_h = surface->h + padding * 2;

        if (node->usage || (image_w > node->width) || (image_h > node->height))
            return nullptr;

        if ((image_w == node->width) && (image_h == node->height)) {
            node->usage = 1;
            return node;
        }

        // extend to the right
        if (node->width - image_w > node->height - image_h) {
            node->left = new atlas::node{node->x, node->y, image_w, node->height};
            node->right = new atlas::node{node->x + image_w, node->y, node->width - image_w, node->height};
        } else  {
            // extend to bottom
            node->left = new atlas::node{node->x, node->y, node->width, image_h};
            node->right = new atlas::node{node->x, node->y + image_h, node->width, node->height - image_h};
        }

        return insert_child_surface(node->left, surface, padding);
    }

    auto create_atlas(int width, int height, int padding) -> atlas {
        uint32_t rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

        auto surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

        if (!surface)
            return {nullptr, nullptr, 0};

        auto root = new atlas::node{0, 0, width, height};

        return {root, surface, padding};
    }

    static auto recursive_delete(atlas::node *node) -> void {
        assert(node != NULL);

        if (node->left)
            recursive_delete(node->left);

        if (node->right)
            recursive_delete(node->right);

        delete node;
    }

    auto destroy_atlas(atlas &_atlas) -> void {
        if (_atlas.root)
            recursive_delete(_atlas.root);

        SDL_FreeSurface(_atlas.surface);
    }

    auto insert_surface(atlas &_atlas, SDL_Surface *_surface) -> SDL_Rect {
        auto node = insert_child_surface(_atlas.root, _surface, _atlas.padding);

        if (!node)
            return {0, 0, 0, 0};

        SDL_Rect rect = {node->x + _atlas.padding, node->y + _atlas.padding, _surface->w, _surface->h};

        SDL_BlitSurface(_surface, 0, _atlas.surface, &rect);

        return rect;
    }

    auto insert_image(atlas &_atlas, image_data &_image) -> SDL_Rect {
        uint32_t rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

        auto surface = SDL_CreateRGBSurfaceFrom(reinterpret_cast<void*>(&_image.pixels[0]), _image.width, _image.height, 32, _image.width, rmask, gmask, bmask, amask);

        if (!surface)
            return {0, 0, 0, 0};

        //SDL_Surface *rgba = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);

        auto rc = insert_surface(_atlas, surface);
        SDL_FreeSurface(surface);

        return rc;
    }

    auto get_atlas_texture(atlas &_atlas) -> image_data {
        std::vector<uint8_t> all_pixels;
        all_pixels.resize(_atlas.surface->w * _atlas.surface->h * 4);

        memcpy(&all_pixels[0], _atlas.surface->pixels, all_pixels.size());

        return image_data{static_cast<uint32_t>(_atlas.surface->w), static_cast<uint32_t>(_atlas.surface->h), 0, pixel_format::rgba8, all_pixels};
    }
} // namespace video
