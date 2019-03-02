#include <core/journal.hpp>
#include <utility/hash.hpp>

#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include "input.hpp"
#include "script.hpp"

namespace scene {
    auto create_input(const uint32_t entity, const json &info, const std::unordered_map<std::string, std::vector<input_action> > &sources) -> std::optional<input_instance> {
        using namespace std;
        using namespace game;

        const auto in_source = info.get<string>();

        const auto it = sources.find(in_source);
        if (it != sources.end()) {
            input_instance in;
            in.entity = entity;
            in.source = nullptr;
            in.actions = (*it).second;

            journal::info(journal::_SCENE, "Create input %", entity);

            return in;
        }

        return {};
    }

    auto create_input_source(scene::instance_t &s, const std::string &name, const std::vector<input_action> &actions) -> bool {
        s.input_sources.emplace(name, actions);

        return true;
    }

    auto process_input_events(scene::instance_t &s, const SDL_Event &e) -> void {
        using namespace game;

        switch (e.type) {
        case SDL_KEYDOWN:
            for (const auto &[ix, input] : s.inputs) {
                (void)ix;

                for(const auto &action : input.actions)
                    if (e.key.keysym.sym == action.key)
                        if (!action.key_down.empty())
                            call_fn(&(s.get_script(input.entity)), action.key_down.c_str());
            }
            break;
        case SDL_KEYUP:
            for (const auto &[ix, input] : s.inputs) {
                (void)ix;

                for(const auto &action : input.actions)
                    if (e.key.keysym.sym == action.key)
                        if (!action.key_up.empty())
                            call_fn(&(s.get_script(input.entity)), action.key_up.c_str());
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            for (const auto &[ix, input] : s.inputs) {
                (void)ix;

                for(const auto &action : input.actions)
                    if (e.cbutton.button == action.cbutton)
                        if (!action.key_down.empty())
                            call_fn(&(s.get_script(input.entity)), action.key_down.c_str());
            }

            journal::debug(journal::_INPUT, "button=% state=%", e.cbutton.button, e.cbutton.state);
            break;
        case SDL_CONTROLLERBUTTONUP:
            for (const auto &[ix, input] : s.inputs) {
                (void)ix;

                for(const auto &action : input.actions)
                    if (e.cbutton.button == action.cbutton)
                        if (!action.key_up.empty())
                            call_fn(&(s.get_script(input.entity)), action.key_up.c_str());
            }

            journal::debug(journal::_INPUT, "button=% state=%", e.cbutton.button, e.cbutton.state);
            break;
        case SDL_CONTROLLERAXISMOTION:
            for (const auto &[ix, input] : s.inputs) {
                (void)ix;

                for(const auto &action : input.actions)
                    if (e.caxis.axis == action.caxis)
                        if (!action.caxis_motion.empty())
                            call_with_args(&(s.get_script(input.entity)), action.caxis_motion.c_str(), (float)e.caxis.value / INT16_MAX);
            }
            break;
        }
    }
} // namespace scene
