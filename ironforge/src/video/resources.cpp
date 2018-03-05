#include <cstddef>
#include <algorithm>
#include <utility/hash.hpp>
#include <core/journal.hpp>
#include <core/assets.hpp>
#include <video/video.hpp>
#include <video/glyphs.hpp>
#include <utility/thread_pool.hpp>

namespace video {
    auto init_resources(instance_t &inst, assets::instance_t &asset, const std::vector<font_info> &fonts) -> void {
        using namespace game;

        inst.textures.reserve(100);
        inst.programs.reserve(100);
        inst.arrays.reserve(100);
        inst.buffers.reserve(100);

        uint32_t textures_flags = 0;
        switch (inst.texture_filter) {
        case texture_filtering::bilinear:
            break;
        case texture_filtering::trilinear:
        case texture_filtering::anisotropic:
            textures_flags |= static_cast<uint32_t>(texture_flags::auto_mipmaps);
            break;
        }

        journal::debug(journal::_VIDEO, "%", "Init resources");

        inst.textures.emplace("white-map", gl::create_texture_2d(imgen::make_color(128, 128, {255, 255, 255}), textures_flags));
        inst.textures.emplace("black-map", gl::create_texture_2d(imgen::make_color(128, 128, {0, 0, 0}), textures_flags));
        inst.textures.emplace("check-map", gl::create_texture_2d(imgen::make_check(128, 128, 0x10, {24, 24, 24}), textures_flags));
        inst.textures.emplace("red-map", gl::create_texture_2d(imgen::make_color(128, 128, {255, 0, 0}), textures_flags));


        const int asz = 1024;

        auto white_im = video::imgen::make_color(64, 64, {255, 255, 255});
        auto ui_atlas = video::create_atlas(asz, asz, 1);
        auto rc = video::insert_image(ui_atlas, white_im);
        video::glyph_cache_build(inst, asset, fonts, ui_atlas);
        video::make_texture_2d(inst, "glyphs-map", get_atlas_texture(ui_atlas), static_cast<uint32_t>(video::texture_flags::auto_mipmaps));
        //auto ui_rc = glm::vec4{rc.x / (float)asz, rc.y / (float)asz, rc.w / (float)asz, rc.h / (float)asz};
    }

    auto cleanup_resources(instance_t &in) -> void {
        using namespace game;

        for (auto &b : in.buffers)
            gl::destroy_buffer(b);

        for (auto &a : in.arrays)
            gl::destroy_vertex_array(a);

        for (auto &[name, t] : in.textures) {
            journal::debug(journal::_VIDEO, "Destroy texture %", name);
            gl::destroy_texture(t);
        }

        for (auto &[name, p] : in.programs) {
            journal::debug(journal::_VIDEO, "Destroy program %", name);
            gl::destroy_program(p);
        }
    }

    auto process(assets::instance_t &asset, instance_t &inst) -> void {
        /*uint32_t textures_flags = 0;
        switch (inst.tex_filtering) {
        case texture_filtering::bilinear:
            break;
        case texture_filtering::trilinear:
        case texture_filtering::anisotropic:
            textures_flags |= static_cast<uint32_t>(texture_flags::auto_mipmaps);
            break;
        }

        for (auto &t : textures) {
            if (!t.ready)
                if (t.imd_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    t.tex = gl::create_texture_2d(t.imd_future.get(), textures_flags);
                    t.ready = true;
                    t.tex.desc = &t;
                    game::journal::debug(game::journal::_VIDEO, "%", "TEX LOADED");
                    // FIXME: ok, for now
                    //t.imd_future.get().pixels = NULL;
                }
        }*/
    }

    /*auto make_texture_2d(const texture_info &info) -> texture {
        texture_desc desc;
        desc.tex = gl::create_texture_2d(info);
        desc.name_hash = 0;
        desc.usage = 0;

        textures.push_back(desc);

        return desc.tex;
    }

    auto make_texture_2d(const image_data &data) -> texture {
        return make_texture_2d({}, data);
    }*/

