#include <vector>

#include <core/json.hpp>
#include <core/journal.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <scene/errors.hpp>
#include <scene/instance.hpp>

namespace scene {
    auto load(assets::instance_t &asset, video::instance_t &vi, const std::string &path) -> load_result {
        using namespace std;
        using namespace game;

        auto scene_contents = assets::get_text(asset, path);

        if (!scene_contents)
            return make_error_code(errc::load_scene);

        journal::debug(journal::_SCENE, "Load scene %", path);

        auto contents = scene_contents.value();

        auto j = json::parse(contents);

        instance_t sc;

        const auto name = j.find("name") != j.end() ? j["name"].get<string>() : "unknown";
        const auto version = j.find("version") != j.end() ? j["version"].get<string>() : "unknown";

        journal::info(journal::_SCENE, "Create scene :\n\tname '%'\n\tversion: %", name, version);

        if (j.find("textures") != j.end()) {
            for (const auto &tex_info : j["textures"]) {
                video::create_texture(asset, vi, tex_info);
            }
        }

        if (j.find("effects") != j.end()) {
            for (auto &eff : j["effects"]) {
                const auto effect_name = eff.find("name") != eff.end() ? eff["name"].get<string>() : string{};
                const auto effect_type = eff.find("type") != eff.end() ? eff["type"].get<string>() : string{};

                if (effect_type == "skybox") {
                    const auto cubemap = eff.find("cubemap") != eff.end() ? eff["cubemap"].get<string>() : string{};

                    sc.skybox = video::get_texture(vi, cubemap);
                }

                if (effect_type == "shader") {
                    video::create_program(asset, vi, eff);
                }
            }
        }

        if (j.find("materials") != j.end()) {
            for (auto &mat : j["materials"]) {
                const auto material_name = mat.find("name") != mat.end() ? mat["name"].get<string>() : string{};

                const auto m = create_material(vi, mat);
                if (!m) {
                    journal::warning(journal::_SCENE, "Can't create '%' material", material_name);
                    continue;
                }

                cache_material(sc, material_name, m.value());
            }
        }

        if (j.find("models") != j.end()) {
            for (auto &md : j["models"]) {
                const auto model_name = md.find("name") != md.end() ? md["name"].get<string>() : string{};

                if (md.find("meshes") == md.end()) {
                    journal::warning(journal::_SCENE, "Model '%' not contain meshes", model_name);
                    continue;
                }

                if (auto it = sc.all_models.find(model_name); it != sc.all_models.end()) {
                    journal::info(journal::_SCENE, "Dublicate '%' model", model_name);
                    continue;
                }

                vector<video::mesh> meshes;
                meshes.reserve(md["meshes"].size());
                for (auto &msh : md["meshes"]) {
                    auto m = video::create_mesh(asset, vi, msh);
                    if (m)
                        meshes.push_back(m.value());
                }

                if (!meshes.empty()) {
                    auto m = create_model(meshes);
                    if (m)
                        sc.all_models.emplace(model_name, m.value());
                }
            }
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
                        const auto on_keydown = (inp_action.find("on_keydown") != inp_action.end() )? inp_action["on_keydown"].get<string>() : string{};
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
        }

        json root_info;
        root_info["name"] = "root";

        const auto r = create_entity(asset, vi, sc, root_info);
        journal::info(journal::_SCENE, "Create root entity %", r);

        if (j.find("nodes") != j.end()) {
            for (auto &n : j["nodes"]) {
                const auto e = create_entity(asset, vi, sc, n);

                journal::info(journal::_SCENE, "Create entity %", e);
            }
        } else {
            journal::warning(journal::_SCENE, "%", "Empty scene");
        }

        return sc;
    }
}
