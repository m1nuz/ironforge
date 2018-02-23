#include <core/journal.hpp>
#include <scene/scene.hpp>

#include "light.hpp"

namespace scene {
    std::vector<ambient_light_instance> ambient_instances;
    std::vector<directional_light_instance> directional_instances;
    std::vector<point_light_instance> point_instances;

    auto init_all_lights() -> void {
        ambient_instances.reserve(1);
        directional_instances.reserve(1);
        point_instances.reserve(max_points_lights);
    }

    auto cleanup_all_lights() -> void {
        ambient_instances.clear();
        directional_instances.clear();
        point_instances.clear();
    }

    static auto create_ambient_light(const ambient_light_info &info) -> ambient_light_instance* {
        auto ai = ambient_light_instance{info.ambient};

        ambient_instances.push_back(ai);

        game::journal::debug(game::journal::_SCENE, "%", "Create ambient light");

        return &ambient_instances.back();
    }

    static auto create_directional_light(const directional_light_info &info) -> directional_light_instance* {
        auto di = directional_light_instance{info.direction, info.diffuse, info.specular};

        directional_instances.push_back(di);

        game::journal::debug(game::journal::_SCENE, "%", "Create directional light");

        return &directional_instances.back();
    }

    static auto create_point_light(int32_t entity, const point_light_info &info) -> point_light_instance* {
        auto pi = point_light_instance{info.diffuse, info.specular, info.attenuation, entity};

        point_instances.push_back(pi);

        game::journal::debug(game::journal::_SCENE, "%", "Create point light");

        return &point_instances.back();
    }

    auto create_light(int32_t entity, const light_info &info) -> std::pair<light_type, void*> {
        switch (info.type) {
        case light_type::ambient:
            return {info.type, create_ambient_light(info.ambient_light)};
        case light_type::directional:
            return {info.type, create_directional_light(info.directional_light)};
        case light_type::point:
            return {info.type, create_point_light(entity, info.point_light)};
        default:
            return {info.type, nullptr};
        }
    }

    auto present_all_lights(std::unique_ptr<instance>& s, std::unique_ptr<renderer::instance> &r) -> void {
        UNUSED(s);

        for (const auto &al : ambient_instances)
            r->append(al);

        for (const auto &dl : directional_instances)
            r->append(dl);

        // TODO: get body of point light and send position
        for (const auto &pl : point_instances)
            r->append(pl);
    }

    auto create_light(const json &info) -> std::optional<light_t> {
        using namespace std;
        using namespace glm;
        using namespace game;

        if (info.find("type") == info.end())
            return {};

        const auto type = info["type"].get<string>();

        light_t l;

        journal::info(journal::_SCENE, "Create % light", type);

        if (type == "ambient_light" || type == "ambient") {
            const auto ambient = info.find("ambient") != info.end() ? info["ambient"].get<vec3>() : vec3{};

            journal::info(journal::_SCENE, "\tambient %", ambient);

            l = renderer::phong::ambient_light{ambient};
        }

        if (type == "directional_light" || type == "directional") {
            const auto diffuse = info.find("diffuse") != info.end() ? info["diffuse"].get<vec3>() : vec3{};
            const auto specular = info.find("specular") != info.end() ? info["specular"].get<vec3>() : vec3{};
            const auto direction = info.find("direction") != info.end() ? info["direction"].get<vec3>() : vec3{};

            journal::info(journal::_SCENE, "\tdiffuse %\n\tspecular %\n\tdirection %", diffuse, specular, direction);

            l = renderer::phong::directional_light{direction, diffuse, specular};
        }

        if (type == "point_light" || type == "point") {

        }

        return l;
    }
} // namespace scene
