#include <core/application.hpp>
#include <ironforge_utility.hpp>
#include <scene/instance.hpp>

namespace scene {
    instance::instance() : instance{"empty", 0} {
    }

    instance::instance(const std::string& _name, uint32_t _state) : name{_name}, name_hash{utils::xxhash64(_name)}, state{_state} {
        application::debug(application::log_category::game, "Create '%' scene %\n", name, utils::to_hex(name_hash));
    }

    instance::~instance() {
        application::debug(application::log_category::game, "Destroy '%' scene\n", name);
    }
} // namespace scene
