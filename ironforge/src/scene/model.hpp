#pragma once

#include <scene/scene.hpp>

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

    };

    struct model_info {
        const char *name;
        std::vector<mesh_info> meshes;
    };

    struct model_instance {

    };

    auto create_model(const model_info &info) -> model_instance*;
} // namespace scene
