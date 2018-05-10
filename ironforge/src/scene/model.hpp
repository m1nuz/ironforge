#pragma once

#include <vector>
#include <video/video.hpp>
#include <scene/volume.hpp>

namespace scene {
//    struct mesh_instance {
//        video::vertices_desc    desc;
//        video::vertices_source  source;
//        std::vector<video::vertices_draw> draws;
//        std::vector<bound_box> bounds;
//        bound_sphere visible_bound;
//    };

    struct model_instance {
        bound_box                   aabb;
        bound_sphere                sphere;

        std::vector<video::mesh>    meshes;
    };

    //auto create_model(const std::string &name, const std::vector<mesh_info> &meshes) -> model_instance;
    auto create_model(const std::vector<video::mesh> &meshes) -> std::optional<model_instance>;
} // namespace scene
