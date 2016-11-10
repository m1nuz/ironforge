#include <glcore_330.h>
#include <video/commands.hpp>
#include <video/triangles_batch.hpp>

namespace video {
    auto create_triangles_batch(const triangles_batch_info &info) -> triangles_batch {
        triangles_batch tb;

        tb.max_triangles = info.max_triangles;
        tb.tex = info.tex;

        tb.vertices.reserve(info.max_triangles);

        std::vector<vertices_draw> draws;
        std::vector<vertices_data> data;
        vertices_desc desc;
        desc.primitive = GL_TRIANGLES;
        desc.vf = vertex_format::v3t2c4;
        desc.ef = index_format::ui16;
        desc.vb_usage = static_cast<uint32_t>(gl::buffer_usage::dynamic_draw);
        data.push_back({nullptr, nullptr, info.max_triangles * 3, 0});

        // TODO: check creation
        tb.source = make_vertices_source(data, desc, draws);

        return tb;
    }

    auto delete_triangles_batch(triangles_batch &tb) -> void {
        tb.vertices.clear();
    }

    auto append_triangles_vertices(triangles_batch &tb, const std::vector<v3t2c4> &vertices) -> void {
        tb.vertices.insert(std::end(tb.vertices), std::begin(vertices), std::end(vertices));
    }

    auto append_triangles_vertices(triangles_batch &tb, const v3t2c4 *vertices, size_t size) -> void {
        tb.vertices.insert(std::end(tb.vertices), vertices, vertices + size);
    }

    auto submit_triangles_batch(gl::command_buffer &cb, triangles_batch &tb, const gl::program &pm, const gl::sampler &sr) -> void {
        cb << vcs::update{tb.source.vertices, 0, &tb.vertices[0], tb.vertices.size() * sizeof (tb.vertices[0])};

        video::query_texture(tb.tex);

        cb << vcs::bind{pm};
        cb << vcs::bind{pm, "color_map", 0, tb.tex};
        cb << vcs::bind{0, sr};
        cb << vcs::bind{tb.source};

        vertices_draw draw;
        memset(&draw, 0, sizeof draw);
        draw.mode = GL_TRIANGLES;
        draw.base_vertex = 0;
        draw.count = tb.vertices.size();
        cb << vcs::draw_arrays{draw};

        tb.vertices.clear();
    }
} // namespace video
