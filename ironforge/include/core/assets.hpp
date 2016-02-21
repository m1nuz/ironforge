#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <tuple>

#include <SDL2/SDL_rwops.h>
#include<ironforge_common.hpp>

namespace assets {
    enum class result : int32_t {
        success,
        failure
    };

    struct binary_data {
        void        *data;
        size_t      size;
    };

    struct text_data {
        char        *text;
        size_t      size;
    };

    struct image_data {
        uint32_t    width;
        uint32_t    height;
        uint32_t    depth;
        int32_t     pixelformat;
        void        *pixels;
    };

    struct readers {
        std::vector<std::tuple<std::string, std::function<int32_t (SDL_RWops *rw, binary_data &)>>> binary_readers;
        std::vector<std::tuple<std::string, std::function<int32_t (SDL_RWops *rw, text_data &)>>>   text_readers;
        std::vector<std::tuple<std::string, std::function<int32_t (SDL_RWops *rw, image_data &)>>>  image_readers;
    };

    struct _default_readers : public readers {
        _default_readers();
    };

    extern _default_readers default_readers;

    __must_ckeck auto append(const readers &rs) -> result;
    __must_ckeck auto append(const std::string& ext, std::function<int32_t (SDL_RWops *rw, binary_data &)> reader) -> result;
    __must_ckeck auto append(const std::string& ext, std::function<int32_t (SDL_RWops *rw, text_data &)> reader) -> result;
    __must_ckeck auto append(const std::string& ext, std::function<int32_t (SDL_RWops *rw, image_data &)> reader) -> result;
    __must_ckeck auto open(const std::string& path) -> result;
} // namespace assets
