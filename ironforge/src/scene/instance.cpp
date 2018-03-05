#include <utility/hash.hpp>
#include <scene/instance.hpp>

namespace scene {

    instance_type::instance_type() {
        names.reserve(initial_name);
        materials.reserve(initial_material);
        models.reserve(initial_model);
        cameras.reserve(initial_camera);
        scripts.reserve(initial_script);
        bodies.reserve(initial_body);
        inputs.reserve(initial_input);
        transforms.reserve(initial_transform);
        emitters.reserve(initial_emitter);
        lights.reserve(initial_light);
        input_sources.reserve(max_input_sources);
    }

    auto instance_type::current_camera() -> instance_t::camera_t& {
        return cameras[current_camera_index];
    }

} // namespace scene
