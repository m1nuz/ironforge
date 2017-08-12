#pragma once

#include <vector>
#include <video/video.hpp>
#include <scene/scene.hpp>
#include <scene/volume.hpp>

namespace scene {
    enum class mesh_source {
        file,
        gen_plane,
        gen_sphere,
        gen_cube,
        gen_grid
    };

    struct mesh_info {
        mesh_source                source;
        const char                 *filename;
        const char                 *height_map = nullptr;
        video::gen_cube_info       cube;
        video::gen_sphere_info     sphere;
        video::gen_grid_plane_info grid;
    };

    struct mesh_instance {
        video::vertices_desc    desc;
        video::vertices_source  source;
        std::vector<video::vertices_draw> draws;
        std::vector<bound_box> bounds;
        bound_sphere visible_bound;
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

    auto init_all_models() -> void;
    auto cleanup_all_models() -> void;
    auto create_model(const model_info &info) -> model_instance*;
    auto default_model() -> model_instance*;
    auto find_model(const char *name) -> model_instance*;
} // namespace scene
