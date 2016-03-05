#pragma once

#include <vector>
#include <video/video.hpp>
#include <scene/scene.hpp>
#include <scene/volume.hpp>

namespace scene {
    enum class mesh_source {
        file,
        gen_sphere,
        gen_cube,
        gen_grid
    };

    struct gen_cube_info {
        float size;
    };

    struct gen_sphere_info {
        int         rings;
        int         sectors;
        float       radius;
    };

    struct gen_grid_plane_info {
        float       horizontal_extend;
        float       vertical_extend;
        uint32_t    rows;
        uint32_t    columns;
        bool        triangle_strip;
    };

    struct mesh_info {
        mesh_source         source;
        const char          *filename;
        gen_cube_info       cube;
        gen_sphere_info     sphere;
        gen_grid_plane_info grid;
    };

    struct mesh_instance {
        video::vertices_desc    desc;
        video::vertices_source  source;

        struct submesh {
            uint32_t    vb_offset;
            uint32_t    ib_offset;
            uint32_t    count;
            uint32_t    base_vertex;
            uint32_t    base_index;
            bound_box   aabb;
        };

        std::vector<submesh> sub_meshes;
    };

    struct model_info {
        const char *name;
        std::vector<mesh_info> meshes;
    };

    struct model_instance {
        uint64_t                    name_hash;
        std::string                 name;
        bound_box                   aabb;
        bound_sphere                sphere;

        std::vector<mesh_instance>  meshes;
    };

    auto create_model(const model_info &info) -> model_instance*;
    auto default_model() -> model_instance*;
    auto get_model(const char *name) -> model_instance*;
} // namespace scene