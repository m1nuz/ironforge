#include <core/journal.hpp>
#include <core/assets.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <readers/readers.hpp>

namespace fs = boost::filesystem;

namespace assets {
    _default_readers::_default_readers() {
        text_readers.emplace_back(".vert", read_shader_text);
        text_readers.emplace_back(".frag", read_shader_text);
        text_readers.emplace_back(".glsl", read_shader_text);
        text_readers.emplace_back(".lua", read_text);
        text_readers.emplace_back(".scene", read_text);
        text_readers.emplace_back(".txt", read_text);
        image_readers.emplace_back(".tga", read_targa);
        binary_readers.emplace_back(".ttf", read_binary);
        //binary_readers.emplace_back(".*", read_binary);
    }

    _default_readers default_readers;

    // ext, reader
    std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, binary_data &)>> binary_readers;
    std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, text_data &)>>   text_readers;
    std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, image_data &)>>  image_readers;

    // name, data
    std::unordered_map<std::string, binary_data>    binaries;
    std::unordered_map<std::string, text_data>      texts;
    std::unordered_map<std::string, image_data>     images;

    // name, path
    std::unordered_map<std::string, std::string>    files;

    static auto is_readable(const std::string &ext) -> bool {
        if (binary_readers.find(ext) != binary_readers.end())
            return true;

        if (text_readers.find(ext) != text_readers.end())
            return true;

        if (image_readers.find(ext) != image_readers.end())
            return true;

        return false;
    }

    auto append(const readers &rs) -> result {
        auto res = result::failure;

        for (auto & r : rs.binary_readers)
            if ((res = append(std::get<0>(r), std::get<1>(r))) != result::success)
                return res;

        for (auto & r : rs.text_readers)
            if ((res = append(std::get<0>(r), std::get<1>(r))) != result::success)
                return res;

        for (auto & r : rs.image_readers)
            if ((res = append(std::get<0>(r), std::get<1>(r))) != result::success)
                return res;

        return result::success;
    }

    auto append(const std::string &ext, std::function<int32_t (SDL_RWops *rw, binary_data &)> reader) -> result {
        if (!reader || ext.empty())
            return result::failure;

        binary_readers.insert({ext, reader});

        return result::success;
    }

    auto append(const std::string &ext, std::function<int32_t (SDL_RWops *rw, text_data &)> reader) -> result {
        if (!reader || ext.empty())
            return result::failure;

        text_readers.insert({ext, reader});

        return result::success;
    }

    auto append(const std::string &ext, std::function<int32_t (SDL_RWops *rw, image_data &)> reader) -> result {
        if (!reader || ext.empty())
            return result::failure;

        image_readers.insert({ext, reader});

        return result::success;
    }

    auto open(const std::string& path) -> result {
        fs::path p(path);

        if (!fs::exists(p))
            return result::failure; // TODO: make error

        if(fs::is_directory(p)) {
            for(auto& entry : boost::make_iterator_range(fs::recursive_directory_iterator(p), {}))
                if (fs::is_regular_file(entry.path())) {

                    if (!is_readable(entry.path().extension().string()))
                        continue;

                    game::journal::debug(game::journal::_GAME, "Asset found %", entry.path().string());

                    files.insert({entry.path().filename().string(), entry.path().string()});
                }
        }

        return result::success;
    }

    auto process() -> void {
        // TODO: process file queue, thread loading
    }

    auto cleanup() -> void {

    }

    auto get_text(const std::string& name) -> text_data { // TODO: return optional
        auto t = texts.find(name);

        if (t != texts.end())
            return t->second;

        auto f = files.find(name);

        if (f != files.end()) {
            fs::path p(f->second);

            auto r = text_readers.find(p.extension().string());

            if (r != text_readers.end()) {
                auto td = text_data{nullptr, 0};

                auto ret = r->second(SDL_RWFromFile(f->second.c_str(), "r"), td);
                if (ret != 0) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {nullptr, 0};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                texts.insert({p.filename().string(), td}); // TODO: free memory
                return td;
            }
        }        

        game::journal::warning(game::journal::_GAME, "File '%' not found", name);

        return {nullptr, 0};
    }

    auto get_text_absolute(const std::string& path) -> text_data {
        fs::path p(path);

        auto r = text_readers.find(p.extension().string());

        if (r == text_readers.end())
            return {nullptr, 0};

        auto td = text_data{nullptr, 0};

        auto ret = r->second(SDL_RWFromFile(path.c_str(), "r"), td);
        if (ret != 0) {
            game::journal::error(game::journal::_SYSTEM, "Can't read file %", path);
            return {nullptr, 0};
        }

        game::journal::debug(game::journal::_GAME, "Read file %", path);
        texts.insert({p.filename().string(), td}); // TODO: free memory
        return td;
    }

    auto get_image(const std::string& name) -> image_data {
        auto im = images.find(name);

        if (im != images.end())
            return im->second;

        auto f = files.find(name);

        if (f != files.end()) {
            fs::path p(f->second);

            auto r = image_readers.find(p.extension().string());

            if (r != image_readers.end()) {
                auto imd = image_data{0, 0, 0, video::pixel_format::unknown, nullptr};

                auto ret = r->second(SDL_RWFromFile(f->second.c_str(), "r"), imd);
                if (ret != 0) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {0, 0, 0, video::pixel_format::unknown, nullptr};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                images.insert({p.filename().string(), imd}); // TODO: free memory
                return imd;
            }
        }

        return {0, 0, 0, video::pixel_format::unknown, nullptr};
    }

    auto get_binary(const std::string& name) -> binary_data {
        auto bin = binaries.find(name);

        if (bin != binaries.end())
            return bin->second;

        auto f = files.find(name);

        if (f != files.end()) {
            fs::path p(f->second);

            auto r = binary_readers.find(p.extension().string());

            if (r != binary_readers.end()) {
                auto dat = binary_data{nullptr, 0};

                auto ret = r->second(SDL_RWFromFile(f->second.c_str(), "r"), dat);

                if (ret != 0) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {nullptr, 0};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                binaries.insert({p.filename().string(), dat});
                return dat;
            }
        }

        return {nullptr, 0};
    }
} // namespace assets
