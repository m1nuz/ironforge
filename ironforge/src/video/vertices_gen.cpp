#include <vector>
#include <cstring>

#include <glcore_330.h>
#include <video/vertices.hpp>
#include <video/vertices_gen.hpp>

namespace video {
    namespace vertgen {
        struct shape {
            std::vector<glm::vec4>  vertices;
            std::vector<glm::vec3>  normals;
            std::vector<glm::vec3>  tangents;
            std::vector<glm::vec3>  bitangents;
            std::vector<glm::vec2>  texcoords;
            std::vector<uint16_t>   indices; // 16bit only for now

            void                    *attributes;
            void                    *elements;
            size_t                  attributes_num;
            size_t                  elements_num;

            uint32_t                mode; // triangles or triangle_strip
        };

        // TODO: make vertices_info unique
        auto make_vi(vertex_format vf, index_format ef, shape &sh) -> vertices_info {
            // TODO: calc bitangents

            auto stride = 0u;

            switch (vf) {
            case vertex_format::v3t2n3:
                sh.attributes_num = sh.vertices.size();
                sh.attributes = (v3t2n3*)malloc(sizeof (v3t2n3) * sh.vertices.size());
                stride = sizeof (v3t2n3);
                for (size_t i = 0; i < sh.vertices.size(); i++) {
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3, position), &sh.vertices[i].x, sizeof (glm::vec3));
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3, texcoord), &sh.texcoords[i].x, sizeof (glm::vec2));
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3, normal), &sh.normals[i].x, sizeof (glm::vec3));

                    //printf("%f %f %f / %f %f / %f %f %f\n", v->position.x, v->position.y, v->position.z, v->texcoord.x, v->texcoord.y, v->normal.x, v->normal.y, v->normal.z);
                }
                break;
            case vertex_format::v3t2n3t3:
                sh.attributes_num = sh.vertices.size();
                sh.attributes = (v3t2n3t3*)malloc(sizeof (v3t2n3t3) * sh.vertices.size());
                stride = sizeof (v3t2n3t3);
                for (size_t i = 0; i < sh.vertices.size(); i++) {
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3t3, position), &sh.vertices[i].x, sizeof (glm::vec3));
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3t3, texcoord), &sh.texcoords[i].x, sizeof (glm::vec2));
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3t3, normal), &sh.normals[i].x, sizeof (glm::vec3));
                    memcpy((char*)sh.attributes + stride * i + offsetof(v3t2n3t3, tangent), &sh.tangents[i].x, sizeof (glm::vec3));
                }
                break;
            default:
                break;
            }

            switch (ef) {
            case index_format::ui16:
                sh.elements_num = sh.indices.size();
                sh.elements = (uint16_t*)malloc(sizeof (uint16_t) * sh.indices.size());
                memcpy(sh.elements, &sh.indices[0], sizeof (uint16_t) * sh.indices.size());
                break;
            case index_format::ui32:
                break;
            default:
                break;
            }

            return {{sh.attributes, sh.elements, sh.attributes_num, sh.elements_num}, {sh.mode, vf, ef}};
        }

        auto make_plane(const glm::mat4 &transform) -> vertices_info {
            UNUSED(transform);

            shape sh;

            sh.attributes = nullptr;
            sh.mode = GL_TRIANGLES;
            sh.vertices = {{-1.0f, -1.0f, 0.0f, +1.0f},
                           {+1.0f, -1.0f, 0.0f, +1.0f},
                           {-1.0f, +1.0f, 0.0f, +1.0f},
                           {+1.0f, +1.0f, 0.0f, +1.0f}};
            sh.normals = {{0.0f, 0.0f, 1.0f},
                          {0.0f, 0.0f, 1.0f},
                          {0.0f, 0.0f, 1.0f},
                          {0.0f, 0.0f, 1.0f}};
            sh.tangents = {{1.0f, 0.0f, 0.0f},
                           {1.0f, 0.0f, 0.0f},
                           {1.0f, 0.0f, 0.0f},
                           {1.0f, 0.0f, 0.0f}};
            sh.texcoords = {{0.0f, 0.0f},
                            {1.0f, 0.0f},
                            {0.0f, 1.0f},
                            {1.0f, 1.0f}};
            sh.indices = {0, 1, 2, 1, 3, 2};

            return make_vi(vertex_format::v3t2n3t3, index_format::ui16, sh);
        }

        auto make_cube(const glm::mat4 &transform) -> vertices_info {
            UNUSED(transform);

            shape sh;
            sh.attributes = nullptr;
            sh.mode = GL_TRIANGLES;
            sh.vertices = {{-1.0f, -1.0f, -1.0f, +1.0f}, {-1.0f, -1.0f, +1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f, +1.0f}, {+1.0f, -1.0f, -1.0f, +1.0f},
                           {-1.0f, +1.0f, -1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f, +1.0f},
                           {-1.0f, -1.0f, -1.0f, +1.0f}, {-1.0f, +1.0f, -1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f, +1.0f}, {+1.0f, -1.0f, -1.0f, +1.0f},
                           {-1.0f, -1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f, +1.0f},
                           {-1.0f, -1.0f, -1.0f, +1.0f}, {-1.0f, -1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, +1.0f, +1.0f}, {-1.0f, +1.0f, -1.0f, +1.0f},
                           {+1.0f, -1.0f, -1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f, +1.0f}};
            sh.normals = {{ 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f},
                          { 0.0f, +1.0f,  0.0f}, { 0.0f, +1.0f,  0.0f}, { 0.0f, +1.0f,  0.0f}, { 0.0f, +1.0f,  0.0f},
                          { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f},
                          { 0.0f,  0.0f, +1.0f}, { 0.0f,  0.0f, +1.0f}, { 0.0f,  0.0f, +1.0f}, { 0.0f,  0.0f, +1.0f},
                          {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f},
                          {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}};
            sh.tangents = {{+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f},
                           {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f},
                           {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f},
                           {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f}, {+1.0f,  0.0f,  0.0f},
                           { 0.0f,  0.0f, +1.0f}, { 0.0f,  0.0f, +1.0f}, { 0.0f,  0.0f, +1.0f}, { 0.0f,  0.0f, +1.0f},
                           { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}};
            sh.texcoords = {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
                            {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
                            {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},
                            {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
                            {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                            {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};
            sh.indices = {0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21};

            return make_vi(vertex_format::v3t2n3t3, index_format::ui16, sh);
        }

        auto make_sphere(const gen_sphere_info *info, const glm::mat4 &transform) -> vertices_info {
            UNUSED(transform);

            shape sh;

            size_t num_slices = info->sectors;
            size_t num_parallels = info->sectors / 2;
            size_t num_vertices = (num_parallels + 1) * (num_slices + 1);
            size_t num_indices = num_parallels * num_slices * 6;

            auto angle_step = (2.0f * M_PI) / ((float) num_slices);

            sh.mode = GL_TRIANGLES;
            sh.vertices.reserve(num_vertices);
            sh.normals.reserve(num_vertices);
            sh.tangents.reserve(num_vertices);
            sh.texcoords.reserve(num_vertices);
            sh.indices.reserve(num_indices);

            for (size_t i = 0; i < num_parallels + 1; i++)
                for (size_t j = 0; j < num_slices + 1; j++) {
                    sh.vertices.emplace_back(info->radius * sinf(angle_step * (float)i) * sinf(angle_step * (float)j),
                                             info->radius * cosf(angle_step * (float)i),
                                             info->radius * sinf(angle_step * (float)i) * cosf(angle_step * (float)j),
                                             1.f);
                    sh.normals.emplace_back(sh.vertices.back().x / info->radius,
                                            sh.vertices.back().y / info->radius,
                                            sh.vertices.back().z / info->radius);
                    sh.texcoords.emplace_back((float)j / (float)num_slices, 1.0f - (float)i / (float)num_parallels);

                    glm::vec4 hv{1.0f, 0.0f, 0.0f, 1.f};
                    glm::quat h{glm::vec3{sh.texcoords.back().x * 2.f * M_PI, 0, 0}};
                    sh.tangents.emplace_back(glm::mat4_cast(h) * hv);
                }

            for (size_t i = 0; i < num_parallels; i++)
                for (size_t j = 0; j < num_slices; j++) {
                    sh.indices.push_back(i * (num_slices + 1) + j);
                    sh.indices.push_back((i + 1) * (num_slices + 1) + j);
                    sh.indices.push_back((i + 1) * (num_slices + 1) + (j + 1));

                    sh.indices.push_back(i * (num_slices + 1) + j);
                    sh.indices.push_back((i + 1) * (num_slices + 1) + (j + 1));
                    sh.indices.push_back(i * (num_slices + 1) + (j + 1));
                }

            return make_vi(vertex_format::v3t2n3t3, index_format::ui16, sh);
        }

        //vertices_info make_disc(const float radius, const uint32_t num_sectors)
        //vertices_info make_torus(const gen_torus_info *info, glm::mat4 transform);
        //vertices_info make_cylinder(const float half_extend, const float radius, const uint32_t num_slices);
        //vertices_info make_cone(const float half_extend, const float radius, const uint32_t num_slices, const uint32_t num_stacks);
        //vertices_info make_ribbon(const gen_ribbon_info *info, glm::mat4 transform);

        auto make_grid_plane(const gen_grid_plane_info *info, const glm::mat4 &transform) -> vertices_info {
            UNUSED(transform);

            size_t num_vertices = (info->rows + 1) * (info->columns + 1);
            size_t num_indices = info->rows * 6 * info->columns;
            uint32_t mode = GL_TRIANGLES;

            if (info->triangle_strip) {
                num_indices = info->rows * 2 * (info->columns + 1);
                mode = GL_TRIANGLE_STRIP;
            }

            shape sh;
            sh.mode = mode;
            sh.vertices.reserve(num_vertices);
            sh.normals.reserve(num_vertices);
            sh.tangents.reserve(num_vertices);
            sh.texcoords.reserve(num_vertices);
            sh.indices.reserve(num_indices);

            for (size_t i = 0; i < num_vertices; i++) {
                float x = (float) (i % (info->columns + 1)) / (float) info->columns;
                float y = 1.0f - (float) (i / (info->columns + 1)) / (float) info->rows;
                float s = x * info->columns;
                float t = y * info->rows;

                sh.vertices.emplace_back(info->horizontal_extend * (x - 0.5f), info->vertical_extend * (y - 0.5f), 0.f, 1.f);
                sh.normals.emplace_back(0.f, 0.f, 1.f);
                sh.tangents.emplace_back(1.f, 0.f, 0.f);
                sh.texcoords.emplace_back(s, t);
            }

            if (info->triangle_strip)
                for (size_t i = 0; i < info->rows * (info->columns + 1); i++) {
                    uint32_t current_column = i % (info->columns + 1);
                    uint32_t current_row = i / (info->columns + 1);

                    if (current_row == 0) {
                        // left to right, top to bottom
                        sh.indices.push_back(current_column + current_row * (info->columns + 1));
                        sh.indices.push_back(current_column + (current_row + 1) * (info->columns + 1));
                    } else {
                        // right to left, bottom to up
                        sh.indices.push_back((info->columns - current_column) + (current_row + 1) * (info->columns + 1));
                        sh.indices.push_back((info->columns - current_column) + current_row * (info->columns + 1));
                    }
                }
            else
                for (size_t i = 0; i < info->rows * info->columns; i++) {
                    uint32_t current_column = i % info->columns;
                    uint32_t current_row = i / info->columns;

                    sh.indices.push_back(current_column + current_row * (info->columns + 1));
                    sh.indices.push_back(current_column + (current_row + 1) * (info->columns + 1));
                    sh.indices.push_back((current_column + 1) + (current_row + 1) * (info->columns + 1));

                    sh.indices.push_back((current_column + 1) + (current_row + 1) * (info->columns + 1));
                    sh.indices.push_back((current_column + 1) + current_row * (info->columns + 1));
                    sh.indices.push_back(current_column + current_row * (info->columns + 1));
                }

            return make_vi(vertex_format::v3t2n3t3, index_format::ui16, sh);
        }
    } // namespace vertgen
} // namespace video
