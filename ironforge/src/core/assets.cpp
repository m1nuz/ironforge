#include <chrono>
#include <experimental/filesystem>

#include <core/journal.hpp>
#include <core/assets.hpp>
#include <core/game.hpp>

#include <readers/readers.hpp>

namespace fs = std::experimental::filesystem;

namespace assets {

    std::mutex      text_mutex;

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

    static auto is_readable(const instance_t &inst, const std::string &ext) -> bool {
        if (inst.binary_readers.find(ext) != binary_readers.end())
            return true;

        if (inst.text_readers.find(ext) != text_readers.end())
            return true;

        if (inst.image_readers.find(ext) != image_readers.end())
            return true;

        return false;
    }

    auto create_default_readers() -> readers {
        readers rs;

        rs.text_readers.emplace(".vert", read_shader_text);
        rs.text_readers.emplace(".frag", read_shader_text);
        rs.text_readers.emplace(".glsl", read_shader_text);
        rs.text_readers.emplace(".lua", read_text);
        rs.text_readers.emplace(".scene", read_text);
        rs.text_readers.emplace(".txt", read_text);
        rs.image_readers.emplace(".tga", read_targa);
        rs.binary_readers.emplace(".ttf", read_binary);

        return rs;
    }

    static auto append(instance_t &inst, const readers &rs) -> bool{
        auto res = false;

        for (auto & r : rs.binary_readers)
            if ((res = append(inst, std::get<0>(r), std::get<1>(r))) != true)
                return res;

        for (auto & r : rs.text_readers)
            if ((res = append(inst, std::get<0>(r), std::get<1>(r))) != true)
                return res;

        for (auto & r : rs.image_readers)
            if ((res = append(inst, std::get<0>(r), std::get<1>(r))) != true)
                return res;

        return true;
    }

    static auto get_textdata(instance_t &inst, const std::string &name) -> text_t {
        using namespace game;

        if (const auto f = inst.all_files.find(name); f != inst.all_files.end()) {
            fs::path p(f->second);

            if (const auto r = inst.text_readers.find(p.extension().string()); r != inst.text_readers.end()) {
                auto td = text_data{nullptr, 0};

                auto ret = r->second(SDL_RWFromFile(f->second.c_str(), "r"), td);
                if (ret != 0) {
                    journal::error(journal::_SYSTEM, "Can't read file %", f->second);
                    return text_t{};
                }

                journal::debug(journal::_GAME, "Read file %", f->second);
                return text_t{{td.text, td.size}};
            }
        }
    }

    auto process_load(instance_t &inst) -> void {
        using namespace std::chrono_literals;

        while (inst.active) {

            {
                std::lock_guard lock(text_mutex);

                for (auto& [name, ti] : inst.text_processed) {
                    if (ti.ready)
                        continue;

                    auto data = get_textdata(inst, name);
                    ti.ready = true;

                    for (auto cb : ti.responces)
                        cb(data);
                }
            }

            std::this_thread::sleep_for(10ms);
        }
    }

    [[nodiscard]] auto create_instance(const readers &rs) -> instance_result {
        instance_t inst;
        inst.binary_readers = rs.binary_readers;
        inst.image_readers = rs.image_readers;
        inst.text_readers = rs.text_readers;

        //TODO: remove
        instance_t __empty;
        append(__empty, rs);

        //inst.load_thread = std::thread{process_load, std::ref()};

        return inst;//instance_t{rs.binary_readers, rs.image_readers, rs.text_readers};
    }

    auto append(instance_t &inst, const std::string &ext, std::function<int32_t (SDL_RWops *rw, binary_data &)> reader) -> bool {
        if (!reader || ext.empty())
            return false;

        binary_readers.insert({ext, reader});
        inst.binary_readers.emplace(ext, reader);

        return true;
    }

    auto append(instance_t &inst, const std::string &ext, std::function<int32_t (SDL_RWops *rw, text_data &)> reader) -> bool {
        if (!reader || ext.empty())
            return false;

        text_readers.insert({ext, reader});
        inst.text_readers.emplace(ext, reader);

        return true;
    }

    auto append(instance_t &inst, const std::string &ext, std::function<int32_t (SDL_RWops *rw, image_data &)> reader) -> bool {
        if (!reader || ext.empty())
            return false;

        image_readers.insert({ext, reader});
        inst.image_readers.emplace(ext, reader);

        return true;
    }

    auto open(instance_t &inst, const std::string& path) -> result {
        using namespace game;

        fs::path p(path);

        if (!fs::exists(p))
            return result::failure; // TODO: make error

        if (fs::is_directory(p)) {
            for (auto& entry : fs::recursive_directory_iterator(p))
                if (fs::is_regular_file(entry.path())) {

                    if (!is_readable(inst, entry.path().extension().string()))
                        continue;

                    journal::debug(journal::_GAME, "Asset found %", entry.path().string());

                    files.insert({entry.path().filename().string(), entry.path().string()});
                    inst.all_files.emplace(entry.path().filename().string(), entry.path().string());
                }
        }

        return result::success;
    }

    auto process(instance_t &inst) -> void {
        // TODO: process file queue, thread loading
    }

    auto cleanup(instance_t &inst) -> void {
        inst.active = false;
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
                auto imd = image_data{0, 0, 0, video::pixel_format::unknown, {}};

                auto ret = r->second(SDL_RWFromFile(f->second.c_str(), "r"), imd);
                if (ret != 0) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {0, 0, 0, video::pixel_format::unknown, {}};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                images.insert({p.filename().string(), imd}); // TODO: free memory
                return imd;
            }
        }

        return {0, 0, 0, video::pixel_format::unknown, {}};
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

    auto get_text(instance_t &inst, std::string_view name, text_responce cb) -> void {
        std::string _name{name};

        if (auto it = inst.all_texts.find(_name); it != inst.all_texts.end()) {
            cb(it->second);
            return;
        }

        std::lock_guard lock(text_mutex);
        if (auto it = inst.text_processed.find(_name); it == inst.text_processed.end()) {
            text_info ti;
            ti.ready = false;
            ti.responces.push_back(cb);
            inst.text_processed.insert({_name, ti});
        }
    }
} // namespace assets
