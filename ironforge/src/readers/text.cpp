#include <cassert>
#include <readers/text.hpp>
#include <core/application.hpp>

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
    const char include_directive[] = "#include";
    const size_t step = sizeof(include_directive) - 1;

    int ret = read_text(rw, text);

    /*char *p = text.text;

    while (p < text.text + text.size) {
        char *sp;
        if ((sp = strstr(p, include_directive)) != nullptr) {
            size_t pos = p - text.text;
            char *end = strchr(sp, '\n');

            if ((size_t)(end - sp) == step) {
                application::error(application::log_category::input, "%\n", "#include expect \"FILENAME\"");
                application::quit();
            }

            char *bracers_first = strchr(sp, '\"');
            if (bracers_first == NULL) {
                application::error(application::log_category::input, "%\n", "#include expect \"FILENAME\"");
                application::quit();
            }

            char *space = sp + sizeof(include_directive);
            while (space < bracers_first)
                if (*space == ' ')
                    space++;
                else {
                    application::error(application::log_category::input, "%\n", "#include expect \"FILENAME\"");
                    application::quit();
                }

            char *bracers_second = strchr(bracers_first + 1, '\"');
            if (bracers_second == NULL) {
                application::error(application::log_category::input, "%\n", "#include expect \"FILENAME\"");
                application::quit();
            }

            size_t name_size = bracers_second - bracers_first - 1;
            if (name_size == 0) {
                application::error(application::log_category::input, "%\n", "empty filename in #include");
                application::quit();
            }

            char name[name_size];
            memcpy(name, bracers_first + 1, name_size);
            name[name_size] = 0;

            SDL_RWops *include_rw = assets::get_file(name);

            //memset(sp, ' ', end - sp);

            char origin[end - sp];
            memcpy(origin, sp, sizeof(origin));
            origin[end - sp] = 0;

            assets::text_data replace;

            ret = read_text(include_rw, replace);

            //if (ret < 0) TODO: make error

            //printf("ORIGIN |%s|\nREPLACE |%s|\n", origin, replace);

            char *old_text = text;
            text = str_replace_once(origin, replace, text);
            p = text + pos;
            free(old_text);
            //printf("NEW SHADER TEXT ||||||||||%s||||||||||\n", text);

            delete[] replace.text;
        }

        p += step;
    }*/

    return ret;
}
