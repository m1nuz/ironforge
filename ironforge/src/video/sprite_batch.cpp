#include <glcore_330.h>
#include <video/sprite_batch.hpp>
#include <video/commands.hpp>

namespace video {
    auto create_sprite_batch(instance_t &vi, const sprite_batch_info &info) -> sprite_batch {
        sprite_batch sb;

        sb.max_sprites = info.max_sprites;
        sb.sprites_count = 0;
        sb.tex = info.tex;

        sb.vertices.reserve(info.max_sprites * 4);
        sb.indices.reserve(info.max_sprites * 6);

        for (size_t i = 0; i < info.max_sprites; i++)
        {
            sb.indices.push_back(0 + i * 4);
            sb.indices.push_back(1 + i * 4);
            sb.indices.push_back(2 + i * 4);

            sb.indices.push_back(0 + i * 4);
            sb.indices.push_back(2 + i * 4);
            sb.indices.push_back(3 + i * 4);
        }

        std::vector<vertices_draw> draws;
        std::vector<vertices_data> data;
        vertices_desc desc;
        desc.primitive = GL_TRIANGLES;
        desc.vf = vertex_format::v3t2c4;
        desc.ef = index_format::ui16;
        desc.vb_usage = static_cast<uint32_t>(gl::buffer_usage::dynamic_draw);
        data.push_back({nullptr, &sb.indices[0], info.max_sprites * 6, info.max_sprites * 4});

        sb.source = make_vertices_source(vi, data, desc, draws);

        return sb;
    }

    auto delete_sprite_batch(sprite_batch &sb) -> void {
        sb.vertices.clear();
        sb.indices.clear();
    }

    auto append_sprite_vertices(sprite_batch &sb, const v3t2c4 (&vertices)[4]) -> void {
        sb.vertices.push_back(vertices[0]);
        sb.vertices.push_back(vertices[1]);
        sb.vertices.push_back(vertices[2]);
        sb.vertices.push_back(vertices[3]);

        sb.sprites_count++;
    }

    auto append_sprite(sprite_batch &sb, const glm::vec3 &position, const glm::vec2 size, const glm::vec4 offset, const glm::vec4 color) -> void {
        const float correction = screen.aspect;

        const v3t2c4 vertices[4] = {
            {{position[0], position[1] + size[1] * correction, 0}, {offset[0], offset[1]}, {color[0], color[1], color[2], color[3]}},
            {{position[0] + size[0], position[1] + size[1] * correction, 0}, {offset[0] + offset[2], offset[1]}, {color[0], color[1], color[2], color[3]}},
            {{position[0] + size[0], position[1], 0}, {offset[0] + offset[2], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
            {{position[0], position[1], 0}, {offset[0], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
        };

        append_sprite_vertices(sb, vertices);
    }

    auto submit_sprite_batch(gl::command_buffer &cb, sprite_batch &sb, const gl::program &pm, const gl::sampler &sr) -> void {
        cb << vcs::update{sb.source.vertices, 0, &sb.vertices[0], sb.vertices.size() * sizeof (sb.vertices[0])};

        cb << vcs::bind{pm};
        cb << vcs::bind{pm, "color_map", 0, sb.tex};
        cb << vcs::bind{0, sr};
        cb << vcs::bind{sb.source};

        vertices_draw draw;
        memset(&draw, 0, sizeof draw);
        draw.mode = GL_TRIANGLES;
        draw.base_vertex = 0;
        draw.count = sb.sprites_count * 6;
        cb << vcs::draw_elements{draw};

        sb.sprites_count = 0;
        sb.vertices.clear();
    }
} // namespace video
