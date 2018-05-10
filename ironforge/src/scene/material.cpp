#include <algorithm>

#include <utility/hash.hpp>
#include <core/journal.hpp>
#include <scene/scene.hpp>
#include <video/video.hpp>

#include "material.hpp"

namespace scene {
    auto create_material(video::instance_t &vi, const json &info) -> std::optional<material_instance> {
        using namespace std;
        using namespace glm;
        using namespace game;

        const auto name = info.find("name") != info.end() ? info["name"].get<string>() : string{};

        material_instance m;
        m.m0.ka = info.find("ambient") != info.end() ? info["ambient"].get<vec3>() : vec3{};
        m.m0.kd = info.find("diffuse") != info.end() ? info["diffuse"].get<vec3>() : vec3{};
        m.m0.ks = info.find("specular") != info.end() ? info["specular"].get<vec3>() : vec3{};
        m.m0.ke = info.find("emission") != info.end() ? info["emission"].get<vec3>() : vec3{};
        m.m0.ns = info.find("shininess") != info.end() ? info["shininess"].get<float>() : 0.f;
        m.m0.tr = info.find("transparency") != info.end() ? info["transparency"].get<float>() : 0.f;
        m.m0.reflectivity = info.find("reflectivity") != info.end() ? info["reflectivity"].get<float>() : 0.f;

        m.m0.diffuse_tex = info.find("diffuse_map") != info.end() ? video::get_texture(vi, info["diffuse_map"].get<string>()) : video::get_texture(vi, "check-map");
        m.m0.specular_tex = info.find("specular_map") != info.end() ? video::get_texture(vi, info["specular_map"].get<string>()) : video::get_texture(vi, "white-map");
        m.m0.gloss_tex = info.find("gloss_map") != info.end() ? video::get_texture(vi, info["gloss_map"].get<string>()) : video::get_texture(vi, "white-map");
        m.m0.emission_tex = info.find("emission_map") != info.end() ? video::get_texture(vi, info["emission_map"].get<string>()) : video::get_texture(vi, "white-map");
        m.m0.normal_tex = info.find("normal_map") != info.end() ? video::get_texture(vi, info["normal_map"].get<string>()) : video::get_texture(vi, "white-map");

        journal::info(journal::_SCENE, "Create '%' material", name);

        return m;
    }
} // namespace scene
