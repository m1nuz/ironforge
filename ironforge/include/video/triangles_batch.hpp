#pragma once

#include <vector>

#include <video/video.hpp>

namespace video {

    struct triangles_batch_info {
        size_t                  max_triangles;
        texture                 tex;
    };

    struct triangles_batch {
        size_t                  max_triangles;
        texture                 tex;

        std::vector<v3t2c4>     vertices;
        vertices_source         source;
    };

    auto create_triangles_batch(instance_t &vi, const triangles_batch_info &info) -> triangles_batch;
    auto delete_triangles_batch(triangles_batch &tb) -> void;

    auto append_triangles_vertices(triangles_batch &tb, const std::vector<v3t2c4> &vertices) -> void;
    auto append_triangles_vertices(triangles_batch &tb, const v3t2c4 *vertices, size_t size) -> void;

    auto submit_triangles_batch(gl::command_buffer &cb, triangles_batch &tb, const gl::program &pm, const gl::sampler &sr) -> void;

} // namespace video