    auto create_texture(assets::instance_t &asset, instance_t &inst, const json &info) -> texture {
        using namespace game;
        using namespace std;

        auto textures_flags = static_cast<uint32_t>(video::texture_flags::auto_mipmaps);

        switch (inst.texture_filter) {
        case texture_filtering::bilinear:
            break;
        case texture_filtering::trilinear:
        case texture_filtering::anisotropic:
            textures_flags |= static_cast<uint32_t>(texture_flags::auto_mipmaps);
            break;
        default:
            break;
        }

        const auto name = info.find("name") != info.end() ? info["name"].get<string>() : string{};
        const auto type = info.find("type") != info.end() ? info["type"].get<string>() : string{};

        if (auto it = inst.textures.find(name); it != inst.textures.end())
            return it->second;

        if (type == "2d") {
            const auto levels = info.find("levels") != info.end() ? info["levels"].get<vector<string>>() : vector<string>{};

            if (levels.empty()) {
                journal::error(journal::_VIDEO, "No levels for texture %", name);
                return {};
            }

            const auto texture_name = levels.size() < inst.texture_level ? levels.back() : levels[inst.texture_level];

            auto imd = assets::get_image(asset, texture_name);

            if (!imd) {
                journal::warning(journal::_GAME, "Texture % not found", name);
                return {};
            }

            auto tex = gl::create_texture_2d(imd.value(), textures_flags);
            inst.textures.emplace(name, tex);

            journal::info(journal::_VIDEO, "Create texture '%'", name);

            return tex;
        }

        if (type == "cubemap") {
            const auto levels = info.find("levels") != info.end() ? info["levels"].get<vector<vector<string>>>() : vector<vector<string>>{};

            if (levels.empty()) {
                journal::error(journal::_VIDEO, "No levels for texture %", name);
                return {};
            }

            const auto level = levels.size() < inst.texture_level ? levels.back() : levels[inst.texture_level];

            if (level.empty()) {
                journal::error(journal::_VIDEO, "No levels for texture %", name);
                return {};
            }

            if (level.size() != 6) {
                journal::error(journal::_VIDEO, "Not enough sides for texture %", name);
                return {};
            }

            image_data images[6];

            for (size_t i = 0; i < 6; i++) {
                auto img = assets::get_image(asset, level[i]);

                if (img)
                    images[i] = img.value();

                // TODO: make error
            }

            auto tex = gl::create_texture_cube(images, textures_flags);
            inst.textures.emplace(name, tex);

            journal::info(journal::_VIDEO, "Create texture '%'", name);

            return tex;
        }

        journal::error(journal::_VIDEO, "Unknown texture type '%'", type);

        return {};
    }

    auto create_program(assets::instance_t &asset, instance_t &inst, const json &info) -> program {
        using namespace game;
        using namespace std;

        const auto name = info.find("name") != info.end() ? info["name"].get<string>() : string{};
        const auto programs = info.find("programs") != info.end() ? info["programs"].get<vector<string>>() : vector<string>{};

        if (!programs.empty()) {
            std::vector<gl::shader_source> sources;

            for (const auto &p : programs) {
                auto ps = assets::get_text(asset, p);

                if (!ps)
                    continue;

                gl::shader_source source;
                source.name = p;
                source.text = ps.value();

                sources.push_back(source);
            }

            if (!sources.empty()) {
                gl::program_info pi;
                pi.name = name;
                pi.sources = sources;

                auto p = gl::create_program(pi);
                inst.programs.emplace(name, p);

                journal::info(journal::_VIDEO, "Create program '%'", name);

                return p;
            }

            journal::error(journal::_VIDEO, "Empty shader sources '%'", name);

            return {};
        }

        journal::error(journal::_VIDEO, "Empty shader programs '%'", name);

        return {};
    }

    static auto create_vertices_info(assets::instance_t &asset, const json &info) -> std::optional<vertices_info> {
        using namespace game;
        using namespace std;

        const auto type = info.find("type") != info.end() ? info["type"].get<string>() : string{};

        if (type == "gen_sphere") {
            const auto radius = info.find("radius") != info.end() ? info["radius"].get<float>() : 1.f;
            const auto rings = info.find("rings") != info.end() ? info["rings"].get<uint32_t>() : 8;
            const auto sectors = info.find("sectors") != info.end() ? info["sectors"].get<uint32_t>() : 8;

            const auto sphere_info = gen_sphere_info{rings, sectors, radius};

            return vertgen::make_sphere(&sphere_info, glm::mat4(1.f));
        }

        if (type == "gen_cube") {
            //const auto cube_info = gen_cube_info{1.f};

            return vertgen::make_cube(glm::mat4(1.f));
        }

        if (type == "gen_plane") {
            return vertgen::make_plane(glm::mat4{1.f});
        }

        if (type == "gen_grid") {
            const auto horizontal_extend = info["horizontal_extend"].get<float>();
            const auto vertical_extend = info["vertical_extend"].get<float>();
            const auto rows = info["rows"].get<uint32_t>();
            const auto columns = info["columns"].get<uint32_t>();
            const auto triangle_strip = info["triangle_strip"].get<bool>();
            //const auto height_map = msh["height_map"].get<string>();

            video::heightmap_t height_map;

            const auto grid_info = gen_grid_plane_info{horizontal_extend, vertical_extend, rows, columns, triangle_strip, height_map};

            return vertgen::make_grid_plane(&grid_info, glm::mat4(1.f), height_map);
        }

        if (type == "file") {
            return {};
        }

        return {};
    }

