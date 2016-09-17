#pragma once

namespace scene {
    enum class entity_flags : uint32_t {
        none            = 0x00000000,
        root            = 0x00000001,
        camera          = 0x00000002,
        current_camera  = 0x00000004,
        renderable      = 0x00000008,
        visible         = 0x00000010,
        call_init       = 0x00001000,
        call_done       = 0x00002000,
        call_update     = 0x00004000
    };

    inline entity_flags operator |(entity_flags a, entity_flags b) {
        return static_cast<entity_flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    /*inline entity_flags operator &(entity_flags a, entity_flags b)
    {
        return static_cast<entity_flags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline entity_flags& operator |=(entity_flags& a, entity_flags b)
    {
        return a= a |b;
    }*/

    struct material_info;
    struct mesh_info;
    struct model_info;
    struct camera_info;
    struct script_info;
    struct body_info;
    struct light_info;
    struct emitter_info;

    struct entity_info {
        const char          *name = nullptr;
        body_info           *body = nullptr;
        light_info          *light = nullptr;
        const char          *material = nullptr;
        const char          *model = nullptr;
        const char          *input = nullptr;
        script_info         *script = nullptr;
        emitter_info        *emitter = nullptr;
        camera_info         *camera = nullptr;
        int32_t             parent = 0;
        uint32_t            flags = static_cast<uint32_t>(entity_flags::call_init | entity_flags::call_done);
    };
} // namespace scene
