#pragma once

#include <functional>


namespace scene {
    struct emitter_instance {
        emitter_instance( ) = default;
    };

    using emitter_ref = std::reference_wrapper<emitter_instance>;
} // namespace scene
