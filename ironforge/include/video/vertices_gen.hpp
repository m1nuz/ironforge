#pragma once

#include <ironforge_common.hpp>
#include <video/vertices.hpp>

namespace video {
    struct gen_cube_info {
        float size;
    };

    struct gen_sphere_info {
        uint32_t    rings;
        uint32_t    sectors;
        float       radius;
    };

    struct gen_grid_plane_info {
        float       horizontal_extend;
        float       vertical_extend;
        uint32_t    rows;
        uint32_t    columns;
        bool        triangle_strip;
    };

    namespace vertgen {
        auto make_cube(glm::mat4 transform) -> vertices_info;
        auto make_sphere(const gen_sphere_info *info, glm::mat4 transform) -> vertices_info;
        auto make_quad_plane(glm::mat4 transform) -> vertices_info;
        //vertices_info make_torus(const gen_torus_info *info, glm::mat4 transform);
        //vertices_info make_ribbon(const gen_ribbon_info *info, glm::mat4 transform);
        auto make_grid_plane(const gen_grid_plane_info *info, glm::mat4 transform) -> vertices_info;
    } // namespace vertgen
} // namespace video