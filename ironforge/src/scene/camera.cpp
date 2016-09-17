#include <core/application.hpp>
#include <video/video.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>
#include "camera.hpp"
#include "physics.hpp"

namespace scene {
    std::vector<camera_instance> cameras;

    auto init_all_cameras() -> void {
        cameras.reserve(max_cameras);
    }

    auto cleanup_all_cameras() -> void {

    }

    auto create_camera(int32_t entity, const camera_info &info) -> camera_instance* {
        using namespace glm;

        application::debug(application::log_category::scene, "Create camera ID % PARENT %\n", entity, info.parent);

        camera_instance ci;
        ci.entity = entity;
        ci.parent = info.parent;
        ci.fov = info.fov;
        ci.aspect = video::screen.aspect;
        ci.znear = info.znear;
        ci.zfar = info.zfar;

        switch (info.type) {
        case camera_type::root:
            ci.projection = glm::mat4{1.f};
            break;
        case camera_type::perspective:
            ci.projection = perspective(ci.fov, ci.aspect, ci.znear, ci.zfar);
            break;
        }

        ci.view = mat4{1.f};

        cameras.push_back(ci);

        return &cameras.back();
    }

    auto present_all_cameras(std::unique_ptr<instance>& s) -> void {
        using namespace glm;

        for (auto i = static_cast<size_t>(1); i < cameras.size(); i++) {
            auto& c = cameras[i];
            auto b = s->get_body(c.entity);

            mat4 view = translate(mat4(1.f), -b->position());
            view = rotate(view, b->orientation().x, vec3(1.f, 0.f, 0.f));
            view = rotate(view, b->orientation().x, vec3(0.f, 1.f, 0.f));
            view = rotate(view, b->orientation().x, vec3(0.f, 0.f, 1.f));

            c.view = view;
        }
    }
} // namespace scene
