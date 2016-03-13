#include <algorithm>

#include <core/application.hpp>
#include <video/video.hpp>
#include <xxhash.h>

#include "model.hpp"

namespace scene {
    std::vector<model_instance> models;

    auto init_all_models() -> void {

    }

    auto cleanup_all_models() -> void {

    }

    auto create_model(const model_info &info) -> model_instance* {
        model_instance mi;
        mi.name = info.name;
        mi.name_hash = XXH64(info.name, strlen(info.name), 0);
        mi.meshes.clear();

        for (auto &msh : info.meshes) {
            mesh_instance m;
            video::vertices_info vi;

            switch (msh.source) {
            case mesh_source::file:
                break;
            case mesh_source::gen_cube:
                vi = video::vertgen::make_cube(glm::mat4(1.f));
                break;
            case mesh_source::gen_sphere:
                vi = video::vertgen::make_sphere(&msh.sphere, glm::mat4(1.f));
                break;
            case mesh_source::gen_grid:
                vi = video::vertgen::make_grid_plane(&msh.grid, glm::mat4(1.f));
                break;
            default:
                break;
            }

            //m.bounds = calc_bound_box({vi.data}, vi.desc);
            //m.visible_bound = calc_bound_sphere({vi.data}, vi.desc);
            m.desc = vi.desc;
            m.source = video::make_vertices_source({vi.data}, vi.desc, m.draws); // TODO: return valid value

            mi.meshes.push_back(m);
        }

        models.push_back(mi);

        application::debug(application::log_category::scene, "Create model '%' %\n", info.name, mi.name_hash);

        return &models.back();
    }

    auto default_model() -> model_instance* {
        return nullptr;
    }

    auto get_model(const char *name) -> model_instance* {
        uint64_t hash = XXH64(name, strlen(name), 0);

        auto it = std::find_if(models.begin(), models.end(), [hash](const model_instance &i) {
            if (i.name_hash == hash)
                return true;
            return false;
        });

        if (it != models.end())
            return &(*it);

        application::warning(application::log_category::scene, "Model '%' not found\n", name);

        return default_model();
    }
} // namespace scene
