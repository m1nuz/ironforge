#include <ironforge_utility.hpp>
#include <core/journal.hpp>

#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include "input.hpp"
#include "script.hpp"

namespace scene {
    static std::vector<input_source>   input_sources;
    static std::vector<input_instance> inputs;

    auto init_all_inputs() -> void {
        input_sources.reserve(max_input_sources);
        inputs.reserve(max_inputs);
    }

    auto cleanup_all_inputs() -> void {
        input_sources.clear();
        inputs.clear();
    }

    auto create_input_source(const std::string &name, const std::vector<input_action> &actions) -> input_source* {
        input_sources.push_back({name, utils::xxhash64(name), std::move(actions)});

        return &input_sources.back();
    }

    auto find_input_source(const char *name) -> input_source* {
        auto hash = utils::xxhash64(name, strlen(name));

        for (size_t i = 0; i < input_sources.size(); i++)
            if (input_sources[i].hash == hash)
                return &input_sources[i];

        return nullptr;
    }

    auto create_input(int32_t entity, input_source* source) -> input_instance* {
        if (!source)
            return nullptr;

        game::journal::debug(game::journal::category::scene, "Create input %\n", entity);

        inputs.push_back({entity, source});

        return &inputs.back();
    }

    auto process_input_events(std::unique_ptr<instance> &s, const SDL_Event &e) -> void {
        switch (e.type) {
        case SDL_KEYDOWN:
            for (const auto &input : inputs)
                for(const auto &action : input.source->actions)
                    if (e.key.keysym.sym == action.key)
                        if (!action.key_down.empty())
                            call_fn(s->get_script(input.entity), action.key_down.c_str());
            break;
        case SDL_KEYUP:
            for (const auto &input : inputs)
                for(const auto &action : input.source->actions)
                    if (e.key.keysym.sym == action.key)
                        if (!action.key_up.empty())
                            call_fn(s->get_script(input.entity), action.key_up.c_str());
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            for (const auto &input : inputs)
                for(const auto &action : input.source->actions)
                    if (e.cbutton.button == action.cbutton)
                        if (!action.key_down.empty())
                            call_fn(s->get_script(input.entity), action.key_down.c_str());
            break;
        case SDL_CONTROLLERBUTTONUP:
            for (const auto &input : inputs)
                for(const auto &action : input.source->actions)
                    if (e.cbutton.button == action.cbutton)
                        if (!action.key_up.empty())
                            call_fn(s->get_script(input.entity), action.key_up.c_str());
            break;
        case SDL_CONTROLLERAXISMOTION:
            for (const auto &input : inputs)
                for(const auto &action : input.source->actions)
                    if (e.caxis.axis == action.caxis)
                        if (!action.caxis_motion.empty())
                            call_with_args(s->get_script(input.entity), action.caxis_motion.c_str(), (float)e.caxis.value / INT16_MAX);
            break;
        }
    }
} // namespace scene
