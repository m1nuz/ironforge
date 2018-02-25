#include <chrono>
#include <experimental/filesystem>

#include <core/journal.hpp>
#include <core/assets.hpp>
#include <core/game.hpp>

#include <readers/readers.hpp>

namespace fs = std::experimental::filesystem;

namespace assets {

    std::mutex      text_mutex;

    static auto is_readable(const instance_t &inst, const std::string &ext) -> bool {
        if (inst.binary_readers.find(ext) != inst.binary_readers.end())
            return true;

        if (inst.text_readers.find(ext) != inst.text_readers.end())
            return true;

        if (inst.image_readers.find(ext) != inst.image_readers.end())
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

    static auto get_textdata(instance_t &inst, const std::string &name) -> text_data_t {
        using namespace game;

        if (const auto f = inst.all_files.find(name); f != inst.all_files.end()) {
            fs::path p(f->second);

            if (const auto r = inst.text_readers.find(p.extension().string()); r != inst.text_readers.end()) {
                auto res = r->second(inst, SDL_RWFromFile(f->second.c_str(), "r"));
                if (!res) {
                    journal::error(journal::_SYSTEM, "Can't read file %", f->second);
                    return {};
                }

                journal::debug(journal::_GAME, "Read file %", f->second);
                return res.value();
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

    auto append(instance_t &inst, const std::string &ext, binary_reader_t reader) -> bool {
        if (!reader || ext.empty())
            return false;

        inst.binary_readers.emplace(ext, reader);

        return true;
    }

    auto append(instance_t &inst, const std::string &ext, text_reader_t reader) -> bool {
        if (!reader || ext.empty())
            return false;

        inst.text_readers.emplace(ext, reader);

        return true;
    }

    auto append(instance_t &inst, const std::string &ext, image_reader_t reader) -> bool {
        if (!reader || ext.empty())
            return false;

        inst.image_readers.emplace(ext, reader);

        return true;
    }

    auto open(instance_t &inst, const std::string& path) -> bool {
        using namespace game;

        fs::path p(path);

        if (!fs::exists(p))
            return false;

        if (fs::is_directory(p)) {
            for (auto& entry : fs::recursive_directory_iterator(p))
                if (fs::is_regular_file(entry.path())) {

                    if (!is_readable(inst, entry.path().extension().string()))
                        continue;

                    journal::debug(journal::_GAME, "Asset found %", entry.path().string());

                    inst.all_files.emplace(entry.path().filename().string(), entry.path().string());
                }
        }

        return true;
    }

    auto process(instance_t &inst) -> void {
        // TODO: process file queue, thread loading
    }

    auto cleanup(instance_t &inst) -> void {
        inst.active = false;
    }

    auto get_text(instance_t &inst, std::string_view name) -> std::optional<text_data_t> {
        const auto _name = std::string{name};

        auto t = inst.texts.find(_name);

        if (t != inst.texts.end())
            return t->second;

        auto f = inst.all_files.find(_name);

        if (f != inst.all_files.end()) {
            fs::path p(f->second);

            auto r = inst.text_readers.find(p.extension().string());

            if (r != inst.text_readers.end()) {
                auto res = r->second(inst, SDL_RWFromFile(f->second.c_str(), "r"));
                if (!res) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                inst.texts.insert({p.filename().string(), res.value()});
                return res.value();
            }
        }        

        game::journal::warning(game::journal::_GAME, "File '%' not found", _name);

        return {};
    }

    /*auto get_text_absolute(const std::string& path) -> text_data {
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
    }*/

    auto get_image(instance_t &inst, std::string_view name) -> std::optional<image_data_t> {
        const auto _name = std::string{name};

        auto im = inst.images.find(_name);

        if (im != inst.images.end())
            return im->second;

        auto f = inst.all_files.find(_name);

        if (f != inst.all_files.end()) {
            fs::path p(f->second);

            auto r = inst.image_readers.find(p.extension().string());

            if (r != inst.image_readers.end()) {

                auto res = r->second(inst, SDL_RWFromFile(f->second.c_str(), "r"));
                if (!res) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                inst.images.insert({p.filename().string(), res.value()}); // TODO: free memory
                return res.value();
            }
        }

        return {};
    }

    auto get_binary(instance_t &inst, std::string_view name) -> std::optional<binary_data_t> {
        const auto _name = std::string{name};

        auto bin = inst.binaries.find(_name);

        if (bin != inst.binaries.end())
            return bin->second;

        auto f = inst.all_files.find(_name);

        if (f != inst.all_files.end()) {
            fs::path p(f->second);

            auto r = inst.binary_readers.find(p.extension().string());

            if (r != inst.binary_readers.end()) {
                auto res = r->second(inst, SDL_RWFromFile(f->second.c_str(), "r"));

                if (!res) {
                    game::journal::error(game::journal::_SYSTEM, "Can't read file %", f->second);
                    return {};
                }

                game::journal::debug(game::journal::_GAME, "Read file %", f->second);
                inst.binaries.insert({p.filename().string(), res.value()});
                return res.value();
            }
        }

        return {};
    }

    auto get_text(instance_t &inst, std::string_view name, text_responce cb) -> void {
        std::string _name{name};

        if (auto it = inst.texts.find(_name); it != inst.texts.end()) {
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
