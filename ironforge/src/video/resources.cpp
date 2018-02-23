#include <cstddef>
#include <algorithm>
#include <utility/hash.hpp>
#include <core/journal.hpp>
#include <core/assets.hpp>
#include <video/video.hpp>
#include <utility/thread_pool.hpp>

// this api depend from used general video api
namespace video {
    struct texture_desc {
        std::string name;
        gl::texture tex;
        uint32_t    usage;
        uint64_t    hash;
        uint64_t    name_hash;
        bool        ready; // TODO: make atomic
        std::shared_future<image_data> imd_future;

        // std::vector<gl::texture> lods;
        // std::vector<std::string> names;
        // std::vector<uint64_t> hashes;
        // std::vector<uint64_t> name_hashes;
        // std::vector<bool> ready;
        // std::vector<std::shared_future<image_data>> futures;
    };

    struct buffer_desc {
        gl::buffer  buf;
        uint32_t    usage;
        uint64_t    hash;
    };

    struct vertex_array_desc {
        gl::vertex_array va;
    };

    struct program_desc {
        gl::program pro;
        uint32_t    usage;
        uint64_t    hash;
        uint64_t    name_hash;
    };

    // TODO: make resource cash instance
    std::vector<texture_desc>       textures;
    std::vector<buffer_desc>        buffers;
    std::vector<vertex_array_desc>  vertex_arrays;
    std::vector<program_desc>       programs;

    utils::thread_pool              pool;

    auto init_resources(instance_t &inst) -> void {
        textures.reserve(100);
        programs.reserve(100);

        uint32_t textures_flags = 0;
        switch (inst.tex_filtering) {
        case texture_filtering::bilinear:
            break;
        case texture_filtering::trilinear:
        case texture_filtering::anisotropic:
            textures_flags |= static_cast<uint32_t>(texture_flags::auto_mipmaps);
            break;
        }

        game::journal::debug(game::journal::_VIDEO, "%", "Init resources");

        const auto white_name = "white-map";
        auto im = video::imgen::make_color(128, 128, {255, 255, 255}); // white
        textures.push_back({white_name, gl::create_texture_2d(im, textures_flags), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(white_name, strlen(white_name)), true});

        const auto black_name = "black-map";
        im = video::imgen::make_color(128, 128, {0, 0, 0}); // black
        textures.push_back({black_name, gl::create_texture_2d(im, textures_flags), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(black_name, strlen(black_name)), true});

        const auto check_name = "check-map";
        im = video::imgen::make_check(128, 128, 0x10, {255, 255, 255}); // check
        textures.push_back({check_name, gl::create_texture_2d(im, textures_flags), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(check_name, strlen(check_name)), true});

        const auto red_name = "red-map";
        im = video::imgen::make_color(128, 128, {255, 0, 0}); // white
        textures.push_back({red_name, gl::create_texture_2d(im, textures_flags), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(white_name, strlen(white_name)), true});

        make_program({"emission-shader", {{"forward-emission.vert", {}}, {"forward-emission.frag", {}}}});
        make_program({"ambient-light-shader", {{"forward-ambient.vert", {}}, {"forward-ambient.frag", {}}}});
        make_program({"forward-directional-shader", {{"forward-directional.vert", {}}, {"forward-directional.frag", {}}}});
        make_program({"postprocess-shader", {{"screenspace.vert", {}}, {"postprocess_final.frag", {}}}});
        make_program({"hblur-shader", {{"screenspace.vert", {}}, {"filter-hblur.frag", {}}}});
        make_program({"vblur-shader", {{"screenspace.vert", {}}, {"filter-vblur.frag", {}}}});
        make_program({"skybox-shader", {{"skybox.vert", {}}, {"skybox.frag", {}}}});
        make_program({"sprite-shader", {{"sprite.vert", {}}, {"sprite.frag", {}}}});
        make_program({"terrain-shader", {{"terrain.vert", {}}, {"terrain.frag", {}}}});
    }

    auto cleanup_resources() -> void {
        for (auto &td : textures)
            gl::destroy_texture(td.tex);

        for (auto &bd : buffers)
            gl::destroy_buffer(bd.buf);

        for (auto &arr : vertex_arrays)
            gl::destroy_vertex_array(arr.va);

        for (auto &p : programs)
            gl::destroy_program(p.pro);

        textures.clear();
        buffers.clear();
        vertex_arrays.clear();
        programs.clear();
    }

    auto process(instance_t &inst) -> void {
        uint32_t textures_flags = 0;
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
        }
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

    auto make_texture_2d(const std::string &name, const image_data &data, const uint32_t flags) -> texture {
        texture_desc desc;
        desc.tex = gl::create_texture_2d(data, flags);
        desc.name_hash = name.empty() ? 0 : utils::xxhash64(name);
        desc.hash = 0; // TODO: calc it
        desc.usage = 0;
        desc.ready = true;

        textures.push_back(desc);

        return desc.tex;
    }

