#pragma once

#include <string>
#include <memory>
#include <ironforge_common.hpp>
#include <renderer/renderer.hpp>

namespace scene {
    enum class flags : uint32_t {
        start   = 0x00000001,
        current = 0x00000002
    };

    struct instance {
        instance();
        instance(const std::string& _name);
        ~instance();

        std::string     name;
        uint32_t        flags;
    };

    auto load(const std::string& name, uint32_t flags) -> std::unique_ptr<instance>;
    auto update(std::unique_ptr<instance>& s, float dt) -> void;
    auto present(std::unique_ptr<instance>& s, std::unique_ptr<renderer::instance> &render, float interpolation) -> void;
} // namespace scene
