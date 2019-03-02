#include <video/image_gen.hpp>

namespace video {

    namespace imgen {

        auto make_radial_gradient(int32_t width, int32_t height, uint8_t c0, uint8_t c1, int32_t radius) -> image_data {
            // the center of the surface
            const double cx = static_cast<double>(width) / 2.0;
            const double cy = static_cast<double>(height) / 2.0;

            // compute max distance M from center
            const double M = static_cast<double>(radius);//sqrt(cx * cx + cy * cy);

            // the color delta
            const double dc = c1 - c0;

            // and constant used in the code....
            const double K = dc / M;

            std::vector<rgb_color> pixels;
            pixels.resize(static_cast<size_t>(width * height));

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    // coodinates relative to center, shifted to pixel centers
                    double x = i - cx + 0.5;
                    double y = j - cy + 0.5;
                    double r = sqrt(x * x + y * y);  // the distance

                    if (r < M) {
                        pixels[i * width + j].r = static_cast<uint8_t>(r * K + c0);
                        pixels[i * width + j].g = pixels[i * width + j].r;
                        pixels[i * width + j].b = pixels[i * width + j].r;
                    } else {
                        pixels[i * width + j].r = c1;
                        pixels[i * width + j].g = pixels[i * width + j].r;
                        pixels[i * width + j].b = pixels[i * width + j].r;
                    }
                }

            std::vector<uint8_t> all_pixels;
            all_pixels.resize(width * height * 3);

            memcpy(std::data(all_pixels), std::data(pixels), all_pixels.size());

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgb8, all_pixels};
        }

        auto make_color(int32_t width, int32_t height, rgb_color color) -> image_data {
            rgb_color *pixels = new rgb_color[width * height];

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    pixels[j * width + i] = color;
                }

            std::vector<uint8_t> all_pixels;
            all_pixels.resize(width * height * 3);

            memcpy(&all_pixels[0], reinterpret_cast<uint8_t*>(pixels), all_pixels.size());

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgb8, all_pixels};
        }

        auto make_color(int32_t width, int32_t height, rgba_color color) -> image_data {
            rgba_color *pixels = new rgba_color[width * height];

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    pixels[j * width + i] = color;
                }

            std::vector<uint8_t> all_pixels;
            all_pixels.resize(width * height * 3);

            memcpy(&all_pixels[0], reinterpret_cast<uint8_t*>(pixels), all_pixels.size());

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgba8, all_pixels};
        }

        auto make_check(int32_t width, int32_t height, uint8_t mask, rgb_color color) -> image_data {
            // TODO: check width and height
            rgb_color *pixels = new rgb_color[width * height];

            for (int j = 0; j < height; j++)
                for (int i = 0; i < width; i++) {
                    unsigned c = (((i & mask) == 0) ^ ((j & mask) == 0));
                    pixels[j * width + i].r = c == 0 ? color.r : 0xff;
                    pixels[j * width + i].g = c == 0 ? color.g : 0xff;
                    pixels[j * width + i].b = c == 0 ? color.b : 0xff;
                }

            std::vector<uint8_t> all_pixels;
            all_pixels.resize(width * height * 3);

            memcpy(&all_pixels[0], reinterpret_cast<uint8_t*>(pixels), all_pixels.size());

            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 0, pixel_format::rgb8, all_pixels};
        }

    } // namespace imggen

} // namespace video
