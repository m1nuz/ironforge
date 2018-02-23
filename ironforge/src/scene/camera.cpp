#include <core/journal.hpp>
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

        game::journal::debug(game::journal::_SCENE, "Create camera ID % PARENT %", entity, info.parent);

        camera_instance ci;
        ci.entity = entity;
        ci.parent = info.parent;
        ci.fov = info.fov;
        ci.type = info.type;
        ci.znear = info.znear;
        ci.zfar = info.zfar;

        switch (info.type) {
        case camera_type::root:
            ci.projection = glm::mat4{1.f};
            break;
        case camera_type::perspective:
            ci.projection = perspective(ci.fov, video::screen.aspect, ci.znear, ci.zfar);
            break;
        }

        ci.view = mat4{1.f};

        cameras.push_back(ci);

        return &cameras.back();
    }

    auto present_all_cameras(std::unique_ptr<instance>& s) -> void {
        using namespace glm;

        for (auto& c : cameras) {
            switch (c.type) {
            case camera_type::root:
                c.projection = glm::mat4{1.f};
                break;
            case camera_type::perspective:
                c.projection = perspective(c.fov, video::screen.aspect, c.znear, c.zfar);
                break;
            }

            auto b = s->get_body(c.entity);

            mat4 view = translate(mat4(1.f), -b->position());
            view = rotate(view, b->orientation().x, vec3(1.f, 0.f, 0.f));
            view = rotate(view, b->orientation().x, vec3(0.f, 1.f, 0.f));
            view = rotate(view, b->orientation().x, vec3(0.f, 0.f, 1.f));

            c.view = view;
        }
    }

    auto create_camera(const uint32_t entity, const json &info) -> std::optional<camera_instance> {
        using namespace glm;
        using namespace game;

        camera_instance ci;
        ci.entity = entity;

        if (info.find("fov") != info.end())
            ci.fov = radians(info["fov"].get<float>());

        if (info.find("zfar") != info.end())
            ci.zfar = info["zfar"].get<float>();

        if (info.find("znear") != info.end())
            ci.znear = info["znear"].get<float>();

        ci.type = camera_type::perspective;

        switch (ci.type) {
        case camera_type::root:
            ci.projection = glm::mat4{1.f};
            break;
        case camera_type::perspective:
            ci.projection = perspective(ci.fov, video::screen.aspect, ci.znear, ci.zfar);
            break;
        default:
            ci.projection = mat4{1.f};
        }

        ci.view = mat4{1.f};

        journal::info(journal::_SCENE, "Create camera %", entity);

        return ci;
    }

    auto present_all_cameras(instance_t& sc) -> void {
        using namespace game;
        using namespace glm;

        for (auto& [ix, c] : sc.cameras) {
            switch (c.type) {
            case camera_type::root:
                c.projection = mat4{1.f};
                break;
            case camera_type::perspective:
                c.projection = perspective(c.fov, video::screen.aspect, c.znear, c.zfar);
                break;
            }

            auto b = sc.bodies[ix];

            mat4 view = translate(mat4(1.f), -b.position());
            view = rotate(view, b.orientation().x, vec3(1.f, 0.f, 0.f));
            view = rotate(view, b.orientation().x, vec3(0.f, 1.f, 0.f));
            view = rotate(view, b.orientation().x, vec3(0.f, 0.f, 1.f));

            c.view = view;
        }
    }
} // namespace scene