    auto create_mesh(assets::instance_t &asset, instance_t &vi, const json &info) -> std::optional<mesh> {
        using namespace game;
        using namespace std;

        const auto type = info.find("type") != info.end() ? info["type"].get<string>() : string{};

        auto vsi = create_vertices_info(asset, info);
        if (!vsi) {
            journal::error(journal::_VIDEO, "%", "Can't create vertices for mesh");
            return {};
        }

        mesh m;
        std::vector<vertices_draw> draws;

        m.desc = vsi.value().desc;
        m.source = make_vertices_source(vi, {vsi.value().data}, vsi.value().desc, draws);
        m.draw = draws[0];

        journal::info(journal::_VIDEO, "Create mesh '%'", type);

        return m;
    }

    auto make_texture_2d(instance_t &vi, const std::string &name, const image_data &data, const uint32_t flags) -> texture {
        auto tex = gl::create_texture_2d(data, flags);
        vi.textures.emplace(name, tex);
        return tex;
    }

    /*auto make_texture_cube(assets::instance_t &asset, const std::string &name, const std::string (&names)[6]) -> texture {
        image_data images[6];

        for (size_t i = 0; i < 6; i++) {
            auto img = assets::get_image(asset, names[i]);

            if (img)
                images[i] = img.value();

            // TODO: make error
        }

        texture_desc desc;
        desc.tex = video::gl::create_texture_cube(images);
        desc.name = name;
        desc.name_hash = utils::xxhash64(name);
        desc.usage = 0;
        desc.ready = true;

        textures.push_back(desc);

        return desc.tex;
    }*/

