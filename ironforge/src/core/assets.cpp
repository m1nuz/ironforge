#include <core/application.hpp>
#include <core/assets.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <readers/readers.hpp>

namespace fs = boost::filesystem;

namespace assets {
    _default_readers::_default_readers() {
        text_readers.push_back(std::make_tuple(".vert", read_shader_text));
        text_readers.push_back(std::make_tuple(".frag", read_shader_text));
        text_readers.push_back(std::make_tuple(".glsl", read_shader_text));
        text_readers.push_back(std::make_tuple(".lua", read_text));
        text_readers.push_back(std::make_tuple(".scene", read_text));
        image_readers.push_back(std::make_tuple(".tga", read_targa));
    }

    _default_readers default_readers;

    struct data_source {
        std::string filepath;
    };

    // ext, reader
    std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, binary_data &)>> binary_readers;
    std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, text_data &)>>   text_readers;
    std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, image_data &)>>  image_readers;

    // name, data
    std::unordered_map<std::string, binary_data>    binaries;
    std::unordered_map<std::string, text_data>      texts;
    std::unordered_map<std::string, image_data>     images;

    // name, path
    std::unordered_map<std::string, data_source> files;

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

    auto append(const std::string& ext, std::function<int32_t (SDL_RWops *rw, binary_data &)> reader) -> result {
        if (!reader || ext.empty())
            return result::failure;

        binary_readers.insert({ext, reader});

        return result::success;
    }

    auto append(const std::string& ext, std::function<int32_t (SDL_RWops *rw, text_data &)> reader) -> result {
        if (!reader || ext.empty())
            return result::failure;

        text_readers.insert({ext, reader});

        return result::success;
    }

    auto append(const std::string& ext, std::function<int32_t (SDL_RWops *rw, image_data &)> reader) -> result {
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

                    application::debug(application::log_category::application, "Asset found %\n", entry.path());

                    files.insert({entry.path().filename().string(), {entry.path().string()}});
                }
        }

        return result::success;
    }

    auto get_text(const std::string& name) -> text_data {
        auto t = texts.find(name);

        if (t != texts.end())
            return t->second;

        auto f = files.find(name);

        if (f != files.end()) {
            fs::path p(f->second.filepath);

            auto r = text_readers.find(p.extension().string());

            if (r != text_readers.end()) {
                auto td = text_data{nullptr, 0};

                auto ret = r->second(SDL_RWFromFile(f->second.filepath.c_str(), "r"), td);
                if (ret != 0) {
                    application::error(application::log_category::system, "Can't read file %\n", f->second.filepath);
                    return {nullptr, 0};
                }

                application::debug(application::log_category::application, "Read file %\n", f->second.filepath);
                texts.insert({p.filename().string(), td}); // TODO: free memory
                return td;
            }

            return {nullptr, 0};
        }

        return {nullptr, 0};
    }
} // namespace assets
