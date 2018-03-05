#include <algorithm>

#include <utility/hash.hpp>
#include <core/journal.hpp>
#include <video/video.hpp>

#include "model.hpp"

namespace scene {
//    auto create_model(const std::string &name, const std::vector<mesh_info> &meshes) -> model_instance {
//        using namespace game;

//        journal::info(journal::_SCENE, "Model '%'", name);

//        model_instance mi;
//        mi.name = name;
//        mi.name_hash = utils::xxhash64(name);
//        mi.meshes.reserve(meshes.size());

//        for (auto &msh : meshes) {
//            mesh_instance m;
//            video::vertices_info vi;

//            switch (msh.source) {
//            case mesh_source::file:
//                break;
//            case mesh_source::gen_plane:
//                vi = video::vertgen::make_plane(glm::mat4{1.f});
//                break;
//            case mesh_source::gen_cube:
//                vi = video::vertgen::make_cube(glm::mat4(1.f));
//                break;
//            case mesh_source::gen_sphere:
//                vi = video::vertgen::make_sphere(&msh.sphere, glm::mat4(1.f));
//                break;
//            case mesh_source::gen_grid:
//            {
//                video::heightmap_t height_map;
//                if (msh.height_map)
//                    height_map = video::get_heightmap(msh.height_map);

//                vi = video::vertgen::make_grid_plane(&msh.grid, glm::mat4(1.f), height_map);
//                break;
//            }
//            default:
//                journal::warning(journal::_VIDEO, "%", "Unknown mesh source");
//                break;
//            }

//            //m.bounds = calc_bound_box({vi.data}, vi.desc);
//            //m.visible_bound = calc_bound_sphere({vi.data}, vi.desc);
//            m.desc = vi.desc;
//            m.source = video::make_vertices_source({vi.data}, vi.desc, m.draws);

//            mi.meshes.push_back(m);
//        }

//        return mi;
//    }

    auto create_model(const std::vector<video::mesh> &meshes) -> std::optional<model_instance> {
        if (meshes.empty())
            return {};

        model_instance model;

        for (const auto &m : meshes) {
            model.meshes.push_back(m);
        }

        return model;
    }
} // namespace scene
