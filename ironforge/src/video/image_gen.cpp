#include <video/image_gen.hpp>

namespace video {
    namespace imgen {
        auto make_radial_gradient(int32_t width, int32_t height, uint8_t c0, uint8_t c1, int32_t radius) -> image_data {
            // the center of the surface
            double cx = (double)width / 2.0;
            double cy = (double)height / 2.0;

            // compute max distance M from center
            double M = (double)radius;//sqrt(cx * cx + cy * cy);

            // the color delta
            double dc = c1 - c0;

            // and constant used in the code....
            double K = dc / M;

            rgb_color *pixels = new rgb_color[width * height];

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    // coodinates relative to center, shifted to pixel centers
                    double x = i - cx + 0.5;
                    double y = j - cy + 0.5;
                    double r = sqrt(x * x + y * y);  // the distance

                    if (r < M) {
                        pixels[i * width + j].r = (uint8_t)(r * K + c0);
                        pixels[i * width + j].g = pixels[i * width + j].r;
                        pixels[i * width + j].b = pixels[i * width + j].r;
                    } else {
                        pixels[i * width + j].r = c1;
                        pixels[i * width + j].g = pixels[i * width + j].r;
                        pixels[i * width + j].b = pixels[i * width + j].r;
                    }
                }

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgb8, reinterpret_cast<uint8_t*>(pixels)};
        }

        auto make_color(int32_t width, int32_t height, rgb_color color) -> image_data {
            rgb_color *pixels = new rgb_color[width * height];

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    pixels[j * width + i] = color;
                }

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgb8, reinterpret_cast<uint8_t*>(pixels)};
        }

        auto make_check(int32_t width, int32_t height, uint8_t mask, rgb_color color) -> image_data {
            // TODO: check width and height
            rgb_color *pixels = new rgb_color[width * height];

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    unsigned c = (((i & mask) == 0) ^ ((j & mask) == 0));
                    pixels[j * width + i] = color;
                    pixels[j * width + i].r *= c;
                    pixels[j * width + i].g *= c;
                    pixels[j * width + i].b *= c;
                }

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgb8, reinterpret_cast<uint8_t*>(pixels)};
        }
    } // namespace imggen
} // namespace video
