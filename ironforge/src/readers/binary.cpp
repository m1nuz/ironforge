#include <readers/binary.hpp>

auto read_binary(SDL_RWops *rw, assets::binary_data &data) -> int32_t {
    assert(rw != nullptr);

    if (!rw)
        return -1;

    auto size = SDL_RWsize(rw);

    if (size > 0) {
        data.raw_memory = operator new(size);
        data.size = size;

        SDL_RWread(rw, data.raw_memory, size, 1);
    }

    SDL_RWclose(rw);

    return 0;
}
