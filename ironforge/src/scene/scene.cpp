#include <core/application.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <renderer/renderer.hpp>

#include <jansson.h>

#define json_error_if(obj, pred_fn, ret, root, ...) \
    if(pred_fn(obj)) { \
    application::error(application::log_category::game, __VA_ARGS__); \
    json_decref(root); \
    }

namespace scene {
    auto create_material(const material_info &info) -> material_instance* {
        application::debug(application::log_category::scene, "Create material %\n", info.name);

        return nullptr;
    }
} // namespace scene

namespace scene {
    auto create_model(const model_info &info) -> model_instance* {
        application::debug(application::log_category::scene, "Create model %\n", info.name);

        return nullptr;
    }
} // namespace scene

namespace scene {
    instance::instance() : instance{"empty"} {
    }

    instance::instance(const std::string& _name) : name{_name} {
        application::debug(application::log_category::game, "Create '%' scene\n", name);
    }

    instance::~instance() {
        application::debug(application::log_category::game, "Destroy '%' scene\n", name);
    }

    struct simple_instance : public instance {
        simple_instance() : instance() {

        }

        simple_instance(const std::string& _name) : instance{_name} {

        }
    };

    inline glm::vec3 json_vec3_value(json_t *arr) {
        if (json_array_size(arr) < 3) {
            application::warning(application::log_category::system, "%\n", "array size is not 3 elements");
            return {0.f, 0.f, 0.f};
        }

        auto value = glm::vec3{0.f, 0.f, 0.f};
        json_t *values[3];
        auto k = static_cast<size_t>(0);
        json_array_foreach(arr, k, values[k]) {
            if (k > 3)
                break;

            value[k] = json_real_value(values[k]);
        }

        return value;
    }

