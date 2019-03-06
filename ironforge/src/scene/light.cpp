#include <core/journal.hpp>
#include <scene/scene.hpp>

#include "light.hpp"

namespace scene {
    auto create_light(const json &info) -> std::optional<light_instance> {
        using namespace std;
        using namespace glm;
        using namespace game;

        if (info.find("type") == info.end())
            return {};

        const auto type = info["type"].get<string>();

        light_instance l;

        journal::info(journal::_SCENE, "Create % light", type);

        if (type == "ambient_light" || type == "ambient") {
            const auto ambient = info.find("ambient") != info.end() ? info["ambient"].get<vec3>() : vec3{};

            journal::info(journal::_SCENE, "\tambient %", glm::to_string(ambient));

            l = renderer::phong::ambient_light{ambient};
        }

        if (type == "directional_light" || type == "directional") {
            const auto diffuse = info.find("diffuse") != info.end() ? info["diffuse"].get<vec3>() : vec3{};
            const auto specular = info.find("specular") != info.end() ? info["specular"].get<vec3>() : vec3{};
            const auto direction = info.find("direction") != info.end() ? info["direction"].get<vec3>() : vec3{};

            journal::info(journal::_SCENE, "\tdiffuse %\n\tspecular %\n\tdirection %", glm::to_string(diffuse), glm::to_string(specular), glm::to_string(direction));

            l = renderer::phong::directional_light{direction, diffuse, specular};
        }

        if (type == "point_light" || type == "point") {

        }

        return l;
    }

    auto present_all_lights(instance_t& sc, std::unique_ptr<renderer::instance> &renderer) -> void {
        for (const auto &lt : sc.lights) {
            const auto l = lt.second;
            std::visit([&renderer](auto&& arg) {
                renderer->append(arg);
            }, l);
        }
    }
} // namespace scene
