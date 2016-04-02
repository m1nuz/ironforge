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

#ifndef ABS
#define ABS(v) ((v) < 0 ? -(v) : (v))
#endif

extern char *
str_replace_once_fast(const char *orig, size_t orig_size, const char *rep, size_t rep_size, char *str, size_t str_size) {
    assert(orig != NULL);
    assert(rep != NULL);
    assert(str != NULL);

    const size_t sz = ABS((int)orig_size - (int)rep_size);
    const size_t nbytes = str_size + sz + 1;

    char *p = (char*)memmem(str, str_size, orig, orig_size);
    if (!p) {
        return strdup(str);
    }

    char *buffer = (char*)malloc(nbytes);
    buffer[nbytes - 1] = 0;

    memcpy(buffer, str, p - str);
    memcpy(buffer + (p - str), rep, rep_size);
    const size_t n = str_size - (p - str) - orig_size;
    memcpy(buffer + (p - str) + rep_size, p + orig_size, n);

    return buffer;
}


// TODO: rewrite this with c++
auto read_shader_text(SDL_RWops *rw, assets::text_data& text) -> int32_t {
    const char include_directive[] = "#include";
    const size_t step = sizeof(include_directive) - 1;

    int ret = read_text(rw, text);

    char *p = text.text;

    while (p < text.text + text.size) {
        char *sp;
        if ((sp = strstr(p, include_directive)) != nullptr) {
            //printf("%s\n", text.text);
            //printf("----END\n");

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

            auto include_text = assets::get_text(name);

            //memset(sp, ' ', end - sp);

            char origin[end - sp];
            memcpy(origin, sp, sizeof(origin));
            origin[end - sp] = 0;

            char *old_text = text.text;
            text.text = str_replace_once_fast(origin, strlen(origin), include_text.text, include_text.size, text.text, text.size);
            text.size = strlen(text.text);
            p = text.text + pos;
            free(old_text);

            // TODO: memory...
            delete[] include_text.text;
        }

        p += step;
    }

    //printf("%s\n", text.text);

    return ret;
}
