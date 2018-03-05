#include <core/common.hpp>
#include <core/journal.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>

#include "transform.hpp"
#include "physics.hpp"

namespace scene {
//    std::vector<transform_instance> transform_instances;

//    auto init_all_transforms() -> void {
//        transform_instances.reserve(max_transforms);
//    }

//    auto cleanup_all_transforms() -> void {

//    }

//    auto create_transform(int32_t _entity, int32_t _parent) -> transform_instance* {
//        game::journal::debug(game::journal::_SCENE, "Create transform ID % PARENT % (%)", _entity, _parent, transform_instances.size());

//        // TODO: error when transform_instances.size > max_transforms

//        transform_instances.push_back(transform_instance{_entity, _parent, glm::mat4(1.f)});

//        return &transform_instances.back();
//    }

//    auto present_all_transforms(std::unique_ptr<instance>& s, std::function<void(int32_t, const glm::mat4 &)> cb) -> void {
//        using namespace glm;

//        for (auto i = static_cast<size_t>(1); i < transform_instances.size(); i++) {
//            auto& t = transform_instances[i];

//            auto b = s->get_body(t.entity);

//            auto model = translate(mat4(1.f), b->position());
//            model = rotate(model, b->orientation().x, vec3(1.f, 0.f, 0.f));
//            model = rotate(model, b->orientation().y, vec3(0.f, 1.f, 0.f));
//            model = rotate(model, b->orientation().z, vec3(0.f, 0.f, 1.f));
//            model = scale(model, b->size());

//            auto p = s->get_transform(t.parent);
//            t.model = p->model * model; // TODO: check transform, it should never be null

//            cb(t.entity, t.model);
//        }
//    }

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

            //journal::debug(journal::_SCENE, "model %", t.model);

            cb(ix, t.model);
        }
    }
} // namespace scene
