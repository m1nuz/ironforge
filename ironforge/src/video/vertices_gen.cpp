#include <glcore_330.h>
#include <video/vertices.hpp>
#include <video/vertices_gen.hpp>

#include "cube.hpp"

namespace video {
    namespace vertgen {
        auto make_cube(glm::mat4 transform) -> vertices_info {
            UNUSED(transform);

            return {{cube_vertices_v3t2n3, cube_indices_v3t2n3, cube_vertices_num, cube_indices_num}, {GL_TRIANGLES, vertex_format::v3t2n3, index_format::ui16}};
        }

        auto make_sphere(const gen_sphere_info *info, glm::mat4 transform) -> vertices_info {
            UNUSED(transform);
        }

        auto make_quad_plane(glm::mat4 transform) -> vertices_info {
            UNUSED(transform);
        }

        //vertices_info make_torus(const gen_torus_info *info, glm::mat4 transform);
        //vertices_info make_ribbon(const gen_ribbon_info *info, glm::mat4 transform);
        auto make_grid_plane(const gen_grid_plane_info *info, glm::mat4 transform) -> vertices_info {

        }
    } // namespace vertgen
} // namespace video
