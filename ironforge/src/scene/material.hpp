#pragma once

#include <vector>
#include <string>
#include <ironforge_common.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>

namespace scene {
    struct material_info {
        glm::vec3       emission;
        glm::vec3       ambient;
        glm::vec3       diffuse;
        glm::vec3       specular;
        float           shininess;
        float           transparency;

        const char      *emission_map = nullptr;
        const char      *diffuse_map = nullptr;
        const char      *specular_map = nullptr;
        const char      *gloss_map = nullptr;

        video::texture  *emission_tex = nullptr;
        video::texture  *diffuse_tex = nullptr;
        video::texture  *specular_tex = nullptr;
        video::texture  *gloss_tex = nullptr;

        const char      *name = nullptr;
    };

    struct material_instance {
        renderer::phong::material m0; // TODO: array of materials?
        std::string name;
        uint64_t name_hash;
    };

    auto init_all_materials() -> void;
    auto create_material(const material_info &info) -> material_instance*;
    auto default_material() -> material_instance*;
    auto get_material(const char *name) -> material_instance*;
} // namespace scene
