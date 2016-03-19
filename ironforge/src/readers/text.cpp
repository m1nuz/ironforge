#include <cassert>
#include <readers/text.hpp>

auto read_text(SDL_RWops *rw, assets::text_data& text) -> int32_t {
    assert(rw != nullptr);

    if (!rw)
        return -1;

    auto lenght = SDL_RWsize(rw);

    if (lenght > 0) {
        text.text = new char [lenght];
        text.text[lenght - 1] = 0;
        text.size = lenght - 1;

        // TODO: check full size
        SDL_RWread(rw, text.text, lenght - 1, 1);
    }

    SDL_RWclose(rw);

    return 0;
}


auto read_shader_text(SDL_RWops *rw, assets::text_data& text) -> int32_t {
    return read_text(rw, text);
}
