#include <cstddef>
#include <algorithm>
#include <ironforge_utility.hpp>
#include <core/application.hpp>
#include <core/assets.hpp>
#include <video/video.hpp>
#include <video/buffer.hpp>

// this api depend from used general video api
namespace video {
    struct texture_desc {
        std::string name;
        gl::texture tex;
        uint32_t    usage;
        uint64_t    hash;
        uint64_t    name_hash;
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

    std::vector<texture_desc> textures;
    std::vector<buffer_desc> buffers;
    std::vector<vertex_array_desc> vertex_arrays;
    std::vector<program_desc> programs;

    auto init_resources() -> void {
        textures.reserve(100);
        programs.reserve(100);

        application::debug(application::log_category::video, "%\n", "Init resources");

        auto white_name = "white-map";
        auto im = video::imgen::make_color(128, 128, {255, 255, 255}); // white
        textures.push_back({white_name, gl::create_texture_2d(im), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(white_name, strlen(white_name))});
        delete[] im.pixels;

        auto black_name = "black-map";
        im = video::imgen::make_color(128, 128, {0, 0, 0}); // black
        textures.push_back({black_name, gl::create_texture_2d(im), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(black_name, strlen(black_name))});
        delete[] im.pixels;

        auto check_name = "check-map";
        im = video::imgen::make_check(128, 128, 0x10, {255, 255, 255}); // check
        textures.push_back({check_name, gl::create_texture_2d(im), 1, utils::xxhash64(im.pixels, im.width * im.height * 3), utils::xxhash64(check_name, strlen(check_name))});
        delete[] im.pixels;

        make_program({"emission-shader", {{"forward-emission.vert", {}}, {"forward-emission.frag", {}}}});
        make_program({"ambient-light-shader", {{"forward-ambient.vert", {}}, {"forward-ambient.frag", {}}}});
        make_program({"forward-directional-shader", {{"forward-directional.vert", {}}, {"forward-directional.frag", {}}}});
        make_program({"postprocess-shader", {{"screenspace.vert", {}}, {"postprocess_final.frag", {}}}});
        make_program({"hblur-shader", {{"screenspace.vert", {}}, {"filter-hblur.frag", {}}}});
        make_program({"vblur-shader", {{"screenspace.vert", {}}, {"filter-vblur.frag", {}}}});
        make_program({"skybox-shader", {{"skybox.vert", {}}, {"skybox.frag", {}}}});
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

    auto make_texture_2d(const texture_info &info) -> texture {
        texture_desc desc;
        desc.tex = gl::create_texture_2d(info);
        desc.name_hash = 0;
        desc.usage = 0;

        textures.push_back(desc);

        return desc.tex;
    }

    auto make_texture_2d(const image_data &data) -> texture {
        texture_desc desc;
        desc.tex = gl::create_texture_2d(data);
        desc.name_hash = 0;
        desc.usage = 0;

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
            default:
                application::warning(application::log_category::video, "%\n", "Unknown vertex format");
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
                application::warning(application::log_category::video, "%\n", "Unknown index format");
                break;
            }

            buffers_info.push_back({vb_size, ib_size});

            const uint32_t count = vd.indices_num ? vd.indices_num : vd.vertices_num; // vertices_num never should be 0
            draws.push_back({desc.primitive, 0, 0, count, base_vertex, base_index}); // offsets calcs later

            base_vertex += vd.vertices_num;
            base_index += vd.indices_num;
        }

        // allocate memory buffer
        void *vertex_data = malloc(vertices_data_size);
        void *index_data = malloc(indices_data_size);
        ptrdiff_t vb_offset = 0;
        ptrdiff_t ib_offset = 0;

        // copy buffers to one buffer
        for (size_t i = 0; i < data.size(); i++) {
            draws[i].vb_offset = vb_offset;
            memcpy((char*)vertex_data + vb_offset, data[i].vertices, buffers_info[i].vb_size);
            vb_offset += buffers_info[i].vb_size;

            draws[i].ib_offset = ib_offset;
            memcpy((char*)index_data + ib_offset, data[i].indices, buffers_info[i].ib_size);
            ib_offset += buffers_info[i].ib_size;
        }

        auto va = gl::create_vertex_array();
        vertex_arrays.push_back({va});

        gl::bind_vertex_array(va);

        // TODO : seaarch other buffer with same hash
        auto vb = gl::create_buffer(gl::buffer_target::array, vertices_data_size, vertex_data, gl::buffer_usage::static_draw);
        buffers.push_back({vb, 0, utils::xxhash64(vertex_data, vertices_data_size)});

        // transfer to video memory
        switch (desc.vf) {
        case vertex_format::v3t2n3:
            gl::vertex_array_buffer(va, vb, 0, sizeof(v3t2n3));
            gl::vertex_array_format(va, vertex_attributes::position, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3, position));
            gl::vertex_array_format(va, vertex_attributes::texcoord, 2, gl::attrib_type::float_value, false, offsetof(v3t2n3, texcoord));
            gl::vertex_array_format(va, vertex_attributes::normal, 3, gl::attrib_type::float_value, false, offsetof(v3t2n3, normal));
            break;
        default:
            application::warning(application::log_category::video, "%\n", "Unknown vertex format");
            break;
        }

        auto eb = gl::create_buffer(gl::buffer_target::element_array, indices_data_size, index_data, gl::buffer_usage::static_draw);
        buffers.push_back({eb, 0, utils::xxhash64(index_data, indices_data_size)});

        gl::unbind_vertex_array(va);

        free(vertex_data);
        free(index_data);

        return {va, vb, eb};
    }

    auto get_texture(const char *name, const texture &default_tex) -> texture {

        auto hash = utils::xxhash64(name, strlen(name));
        auto it = std::find_if(textures.begin(), textures.end(), [hash](const texture_desc &td) {
            return td.name_hash == hash;
        });

        if (it != textures.end())
            return it->tex;

        auto imd = assets::get_image(name);

        if (!imd.pixels) {
            application::warning(application::log_category::application, "Texture % not found\n", name);
            return default_tex;
        }

        return make_texture_2d(imd);
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

    auto make_program(const gl330::program_info &info) -> program {
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
            application::warning(application::log_category::video, "Program % already created\n", pi->pro.pid);
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
