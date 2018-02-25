#pragma once

#include <vector>
#include <string>
#include <core/common.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>

#include <optional>

#include <json.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace scene {
    struct material_info {
        glm::vec3       emission = {0.f, 0.f, 0.f};
        glm::vec3       ambient = {0.f, 0.f, 0.f};
        glm::vec3       diffuse = {0.f, 0.f, 0.f};
        glm::vec3       specular = {0.f, 0.f, 0.f};
        float           shininess = 0.f;
        float           transparency = 0.f;
        float           reflectivity = 0.f;

        const char      *emission_map = nullptr;
        const char      *diffuse_map = nullptr;
        const char      *specular_map = nullptr;
        const char      *gloss_map = nullptr;
        const char      *normal_map = nullptr;

        video::texture  emission_tex;
        video::texture  diffuse_tex;
        video::texture  specular_tex;
        video::texture  gloss_tex;
        video::texture  normal_tex;

        const char      *name = nullptr;
    };

    struct material_instance {
        renderer::phong::material m0; // TODO: array of materials?
        std::string name = {};
        uint64_t name_hash = 0;
    };

    /*auto init_all_materials() -> void;
    auto cleanup_all_materials() -> void;
    auto create_material(const material_info &info) -> material_instance*;
    auto default_material() -> material_instance*;
    auto find_material(const char *name) -> material_instance*;

    auto process_all_materials() -> void;*/

    using json = nlohmann::json;
    auto create_material(assets::instance_t &asset, const json &info) -> std::optional<material_instance>;
} // namespace scene