    // TODO: make return value optional
    auto make_vertices_source(instance_t &vi, const std::vector<vertices_data> &data, const vertices_desc &desc, std::vector<vertices_draw> &draws) -> vertices_source {
        struct buffer_size_info {
            size_t vb_size;
            size_t ib_size;
        };

        // TODO: check data.size()

        draws.reserve(data.size());

        std::vector<buffer_size_info> buffers_info;
        buffers_info.reserve(data.size());

        uint32_t vertices_data_size = 0;
        uint32_t indices_data_size = 0;
        uint32_t base_vertex = 0;
        uint32_t base_index = 0;

        for (const auto &vd : data) {
            size_t vb_size = 0; // size for vertex data
            // count vertex buffers size and other draw stuff
            switch (desc.vf) {
            case vertex_format::v3t2n3:
                vb_size = vd.vertices_num * sizeof(v3t2n3);
                vertices_data_size += vb_size;
                break;
            case vertex_format::v3t2c4:
                vb_size = vd.vertices_num * sizeof(v3t2c4);
                vertices_data_size += vb_size;
                break;
            case vertex_format::v3t2n3t3:
                vb_size = vd.vertices_num * sizeof(v3t2n3t3);
                vertices_data_size += vb_size;
                break;
            default:
                game::journal::warning(game::journal::_VIDEO, "%", "Unknown vertex format");
                break;
            }

            size_t ib_size = 0; // size for index data(of vd)
            // count index buffers size
            switch (desc.ef) {
            case index_format::ui16:
                ib_size = vd.indices_num * sizeof(uint16_t);
                indices_data_size += ib_size;
                break;
            case index_format::ui32:
                ib_size = vd.indices_num * sizeof(uint32_t);
                indices_data_size += ib_size;
                break;
            default:
                game::journal::warning(game::journal::_VIDEO, "%", "Unknown index format");
                break;
            }

            buffers_info.push_back({vb_size, ib_size});

            const uint32_t count = vd.indices_num ? vd.indices_num : vd.vertices_num; // vertices_num never should be 0
            draws.push_back({desc.primitive, 0, 0, count, base_vertex, base_index}); // offsets calcs later

            base_vertex += vd.vertices_num;
            base_index += vd.indices_num;
        }

        // allocate memory buffers
        void *vertex_data = malloc(vertices_data_size); // TODO: check mem allocation
        memset(vertex_data, 0, vertices_data_size);

        void *index_data = nullptr;
        if (indices_data_size > 0)
        {
            index_data = malloc(indices_data_size);
            memset(index_data, 0, indices_data_size);
        }

        ptrdiff_t vb_offset = 0;
        ptrdiff_t ib_offset = 0;

        // copy buffers to one buffer
        for (size_t i = 0; i < data.size(); i++) {
            draws[i].vb_offset = vb_offset;
            if (data[i].vertices)
                memcpy((char*)vertex_data + vb_offset, data[i].vertices, buffers_info[i].vb_size);
            vb_offset += buffers_info[i].vb_size;

            if (index_data) {
                draws[i].ib_offset = ib_offset;
                memcpy((char*)index_data + ib_offset, data[i].indices, buffers_info[i].ib_size);
                ib_offset += buffers_info[i].ib_size;
            }
        }

        auto va = gl::create_vertex_array();
        vi.arrays.push_back({va});

        gl::bind_vertex_array(va);

        // TODO : seaarch other buffer with same hash
        auto vb = gl::create_buffer(gl::buffer_target::array, vertices_data_size, vertex_data, static_cast<gl::buffer_usage>(desc.vb_usage));
        vi.buffers.push_back(vb);

        // transfer to video memory
        switch (desc.vf) {
        case vertex_format::v3t2n3:
            gl::vertex_array_buffer(va, vb, 0, sizeof(v3t2n3));
            gl::vertex_array_format(va, vertex_attributes::position, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3, position));
            gl::vertex_array_format(va, vertex_attributes::texcoord, 2, gl::attrib_type::float_value, false, offsetof(v3t2n3, texcoord));
            gl::vertex_array_format(va, vertex_attributes::normal, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3, normal));
            break;
        case vertex_format::v3t2c4:
            gl::vertex_array_buffer(va, vb, 0, sizeof(v3t2c4));
            gl::vertex_array_format(va, vertex_attributes::position, 3, gl::attrib_type::float_value, false, offsetof(v3t2c4, position));
            gl::vertex_array_format(va, vertex_attributes::texcoord, 2, gl::attrib_type::float_value, false, offsetof(v3t2c4, texcoord));
            gl::vertex_array_format(va, vertex_attributes::color, 4, gl::attrib_type::float_value, false, offsetof(v3t2c4, color));
            break;
        case vertex_format::v3t2n3t3:
            gl::vertex_array_buffer(va, vb, 0, sizeof(v3t2n3t3));
            gl::vertex_array_format(va, vertex_attributes::position, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3t3, position));
            gl::vertex_array_format(va, vertex_attributes::texcoord, 2, gl::attrib_type::float_value, false, offsetof(v3t2n3t3, texcoord));
            gl::vertex_array_format(va, vertex_attributes::normal, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3t3, normal));
            gl::vertex_array_format(va, vertex_attributes::tangent, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3t3, tangent));
            break;
        default:
            game::journal::warning(game::journal::_VIDEO, "%", "Unknown vertex format");
            break;
        }

        // TODO : seaarch other buffer with same hash
        gl::buffer eb;
        memset(&eb, 0, sizeof eb);
        if (index_data)
        {
            eb = gl::create_buffer(gl::buffer_target::element_array, indices_data_size, index_data, static_cast<gl::buffer_usage>(desc.eb_usage));
            vi.buffers.push_back(eb);
        }

        gl::unbind_vertex_array(va);

        free(vertex_data);
        free(index_data);

        return {va, vb, eb};
    }

    auto get_texture(instance_t &vi, const std::string &name) -> texture {
        using namespace game;

        if (auto it = vi.textures.find(name); it != vi.textures.end()) {
            return it->second;
        }

        journal::error(journal::_VIDEO, "Texture % not found", name);

        return {};
    }

    auto get_heightmap(const std::string &name) -> heightmap_t {
        UNUSED(name);

        return {};
    }

    /*auto make_program(assets::instance_t &asset, const gl::program_info &info) -> program {
        auto inf = info;

        std::vector<uint64_t> hashes;
        hashes.reserve(3); // types of shader

        for (auto &i : inf.sources) {
            // just read from file to text
            if (!i.name.empty() && i.text.empty()) {
                i.text = assets::get_text(asset, i.name).value_or(std::string{});

                hashes.push_back(utils::xxhash64(i.text));
            }
        }

        auto bytes = hashes.size() * (hashes.size() > 0 ? sizeof (hashes[0]) : 0);
        auto hash = utils::xxhash64(&hashes[0], bytes);

        auto pi = std::find_if(programs.begin(), programs.end(), [hash](const program_desc &desc) {
            if (desc.hash == hash)
                return true;
            return false;
        });

        if (pi != programs.end()) {
            game::journal::warning(game::journal::_VIDEO, "Program % already created", pi->pro.pid);
            return pi->pro;
        }

        auto p = gl::create_program(inf);
        programs.push_back({p, 1, hash, utils::xxhash64(info.name)});

        return p;
    }*/

    /*auto get_shader(const char *name) -> program {
        auto hash = utils::xxhash64(name, strlen(name));

        auto it = std::find_if(programs.begin(), programs.end(), [hash](const program_desc &desc) {
            if (desc.name_hash == hash)
                return true;
            return false;
        });

        if (it != programs.end())
            return (*it).pro;

        return {0, {}, {}, {}};
    }*/

    auto get_shader(instance_t &vi, const std::string &name) -> program {
        using namespace game;

        if (auto it = vi.programs.find(name); it != vi.programs.end())
            return it->second;

        journal::error(journal::_VIDEO, "Shader % not found", name);

        return {};
    }
} // namespace video
