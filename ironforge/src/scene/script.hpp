#pragma once

namespace scene {
    struct script_instance {

    };

    struct script_info {
        const char  *name;
        const char  *source;
        const char  *class_name;
    };

    struct script {
        char        *name;
        char        *source;
        char        *table;
        int32_t     entity;
        uint32_t    flags;
    };
} // namespace scene
