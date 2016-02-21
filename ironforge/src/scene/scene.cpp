#include <core/application.hpp>
#include <scene/scene.hpp>

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

    auto load(const std::string& name, uint32_t flags) -> std::unique_ptr<instance> {
        return make_unique<simple_instance>(name);
    }

    auto update(std::unique_ptr<instance>& s, float dt) -> void {
    }

    auto present(std::unique_ptr<instance>& s, std::unique_ptr<renderer::instance> &render, float interpolation) -> void {

    }
} // namespace scene
