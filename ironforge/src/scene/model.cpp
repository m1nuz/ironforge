#include <algorithm>

#include <ironforge_utility.hpp>
#include <core/journal.hpp>
#include <video/video.hpp>

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
        mi.name_hash = utils::xxhash64(info.name, strlen(info.name), 0);
        mi.meshes.clear();

        for (auto &msh : info.meshes) {
            mesh_instance m;
            video::vertices_info vi;

            switch (msh.source) {
            case mesh_source::file:
                break;
            case mesh_source::gen_plane:
                vi = video::vertgen::make_plane(glm::mat4{1.f});
                break;
            case mesh_source::gen_cube:
                vi = video::vertgen::make_cube(glm::mat4(1.f));
                break;
            case mesh_source::gen_sphere:
                vi = video::vertgen::make_sphere(&msh.sphere, glm::mat4(1.f));
                break;
            case mesh_source::gen_grid:
            {
                video::heightmap_t height_map;
                /*if (msh.height_map)
                    height_map = video::get_heightmap(msh.height_map);*/

                vi = video::vertgen::make_grid_plane(&msh.grid, glm::mat4(1.f), height_map);
                break;
            }
            default:
                game::journal::warning(game::journal::_VIDEO, "%", "Unknown mesh source");
                break;
            }

            //m.bounds = calc_bound_box({vi.data}, vi.desc);
            //m.visible_bound = calc_bound_sphere({vi.data}, vi.desc);
            m.desc = vi.desc;
            m.source = video::make_vertices_source({vi.data}, vi.desc, m.draws);

            mi.meshes.push_back(m);
        }

        models.push_back(mi);

        game::journal::debug(game::journal::_SCENE, "Create model '%' %", info.name, mi.name_hash);

        return &models.back();
    }

    auto default_model() -> model_instance* {
        return nullptr;
    }

    auto find_model(const char *name) -> model_instance* {
        uint64_t hash = utils::xxhash64(name, strlen(name), 0);

        auto it = std::find_if(models.begin(), models.end(), [hash](const model_instance &i) {
            if (i.name_hash == hash)
                return true;
            return false;
        });

        if (it != models.end())
            return &(*it);

        game::journal::warning(game::journal::_SCENE, "Model '%' not found", name);

        return default_model();
    }
} // namespace scene
