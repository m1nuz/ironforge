#include <core/common.hpp>
#include <core/journal.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>

#include "transform.hpp"
#include "physics.hpp"

namespace scene {
    auto create_transforms(const uint32_t entity, const uint32_t parent) -> std::optional<transform_instance> {
        using namespace glm;
        using namespace game;

        journal::info(journal::_SCENE, "Create transform % parent %", entity, parent);

        return transform_instance{entity, parent, mat4(1.f)};
    }

    auto present_all_transforms(instance_t &sc, std::function<void(uint32_t, const glm::mat4 &)> cb) -> void {
        using namespace glm;

        for (auto &tr : sc.transforms) {
            const auto ix = tr.first;
            auto &t = tr.second;
            const auto &b = sc.bodies[ix];

            auto model = translate(mat4{1.f}, b.position());
            model = rotate(model, b.orientation().x, vec3{1.f, 0.f, 0.f});
            model = rotate(model, b.orientation().y, vec3{0.f, 1.f, 0.f});
            model = rotate(model, b.orientation().z, vec3{0.f, 0.f, 1.f});
            model = scale(model, b.size());

            auto &p = sc.transforms[t.parent];
            t.model = p.model * model;

            cb(ix, t.model);
        }
    }
} // namespace scene
