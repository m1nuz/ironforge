#include <vector>

#include <json.hpp>

#include <core/journal.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include <scene/entity.hpp>

namespace game {
    auto get_base_path() -> const std::string&;
}

namespace scene {
    auto load(const std::string &path) -> load_result {
        using namespace std;
        using namespace game;

        auto scene_path = game::get_base_path() + path;

        journal::debug(journal::_SCENE, "Load scene %", scene_path);

        auto contents = assets::readfile(scene_path);
        if (!assets::is_ok(contents))
            return get<error_code>(contents);

        auto j = json::parse(get<string>(contents));

        instance_t sc;

        const auto name = j.find("name") != j.end() ? j["name"].get<string>() : "unknown";
        const auto version = j.find("version") != j.end() ? j["version"].get<string>() : "unknown";

        journal::info(journal::_SCENE, "Create scene :\n\tname '%'\n\tversion: %", name, version);

        if (j.find("effects") == j.end())
            return make_error_code(errc::io_error);

        for (auto &eff : j["effects"]) {
            const auto effect_name = eff.find("name") != eff.end() ? eff["name"].get<string>() : string{};
            const auto effect_type = eff.find("type") != eff.end() ? eff["type"].get<string>() : string{};

            if (effect_type == "skybox") {
                if (eff.find("textures") != eff.end()) {
                    std::string texs[6];

                    int tix = 0;
                    for (const auto &t : eff["textures"]) {
                        texs[tix] = t.get<string>();
                        tix++;
                    }

                    video::make_texture_cube(effect_name, texs);
                }
            }
        }

        if (j.find("materials") == j.end())
            return make_error_code(errc::io_error);

        for (auto &mat : j["materials"]) {
            const auto material_name = mat.find("name") != mat.end() ? mat["name"].get<string>() : string{};

            const auto m = create_material(mat);
            if (!m) {
                journal::warning(journal::_SCENE, "Can't create '%' material", material_name);
                continue;
            }

            cache_material(sc, material_name, m.value());
        }

        if (j.find("models") == j.end())
            return make_error_code(errc::io_error);

        //vector<instance_t::model_t> all_models;
        //all_models.reserve(j["models"].size());

        for (auto &md : j["models"]) {
            const auto model_name = md.find("name") != md.end() ? md["name"].get<string>() : string{};

            if (md.find("meshes") == md.end()) {
                journal::warning(journal::_SCENE, "Model '%' not contain meshes", model_name);
                continue;
            }

            vector<mesh_info> meshe_infos;
            meshe_infos.reserve(md["meshes"].size());

            for (auto &msh : md["meshes"]) {
                mesh_info mi;

                if (msh.find("type") == msh.end()) {
                    journal::warning(journal::_SCENE, "Mesh of '%' not contain type", model_name);
                    continue;
                }

                const auto mesh_type = msh["type"].get<string>();

                // TODO: rework as variant state
                if (mesh_type == "gen_sphere") {
                    mi.source = mesh_source::gen_sphere;
                    mi.sphere.radius = msh.find("radius") != msh.end() ? msh["radius"].get<float>() : 1.f;
                    mi.sphere.rings = msh.find("rings") != msh.end() ? msh["rings"].get<uint32_t>() : 8;
                    mi.sphere.sectors = msh.find("sectors") != msh.end() ? msh["sectors"].get<uint32_t>() : 8;
                }

                if (mesh_type == "gen_cube") {
                    mi.source = mesh_source::gen_cube;
                }

                if (mesh_type == "gen_plane") {
                    mi.source = mesh_source::gen_plane;
                }

                if (mesh_type == "gen_grid") {
                    const auto horizontal_extend = msh["horizontal_extend"].get<float>();
                    const auto vertical_extend = msh["vertical_extend"].get<float>();
                    const auto rows = msh["rows"].get<uint32_t>();
                    const auto columns = msh["columns"].get<uint32_t>();
                    const auto triangle_strip = msh["triangle_strip"].get<bool>();
                    //const auto height_map = msh["height_map"].get<string>();

                    mi.source = mesh_source::gen_grid;
                    mi.grid.horizontal_extend = horizontal_extend;
                    mi.grid.vertical_extend = vertical_extend;
                    mi.grid.rows = rows;
                    mi.grid.columns = columns;
                    mi.grid.triangle_strip = triangle_strip;
                    mi.height_map = nullptr;
                }

                meshe_infos.push_back(mi);
            }

            const auto model = create_model(model_name, meshe_infos);
            if (!cache_model(sc, model_name, model))
                journal::info(journal::_SCENE, "Model '%' already in cache", model_name);
        }

        // create input objects
        if (j.find("inputs") != j.end()) {
            for (auto &inp : j["inputs"]) {
                const auto input_name = inp.find("name") != inp.end() ? inp["name"].get<string>() : string{};

                std::vector<input_action> input_actions;
                if (inp.find("actions") != inp.end()) {
                    input_actions.reserve(inp["actions"].size());

                    for (auto &inp_action : inp["actions"]) {
                        const auto key = inp_action.find("key") != inp_action.end() ? SDL_GetKeyFromName(inp_action["key"].get<string>().c_str()) : SDLK_UNKNOWN;
                        const auto cbutton = inp_action.find("cbutton") != inp_action.end() ? SDL_GameControllerGetButtonFromString(inp_action["cbutton"].get<string>().c_str()) : SDL_CONTROLLER_BUTTON_INVALID;
                        const auto caxis = inp_action.find("caxis") != inp_action.end() ? SDL_GameControllerGetAxisFromString(inp_action["caxis"].get<string>().c_str()) : SDL_CONTROLLER_AXIS_INVALID;
                        const string on_keydown = (inp_action.find("on_keydown") != inp_action.end() )? inp_action["on_keydown"].get<string>() : string{};
                        const auto on_keyup = inp_action.find("on_keyup") != inp_action.end() ? inp_action["on_keyup"].get<string>() : string{};
                        const auto on_cmotion = inp_action.find("cmotion") != inp_action.end() ? inp_action["cmotion"].get<string>() : string{};

                        input_action act;
                        act.key = key;
                        act.cbutton = cbutton;
                        act.caxis = caxis;
                        act.key_down = on_keydown;
                        act.key_up = on_keyup;
                        act.caxis_motion = on_cmotion;

                        input_actions.push_back(act);
                    }
                }

                if (!input_actions.empty())
                {
                    create_input_source(sc, input_name, input_actions);
                }

            }
        } else {
            //journal::warning(journal::_SCENE, "%", "No inputs");
        }

        json root_info;
        root_info["name"] = "root";

        const auto r = create_entity(sc, root_info);

        if (j.find("nodes") == j.end())
            return make_error_code(errc::io_error);

        for (auto &n : j["nodes"]) {
            const auto e = create_entity(sc, n);
        }        

        return sc;
    }
}
