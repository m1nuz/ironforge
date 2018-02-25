#include <readers/binary.hpp>

auto read_binary(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::binary_data_t> {
    (void)inst;

    assert(rw != nullptr);

    if (!rw)
        return {};

    const auto size = static_cast<size_t>(SDL_RWsize(rw));

    assets::binary_data_t data;

    if (size > 0) {
        data.resize(size);

        SDL_RWread(rw, &data[0], size, 1);
        SDL_RWclose(rw);

        return data;
    }

    return {};
}
