#include <algorithm>

#include <core/application.hpp>
#include <xxhash.h>

#include "model.hpp"

namespace scene {
    std::vector<model_instance> models;


    auto create_model(const model_info &info) -> model_instance* {
        model_instance mi;
        mi.name = info.name;
        mi.name_hash = XXH64(info.name, strlen(info.name), 0);
        mi.meshes.clear();

        for (auto &msh : info.meshes) {
            mesh_instance m;

            switch (msh.source) {
            case mesh_source::file:
                break;
            case mesh_source::gen_cube:
                break;
            case mesh_source::gen_sphere:
                break;
            case mesh_source::gen_grid:
                break;
            default:
                break;
            }
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
