#include <ironforge_common.hpp>
#include <core/application.hpp>
#include <scene/scene.hpp>

#include "transform.hpp"
#include "physics.hpp"

namespace scene {
    std::vector<transform_instance> transform_instances;

    auto init_all_transforms() -> void {
        transform_instances.reserve(max_transforms);
    }

    auto create_transform(int32_t _entity, int32_t _parent) -> transform_instance* {
        application::debug(application::log_category::scene, "Create transform ID % PARENT % (%)\n", _entity, _parent, transform_instances.size());

        // TODO: error when transform_instances.size > max_transforms

        transform_instances.push_back(transform_instance{_entity, _parent, glm::mat4(1.f)});

        return &transform_instances.back();
    }

    auto present_all_transforms(std::unique_ptr<instance>& s, std::function<void(int32_t, const glm::mat4 &)> cb) -> void {
        using namespace glm;

        for (auto i = static_cast<size_t>(1); i < transform_instances.size(); i++) {
            auto& t = transform_instances[i];

            auto b = s->get_body(t.entity);

            auto model = translate(mat4(1.f), b->position());
            model = rotate(model, b->orientation().x, vec3(1.f, 0.f, 0.f));
            model = rotate(model, b->orientation().y, vec3(0.f, 1.f, 0.f));
            model = rotate(model, b->orientation().z, vec3(0.f, 0.f, 1.f));
            model = scale(model, b->size());

            auto p = s->get_transform(t.parent);
            t.model = p->model * model; // TODO: check transform, it should never be null

            cb(t.entity, t.model);
        }
    }
} // namespace scene
