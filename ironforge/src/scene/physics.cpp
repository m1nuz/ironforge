#include <ironforge_common.hpp>
#include <core/journal.hpp>
#include <scene/scene.hpp>
#include "physics.hpp"

namespace physics {
    using body_instance = scene::body_instance;

    std::vector<body_instance> bodies;

    inline auto integrate(body_state &state, float dt) -> void {
        state.position += state.velocity * dt;
        state.orientation += state.rotation * dt;
    }

    inline auto interpolate(body_state &state, const body_state &previous, const body_state &current, float alpha) -> void {
        using namespace glm;
        state.position = mix(previous.position, current.position, alpha);
        state.orientation = mix(previous.orientation, current.orientation, alpha); // TODO: wrong!
        state.size = mix(previous.size, current.size, alpha);
    }

    auto init_all() -> void {
        bodies.reserve(scene::max_bodies);
    }

    auto cleanup() -> void {
        for (auto i = 0ul; i < bodies.size(); ++i)
            game::journal::debug(game::journal::_SCENE, "Destoy body (%)", i);

        bodies.clear();
    }

    auto integrate_all(float dt) -> void {
        for (auto &b : bodies) {
            b.previous = b.current;
            integrate(b.current, dt);
        }
    }

    auto interpolate_all(float interpolation) -> void {
        for (auto &b : bodies)
            interpolate(b.state, b.previous, b.current, interpolation);
    }
} // namespace physics

namespace scene {
    auto create_body(const body_info &info) -> body_instance* {
        physics::body_state state;
        state.position = info.position;
        state.orientation = info.orientation;
        state.size = info.size;
        state.velocity = info.velocity;
        state.rotation = info.rotation;

        physics::bodies.push_back(body_instance{state, state, state});

        game::journal::debug(game::journal::_SCENE, "Create body (%)", physics::bodies.size());

        return &physics::bodies.back();
    }
} // namespace scene
