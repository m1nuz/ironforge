#include <core/journal.hpp>
#include <scene/scene.hpp>
#include "physics.hpp"

namespace physics {
    using body_instance = scene::body_instance;

    inline auto integrate(body_state &state, const float dt) -> void {
        state.position += state.velocity * dt;
        state.orientation += state.rotation * dt;
    }

    inline auto interpolate(body_state &state, const body_state &previous, const body_state &current, const float alpha) -> void {
        using namespace glm;
        state.position = mix(previous.position, current.position, alpha);
        state.orientation = mix(previous.orientation, current.orientation, alpha); // TODO: wrong!
        state.size = mix(previous.size, current.size, alpha);
    }

    auto integrate_all(scene::instance_t &sc, const float dt) noexcept -> void {
        for (auto & bd : sc.bodies) {
            auto &b = bd.second;
            b.previous = b.current;
            integrate(b.current, dt);
        }
    }

    auto cleanup_all(scene::instance_t &sc) noexcept -> void {
        using namespace game;

        for (auto &[ix, b] : sc.bodies) {
            (void)b;
            journal::debug(journal::_SCENE, "Destoy body (%)", ix);
        }
    }
} // namespace physics

namespace scene {
    auto create_body(const json &info) -> std::optional<body_instance> {
        using namespace game;
        using namespace glm;

        physics::body_state state;

        if (info.find("position") != info.end())
            state.position = info["position"].get<vec3>();

        if (info.find("orientation") != info.end())
            state.orientation = info["orientation"].get<vec3>();

        if (info.find("size") != info.end())
            state.size = info["size"].get<vec3>();

        if (info.find("velocity") != info.end())
            state.velocity = info["velocity"].get<vec3>();

        if (info.find("rotation") != info.end())
            state.rotation = info["rotation"].get<vec3>();

        journal::info(journal::_SCENE, "Create body:\n\tposition %\n\torientation %\n\tsize %s\n\tvelocity %\n\trotation %",
                      state.position, state.orientation, state.size, state.velocity, state.rotation);

        return body_instance{state, state, state};
    }

    auto interpolate_all(instance_t &sc, const float interpolation) -> void {
        for (auto &sb : sc.bodies) {
            auto& b = sb.second;
            physics::interpolate(b.state, b.previous, b.current, interpolation);
        }
    }
} // namespace scene