    auto make_texture_cube(const std::string &name, const std::string (&names)[6]) -> texture {
        image_data images[6];

        for (size_t i = 0; i < 6; i++) {
            images[i] = assets::get_image(names[i]);

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
    }

    // TODO: make return value optional
    auto make_vertices_source(const std::vector<vertices_data> &data, const vertices_desc &desc, std::vector<vertices_draw> &draws) -> vertices_source {
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
        vertex_arrays.push_back({va});

        gl::bind_vertex_array(va);

        // TODO : seaarch other buffer with same hash
        auto vb = gl::create_buffer(gl::buffer_target::array, vertices_data_size, vertex_data, static_cast<gl::buffer_usage>(desc.vb_usage));
        buffers.push_back({vb, 0, utils::xxhash64(vertex_data, vertices_data_size)});

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
            buffers.push_back({eb, 0, utils::xxhash64(index_data, indices_data_size)});
        }

        gl::unbind_vertex_array(va);

        free(vertex_data);
        free(index_data);

        return {va, vb, eb};
    }

    auto get_texture(const char *name, const texture &default_tex) -> texture {
        uint32_t textures_flags = static_cast<uint32_t>(video::texture_flags::auto_mipmaps);
        /*switch (inst.tex_filtering) {
        case texture_filtering::bilinear:
            break;
        case texture_filtering::trilinear:
        case texture_filtering::anisotropic:
            textures_flags |= static_cast<uint32_t>(video::texture_flags::auto_mipmaps);
            break;
        }*/

        const auto hash = utils::xxhash64(name, strlen(name));
        auto it = std::find_if(textures.begin(), textures.end(), [hash](const texture_desc &td) {
            return td.name_hash == hash;
        });

        if (it != textures.end()) {
            game::journal::info(game::journal::_GAME, "Texture % found", name);
            return it->tex;
        }

        auto imd = /*image_future.get();/*/assets::get_image(name);

        if (imd.pixels.empty()) {
            game::journal::warning(game::journal::_GAME, "Texture % not found", name);
            return default_tex;
        }

        return make_texture_2d(name, imd, textures_flags);

        // TODO: fix texture striming
        /*texture_desc desc;
        desc.name = name;
        desc.tex = default_check_texture();
        desc.name_hash = utils::xxhash64(name, strlen(name));//name.empty() ? 0 : utils::xxhash64(name);
        desc.hash = 0; // TODO: calc it
        desc.usage = 0;
        desc.ready = false;
        desc.imd_future = pool.enqueue(assets::get_image, name);

        game::journal::debug(game::journal::_GAME, "Texture % LOADING", name);

        // FIXME: dont modif desc
        textures.push_back(desc);
        textures.back().tex.desc = &textures.back();
        return textures.back().tex;*/
    }

    auto query_texture(texture &tex, const texture_desc *desc) -> void {
        if (desc) {
            if (desc->ready)
                tex = desc->tex;
        }
    }

    auto query_texture(texture &tex) -> void {
        if (tex.desc) {
            if (tex.desc->ready)
                tex = tex.desc->tex;
        }
    }

    auto default_white_texture() -> texture {
        return textures[0].tex;
    }

    auto default_black_texture() -> texture {
        return textures[1].tex;
    }

    auto default_check_texture() -> texture {
        return textures[2].tex;
    }

    auto default_red_texture() -> texture {
        return textures[3].tex;
    }

    auto get_heightmap(const char *name) -> heightmap_t {
        UNUSED(name);
        /*const auto hash = utils::xxhash64(name, strlen(name));
        auto it = std::find_if(textures.begin(), textures.end(), [hash](const texture_desc &td) {
            return td.name_hash == hash;
        });

        if (it != textures.end()) {
            game::journal::info(game::journal::_GAME, "Texture % found", name);
            return it->tex;
        }

        auto imd = assets::get_image(name);

        if (!imd.pixels) {
            game::journal::warning(game::journal::_GAME, "Texture % not found", name);
            return default_tex;
        }

        make_texture_2d(name, imd);*/

        return {};
    }

    auto make_program(const gl::program_info &info) -> program {
        auto inf = info;

        std::vector<uint64_t> hashes;
        hashes.reserve(3); // types of shader

        for (auto &i : inf.sources) {
            // just read from file to text
            if (!i.name.empty() && i.text.empty()) {
                auto t = assets::get_text(i.name);
                i.text = {t.text, t.size};

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
    }

    auto get_shader(const char *name) -> program {
        auto hash = utils::xxhash64(name, strlen(name));

        auto it = std::find_if(programs.begin(), programs.end(), [hash](const program_desc &desc) {
            if (desc.name_hash == hash)
                return true;
            return false;
        });

        if (it != programs.end())
            return (*it).pro;

        return {0, {}, {}, {}};
    }
} // namespace video