    auto load(const std::string& _name, uint32_t flags) -> std::unique_ptr<instance> {
        auto t = assets::get_text(_name);

        // TODO: make error when t.text == null

        json_error_t error;
        auto root = json_loadb(t.text, t.size, 0, &error);
        json_error_if(root, !json_is_object, -1, root, "%\n", "root is not an object");

        auto scene_name = json_object_get(root, "name");
        json_error_if(scene_name, !json_is_string, -1, root, "%\n", "scene name is not a string");

        auto scene_version = json_object_get(root, "version");
        json_error_if(scene_version, !json_is_string, -1, root, "%\n", "scene version is not a string");

        // read effects
        auto effects = json_object_get(root, "effects");
        json_error_if(effects, !json_is_array, -1, root, "%\n", "effects is not an array");

        for(size_t i = 0; i < json_array_size(effects); i++) {
            auto effect = json_array_get(effects, i);
            json_error_if(effect, !json_is_object, -1, root, "%\n", "effect is not a object");

            auto name = json_object_get(effect, "name");
            json_error_if(name, !json_is_string, -1, root, "%\n", "name is not a string");

            auto type = json_object_get(effect, "type");
            json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

            application::debug(application::log_category::scene, "Effect % '%'\n", json_string_value(type), json_string_value(name));

            if (strcmp(json_string_value(type), "ambient_light") == 0) {
                auto ambient = json_object_get(effect, "ambient");
                json_error_if(ambient, !json_is_array, -1, root, "%\n", "ambient is not an array");

                auto La = json_vec3_value(ambient);

                //render_set_ambientlight(&(AMBIENT_LIGHT){.La = {La[0], La[1], La[2]}});
                application::debug(application::log_category::scene, "Ambient light % % %\n", La.x, La.y, La.z);
            }

            if (strcmp(json_string_value(type), "directional_light") == 0) {
                auto diffuse = json_object_get(effect, "diffuse");
                json_error_if(diffuse, !json_is_array, -1, root, "%\n", "diffuse is not an array");
                auto specular = json_object_get(effect, "specular");
                json_error_if(specular, !json_is_array, -1, root, "%\n", "specular is not an array");
                auto direction = json_object_get(effect, "direction");
                json_error_if(direction, !json_is_array, -1, root, "%\n", "direction is not an array");

                auto Ld = json_vec3_value(diffuse);
                auto Ls = json_vec3_value(specular);
                auto d = json_vec3_value(direction);

                /*render_set_directionallight(&(DIRECTIONAL_LIGHT){.Ld = {Ld[0], Ld[1], Ld[2]},
                                                                 .Ls = {Ls[0], Ls[1], Ls[2]},
                                                                 .direction = {d[0], d[1], d[2]}});*/

                application::debug(application::log_category::scene, "Diffuse light d% d% d% s% s% s% % % %\n",
                                   Ld.x, Ld.y, Ld.z, Ls.x, Ls.y, Ls.z, d.x, d.y, d.z);
            }
        }

        // read materials
        auto materials = json_object_get(root, "materials");
        json_error_if(materials, !json_is_array, -1, root, "%\n", "materials is not an array");

        for(size_t i = 0; i < json_array_size(materials); i++) {
            auto material = json_array_get(materials, i);
            json_error_if(material, !json_is_object, -1, root, "%\n", "material is not an object");

            auto name = json_object_get(material, "name");
            json_error_if(name, !json_is_string, -1, root, "%\n", "name is not a string");

            auto type = json_object_get(material, "type");
            json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

            application::debug(application::log_category::scene, "Material % %\n", json_string_value(type), json_string_value(name));

            if (strcmp(json_string_value(type), "phong") == 0) {
                auto ambient = json_object_get(material, "ambient");
                auto diffuse = json_object_get(material, "diffuse");
                auto specular = json_object_get(material, "specular");
                auto emission = json_object_get(material, "emission");

                auto diffuse_map = json_object_get(material, "diffuse_map");
                auto specular_map = json_object_get(material, "specular_map");
                auto gloss_map = json_object_get(material, "gloss_map");
                auto emission_map = json_object_get(material, "emission_map");

                material_info mi;
                memset(&mi, 0, sizeof mi);
                mi.name = json_string_value(name);

                if (json_is_array(ambient))
                    mi.ambient = json_vec3_value(ambient);

                if (json_is_array(diffuse))
                    mi.diffuse = json_vec3_value(diffuse);

                if (json_is_array(specular))
                    mi.specular = json_vec3_value(specular);

                if (json_is_array(emission))
                    mi.emission = json_vec3_value(emission);

                if (json_is_string(diffuse_map))
                    mi.diffuse_map = json_string_value(diffuse_map);

                if (json_is_string(specular_map))
                    mi.specular_map = json_string_value(specular_map);

                if (json_is_string(gloss_map))
                    mi.gloss_map = json_string_value(gloss_map);

                if (json_is_string(emission_map))
                    mi.emission_map = json_string_value(emission_map);

                create_material(mi);
            }
        }

        // read models
        auto models = json_object_get(root, "models");
        json_error_if(models, !json_is_array, -1, root, "%\n", "models is not an array");

        for(size_t i = 0; i < json_array_size(models); i++) {
            auto model = json_array_get(models, i);
            json_error_if(model, !json_is_object, -1, root, "%\n", "model is not an object");

            auto name = json_object_get(model, "name");
            json_error_if(name, !json_is_string, -1, root, "%\n", "name is not a string");

            application::debug(application::log_category::scene, "Model %\n", json_string_value(name));

            auto meshes = json_object_get(model, "meshes");
            json_error_if(meshes, !json_is_array, -1, root, "%\n", "meshes is not an array");

            model_info mi;
            mi.name = json_string_value(name);
            mi.meshes.reserve(json_array_size(meshes));
            memset(&mi.meshes[0], 0, mi.meshes.capacity() * sizeof (mesh_info));

            for (size_t j = 0; j < json_array_size(meshes); j++) {
                auto mesh = json_array_get(meshes, j);
                json_error_if(mesh, !json_is_object, -1, root, "%\n", "mesh is not an object");

                auto type = json_object_get(mesh, "type");
                json_error_if(type, !json_is_string, -1, root, "%\n", "type is not a string");

                application::debug(application::log_category::scene, "Mesh %\n", json_string_value(type));

                if (strcmp(json_string_value(type), "gen_sphere") == 0) {
                    auto radius = json_object_get(mesh, "radius");
                    json_error_if(radius, !json_is_real, -1, root, "%s\n", "radius is not a real");
                    auto rings = json_object_get(mesh, "rings");
                    json_error_if(rings, !json_is_integer, -1, root, "%s\n", "rings is not an int");
                    auto sectors = json_object_get(mesh, "sectors");
                    json_error_if(sectors, !json_is_integer, -1, root, "%s\n", "sectors is not an int");

                    mi.meshes[j].source = mesh_source::gen_sphere;
                    mi.meshes[j].sphere.radius = json_real_value(radius);
                    mi.meshes[j].sphere.rings = json_integer_value(rings);
                    mi.meshes[j].sphere.sectors = json_integer_value(sectors);
                }

                if (strcmp(json_string_value(type), "gen_cube") == 0) {
                    mi.meshes[j].source = mesh_source::gen_cube;
                }

                if (strcmp(json_string_value(type), "gen_plane") == 0) {
                    auto horizontal_extend = json_object_get(mesh, "horizontal_extend");
                    json_error_if(horizontal_extend, !json_is_number, -1, root, "%s\n", "horizontal_extend is not a number");

                    auto vertical_extend = json_object_get(mesh, "vertical_extend");
                    json_error_if(vertical_extend, !json_is_number, -1, root, "%s\n", "vertical_extend is not a number");

                    auto rows = json_object_get(mesh, "rows");
                    json_error_if(rows, !json_is_integer, -1, root, "%s\n", "rows is not an int");

                    auto columns = json_object_get(mesh, "columns");
                    json_error_if(columns, !json_is_integer, -1, root, "%s\n", "columns is not an int");

                    auto triangle_strip = json_object_get(mesh, "triangle_strip");

                    mi.meshes[j].source = mesh_source::gen_grid;
                    mi.meshes[j].grid.horizontal_extend = json_number_value(horizontal_extend);
                    mi.meshes[j].grid.vertical_extend = json_number_value(vertical_extend);
                    mi.meshes[j].grid.rows = json_integer_value(rows);
                    mi.meshes[j].grid.columns = json_integer_value(columns);
                    mi.meshes[j].grid.triangle_strip = json_is_true(triangle_strip);
                }
            }

            create_model(mi);
        }

        return make_unique<simple_instance>(_name);
    }

    auto update(std::unique_ptr<instance>& s, float dt) -> void {
    }

    auto present(std::unique_ptr<instance>& s, std::unique_ptr<renderer::instance> &render, float interpolation) -> void {

    }
} // namespace scene
