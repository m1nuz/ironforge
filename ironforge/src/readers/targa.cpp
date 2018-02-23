#include <readers/targa.hpp>
#include <SDL2/SDL.h>

enum TARGA_DATA_TYPE
{
    TARGA_DATA_NO = 0,
    TARGA_DATA_COLOR_MAPPED = 1,        // indexed
    TARGA_DATA_TRUE_COLOR = 2,          // RGB
    TARGA_DATA_BLACK_AND_WHITE = 3,     // grayscale
    TAGRA_DATA_RLE_COLOR_MAPPED = 9,
    TARGA_DATA_RLE_TRUE_COLOR = 10,
    TARGA_DATA_RLE_BLACK_AND_WITE = 11
};

#pragma pack(push, tga_header_align)
#pragma pack(1)
typedef struct TargaHeader
{
    uint8_t     length;
    uint8_t     color_map;
    uint8_t     data_type;
    uint16_t    colormap_index;
    uint16_t    colormap_length;
    uint8_t     colormap_entry_size;
    uint16_t    x;
    uint16_t    y;
    uint16_t    width;
    uint16_t    height;
    uint8_t     bpp;
    uint8_t     decription;
} TARGA_HEADER;
#pragma pack(pop, tga_header_align)

auto read_targa(SDL_RWops *rw, assets::image_data& image) -> int32_t {
    if (!rw)
        return -1;

    Sint64 lenght = SDL_RWsize(rw);

    TARGA_HEADER header;
    SDL_RWread(rw, &header, sizeof(header), 1);

    const uint8_t bytesperpixel = header.bpp / 8;
    std::vector<uint8_t> data;
    data.resize(header.width * header.height * (bytesperpixel + 1));

    uint8_t *pdata = &data[0];

    if (header.data_type == TARGA_DATA_RLE_TRUE_COLOR) {
        uint8_t block = 0;
        size_t readen = 0;

        for (int i = 0; i < header.width * header.height; i++) {
            readen = SDL_RWread(rw, &block, 1, 1);

            if (readen) {
                uint8_t count = (block & 0x7f) + 1;

                if (block & 0x80) {
                    uint8_t bytes[4] = {0};
                    SDL_RWread(rw, bytes, bytesperpixel, 1);

                    for(int j = 0; j < count; j++) {
                        memcpy(pdata, bytes, bytesperpixel);
                        pdata += bytesperpixel;
                    }
                } else {
                    SDL_RWread(rw, pdata, bytesperpixel * count, 1);
                    pdata += bytesperpixel * count;
                }
            }
        }

        SDL_RWclose(rw);
    }
    else if ((header.data_type == TARGA_DATA_TRUE_COLOR) || (header.data_type == TARGA_DATA_BLACK_AND_WHITE)) {
        if (!SDL_RWread(rw, &data[0], lenght - sizeof(header), 1))
            SDL_RWclose(rw);
    }

    switch (header.bpp) {
    case 8:
        image.pixelformat = video::pixel_format::r8;
        break;
    case 24:
        image.pixelformat = video::pixel_format::bgr8;
        break;
    case 32:
        image.pixelformat = video::pixel_format::bgra8;
        break;
    default:
        image.pixelformat = video::pixel_format::unknown;
    }

    image.width = header.width;
    image.height = header.height;
    image.depth = 0;
    image.pixels = data;

    return 0;
}
