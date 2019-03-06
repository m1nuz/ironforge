#pragma once

#define randf() (rand() / (float)RAND_MAX)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#pragma GCC diagnostic pop

#include <core/json.hpp>

namespace glm {

    inline void to_json( nlohmann::json &j, const glm::vec3 &p ) {
        j.array( {p.x, p.y, p.z} );
    }

    inline void from_json( const nlohmann::json &j, glm::vec3 &p ) {
        int ix = 0;
        for ( auto it = j.begin( ); it != j.end( ); ++it, ix++ ) {
            p[ix] = *it;
        }
    }

} // namespace glm
