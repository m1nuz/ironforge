#include <glcore_330.h>
#include <core/application.hpp>
#include <video/vertices.hpp>
#include <video/vertices_gen.hpp>

#include "cube.hpp"
#include "quad.hpp"

namespace video {
    namespace vertgen {
        auto make_cube(glm::mat4 transform) -> vertices_info {
            UNUSED(transform);

            return {{cube_vertices_v3t2n3, cube_indices_v3t2n3, cube_vertices_num, cube_indices_num}, {GL_TRIANGLES, vertex_format::v3t2n3, index_format::ui16}};
        }

        auto make_sphere(const gen_sphere_info *info, glm::mat4 transform) -> vertices_info {
            UNUSED(transform);

            const float R = 1. / (float)(info->rings - 1);
            const float S = 1. / (float)(info->sectors - 1);
            uint32_t r, s;

            auto vertices = (v3t2n3*)malloc(info->rings * info->sectors * sizeof(v3t2n3));

            auto v = vertices;

            for (r = 0; r < info->rings; r++) {
                for (s = 0; s < info->sectors; s++) {
                    const float y = sin(-M_PI_2 + M_PI * r * R );
                    const float x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
                    const float z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

                    (*v).texcoord[0] = s * S;
                    (*v).texcoord[1] = r * R;

                    (*v).position[0] = x * info->radius;
                    (*v).position[1] = y * info->radius;
                    (*v).position[2] = z * info->radius;

                    (*v).normal[0] = x;
                    (*v).normal[1] = y;
                    (*v).normal[2] = z;

                    v++;
                }
            }

            if ((info->rings * info->sectors * 6) > UINT16_MAX)
                application::warning(application::log_category::video, "%\n", "indices is too mutch");

            auto indices = (uint16_t*)malloc(info->rings * info->sectors * 6 * sizeof(uint16_t));

            auto i = indices;

            for (r = 0; r < info->rings; r++) {
                for (s = 0; s < info->sectors; s++) {
                    *i++ = r * info->sectors + s; // 0
                    *i++ = r * info->sectors + (s + 1); // 1
                    *i++ = (r + 1) * info->sectors + s; // 3
                    *i++ = r * info->sectors + (s + 1); // 1
                    *i++ = (r + 1) * info->sectors + (s + 1); // 2
                    *i++ = (r + 1) * info->sectors + s; // 3
                }
            }

            // TODO: mul to transorm matrix

            return {{vertices, indices, info->rings * info->sectors, info->rings * (info->sectors - 1) * 6}, {GL_TRIANGLES, vertex_format::v3t2n3, index_format::ui16}};
        }

        auto make_quad_plane(glm::mat4 transform) -> vertices_info {
            UNUSED(transform);

            return {{quad_vertices, quad_indices, quad_vertices_num, quad_indices_num}, {GL_TRIANGLES, vertex_format::v3t2n3, index_format::ui16}};
        }

        //vertices_info make_torus(const gen_torus_info *info, glm::mat4 transform);
        //vertices_info make_ribbon(const gen_ribbon_info *info, glm::mat4 transform);
        auto make_grid_plane(const gen_grid_plane_info *info, glm::mat4 transform) -> vertices_info {
            UNUSED(transform);

            uint32_t number_vertices = (info->rows + 1) * (info->columns + 1);
            uint32_t number_indices = info->rows * 6 * info->columns;
            uint32_t mode = GL_TRIANGLES;

            if (info->triangle_strip) {
                number_indices = info->rows * 2 * (info->columns + 1);
                mode = GL_TRIANGLE_STRIP;
            }

            float *vertices = (float *)malloc(4 * number_vertices * sizeof(float));
            float *normals = (float *)malloc(3 * number_vertices * sizeof(float));
            float *texCoords = (float *)malloc(2 * number_vertices * sizeof(float));
            uint16_t *indices = (uint16_t *)malloc(number_indices * sizeof(uint16_t));

            for (uint32_t i = 0; i < number_vertices; i++) {
                float x = (float) (i % (info->columns + 1)) / (float) info->columns;
                float y = 1.0f - (float) (i / (info->columns + 1)) / (float) info->rows;
                float s = x * info->columns;
                float t = y * info->rows;

                vertices[i * 4 + 0] = info->horizontal_extend * (x - 0.5f);
                vertices[i * 4 + 1] = info->vertical_extend * (y - 0.5f);
                vertices[i * 4 + 2] = 0.0f;
                vertices[i * 4 + 3] = 1.0f;

                normals[i * 3 + 0] = 0.0f;
                normals[i * 3 + 1] = 0.0f;
                normals[i * 3 + 2] = 1.0f;

                texCoords[i * 2 + 0] = s;
                texCoords[i * 2 + 1] = t;
            }

            if (info->triangle_strip)
                for (uint32_t i = 0; i < info->rows * (info->columns + 1); i++) {
                    uint32_t currentColumn = i % (info->columns + 1);
                    uint32_t currentRow = i / (info->columns + 1);

                    if (currentRow == 0) {
                        // Left to right, top to bottom
                        indices[i * 2] = currentColumn + currentRow * (info->columns + 1);
                        indices[i * 2 + 1] = currentColumn + (currentRow + 1) * (info->columns + 1);
                    } else {
                        // Right to left, bottom to up
                        indices[i * 2] = (info->columns - currentColumn) + (currentRow + 1) * (info->columns + 1);
                        indices[i * 2 + 1] = (info->columns - currentColumn) + currentRow * (info->columns + 1);
                    }
                }
            else
                for (uint32_t i = 0; i < info->rows * info->columns; i++) {
                    uint32_t currentColumn = i % info->columns;
                    uint32_t currentRow = i / info->columns;

                    indices[i * 6 + 0] = currentColumn + currentRow * (info->columns + 1);
                    indices[i * 6 + 1] = currentColumn + (currentRow + 1) * (info->columns + 1);
                    indices[i * 6 + 2] = (currentColumn + 1) + (currentRow + 1) * (info->columns + 1);

                    indices[i * 6 + 3] = (currentColumn + 1) + (currentRow + 1) * (info->columns + 1);
                    indices[i * 6 + 4] = (currentColumn + 1) + currentRow * (info->columns + 1);
                    indices[i * 6 + 5] = currentColumn + currentRow * (info->columns + 1);
                }

            v3t2n3 *vs = (v3t2n3*)malloc(number_vertices * sizeof(v3t2n3));

            for (uint32_t i = 0; i < number_vertices; i++) {
                vs[i].position[0] = vertices[i * 4 + 0];
                vs[i].position[1] = vertices[i * 4 + 1];
                vs[i].position[2] = vertices[i * 4 + 2];

                vs[i].normal[0] = normals[i * 3 + 0];
                vs[i].normal[1] = normals[i * 3 + 1];
                vs[i].normal[2] = normals[i * 3 + 2];

                vs[i].texcoord[0] = texCoords[i * 2 + 0];
                vs[i].texcoord[1] = texCoords[i * 2 + 1];
            }

            // TODO: mul with transorm matrix

            free(vertices);
            free(normals);
            free(texCoords);

            return {{vs, indices, number_vertices, number_indices}, {mode, vertex_format::v3t2n3, index_format::ui16}};
        }
    } // namespace vertgen
} // namespace video
