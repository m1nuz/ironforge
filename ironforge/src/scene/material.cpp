#include <algorithm>

#include <utility/hash.hpp>
#include <core/journal.hpp>
#include <scene/scene.hpp>
#include <video/video.hpp>

#include "material.hpp"

namespace scene {
    /*std::vector<material_instance> materials;

    auto init_all_materials() -> void {
        materials.reserve(max_materials);
    }

    auto cleanup_all_materials() -> void {

    }

    auto create_material(const material_info &info) -> material_instance* {
        // TODO: check copy in materials

        material_instance mi;

        mi.name = info.name;
        mi.name_hash = utils::xxhash64(info.name, strlen(info.name), 0);
        mi.m0.ka = info.ambient;
        mi.m0.kd = info.diffuse;
        mi.m0.ks = info.specular;
        mi.m0.ke = info.emission;
        mi.m0.ns = info.shininess;
        mi.m0.tr = info.transparency;
        mi.m0.reflectivity = info.reflectivity;

        mi.m0.diffuse_tex = info.diffuse_map ? video::get_texture(info.diffuse_map) : info.diffuse_tex.id != 0 ? info.diffuse_tex : video::default_check_texture();
        mi.m0.specular_tex = info.specular_map ? video::get_texture(info.specular_map) : info.specular_tex.id != 0 ? info.specular_tex : video::default_white_texture();
        mi.m0.gloss_tex = info.gloss_map ? video::get_texture(info.gloss_map) : info.gloss_tex.id != 0 ? info.gloss_tex : video::default_white_texture();
        mi.m0.emission_tex = info.emission_map ? video::get_texture(info.emission_map) : info.emission_tex.id != 0 ? info.emission_tex : video::default_black_texture();
        mi.m0.normal_tex = info.normal_map ? video::get_texture(info.normal_map) : info.normal_tex.id != 0 ? info.normal_tex : video::default_white_texture();

        materials.push_back(mi);

        game::journal::debug(game::journal::_SCENE, "Create material '%' % (%)", info.name, mi.name_hash, materials.size());

        return &materials.back();
    }

    auto default_material() -> material_instance* {
        return nullptr;
    }

    auto find_material(const char *name) -> material_instance* {
        uint64_t hash =  utils::xxhash64(name, strlen(name), 0);

        auto it = std::find_if(materials.begin(), materials.end(), [hash](const material_instance &i) {
            if (i.name_hash == hash)
                return true;
            return false;
        });

        if (it != materials.end())
            return &(*it);

        game::journal::warning(game::journal::_SCENE, "Material '%' not found", name);

        return default_material();
    }

    auto process_all_materials() -> void {
        for (auto &m : materials) {
            //printf("%s %p\n", m.name.c_str(), (void*)m.m0.diffuse_tex.desc);
            video::query_texture(m.m0.diffuse_tex);
            video::query_texture(m.m0.specular_tex);
            video::query_texture(m.m0.gloss_tex);
            video::query_texture(m.m0.normal_tex);
        }
    }*/

    auto create_material(video::instance_t &vi, const json &info) -> std::optional<material_instance> {
        using namespace std;
        using namespace glm;
        using namespace game;

        const auto name = info.find("name") != info.end() ? info["name"].get<string>() : string{};

        material_instance m;
        m.m0.ka = info.find("ambient") != info.end() ? info["ambient"].get<vec3>() : vec3{};
        m.m0.kd = info.find("diffuse") != info.end() ? info["diffuse"].get<vec3>() : vec3{};
        m.m0.ks = info.find("specular") != info.end() ? info["specular"].get<vec3>() : vec3{};
        m.m0.ke = info.find("emission") != info.end() ? info["emission"].get<vec3>() : vec3{};
        m.m0.ns = info.find("shininess") != info.end() ? info["shininess"].get<float>() : 0.f;
        m.m0.tr = info.find("transparency") != info.end() ? info["transparency"].get<float>() : 0.f;
        m.m0.reflectivity = info.find("reflectivity") != info.end() ? info["reflectivity"].get<float>() : 0.f;

        m.m0.diffuse_tex = info.find("diffuse_map") != info.end() ? video::get_texture(vi, info["diffuse_map"].get<string>()) : video::get_texture(vi, "check-map");
        m.m0.specular_tex = info.find("specular_map") != info.end() ? video::get_texture(vi, info["specular_map"].get<string>()) : video::get_texture(vi, "white-map");
        m.m0.gloss_tex = info.find("gloss_map") != info.end() ? video::get_texture(vi, info["gloss_map"].get<string>()) : video::get_texture(vi, "white-map");
        m.m0.emission_tex = info.find("emission_map") != info.end() ? video::get_texture(vi, info["emission_map"].get<string>()) : video::get_texture(vi, "white-map");
        m.m0.normal_tex = info.find("normal_map") != info.end() ? video::get_texture(vi, info["normal_map"].get<string>()) : video::get_texture(vi, "white-map");

        journal::info(journal::_SCENE, "Create '%' material", name);

        return m;
    }
} // namespace scene
