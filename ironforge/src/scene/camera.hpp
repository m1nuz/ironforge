#pragma once

#include  <ironforge_common.hpp>

namespace scene {
    struct camera_info {
        // camera_type type;
        float   fov    = 0.f;
        //float   aspect = 1.f;
        float   znear  = 1.f;
        float   zfar   = 1000.f;
        int32_t parent = 0;
    };

    struct camera_instance {
        int         entity;
        int         parent;
        float       fov;
        float       aspect;
        float       znear;
        float       zfar;
        glm::mat4   projection;
        glm::mat4   view;
    };

    auto init_all_cameras() -> void;
    auto create_camera(int32_t entity, const camera_info &info) -> camera_instance*;
    auto present_all_cameras(std::unique_ptr<instance>& s) -> void;
} // namespace scene
