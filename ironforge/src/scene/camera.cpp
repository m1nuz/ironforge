#include <core/journal.hpp>
#include <video/video.hpp>
#include <scene/instance.hpp>
#include "camera.hpp"
#include "physics.hpp"

namespace scene {
    auto create_camera(const uint32_t entity, const json &info, const float aspect_ratio) -> std::optional<camera_instance> {
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
            ci.projection = perspective(ci.fov, aspect_ratio, ci.znear, ci.zfar);
            break;
        default:
            ci.projection = mat4{1.f};
        }

        ci.view = mat4{1.f};

        journal::info(journal::_SCENE, "Create camera %", entity);

        return ci;
    }

    auto present_all_cameras(instance_t& sc, const float aspect_ratio) -> void {
        using namespace game;
        using namespace glm;

        for (auto& [ix, c] : sc.cameras) {
            switch (c.type) {
            case camera_type::root:
                c.projection = mat4{1.f};
                break;
            case camera_type::perspective:
                c.projection = perspective(c.fov, aspect_ratio, c.znear, c.zfar);
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
