#include <algorithm>

#include <ironforge_utility.hpp>
#include <core/application.hpp>
#include <scene/scene.hpp>
#include <video/video.hpp>

#include "material.hpp"

namespace scene {
    std::vector<material_instance> materials;

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

        mi.m0.diffuse_tex = info.diffuse_map ? video::get_texture(info.diffuse_map) : info.diffuse_tex.id != 0 ? info.diffuse_tex : video::default_check_texture();
        mi.m0.specular_tex = info.specular_map ? video::get_texture(info.specular_map) : info.specular_tex.id != 0 ? info.specular_tex : video::default_white_texture();
        mi.m0.gloss_tex = info.gloss_map ? video::get_texture(info.gloss_map) : info.gloss_tex.id != 0 ? info.gloss_tex : video::default_white_texture();
        mi.m0.emission_tex = info.emission_map ? video::get_texture(info.emission_map) : info.emission_tex.id != 0 ? info.emission_tex : video::default_black_texture();

        materials.push_back(mi);

        application::debug(application::log_category::scene, "Create material '%' % (%)\n", info.name, mi.name_hash, materials.size());

        return &materials.back();
    }

    auto default_material() -> material_instance* {
        return nullptr;
    }

    auto get_material(const char *name) -> material_instance* {
        uint64_t hash =  utils::xxhash64(name, strlen(name), 0);

        auto it = std::find_if(materials.begin(), materials.end(), [hash](const material_instance &i) {
            if (i.name_hash == hash)
                return true;
            return false;
        });

        if (it != materials.end())
            return &(*it);

        application::warning(application::log_category::scene, "Material '%' not found\n", name);

        return default_material();
    }
} // namespace scene
