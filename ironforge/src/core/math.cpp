#include <core/math.hpp>

namespace glm {

    void to_json( nlohmann::json &j, const glm::vec3 &p ) {
        j.array( {p.x, p.y, p.z} );
    }

    void from_json( const nlohmann::json &j, glm::vec3 &p ) {
        int ix = 0;
        for ( auto it = j.begin( ); it != j.end( ); ++it, ix++ ) {
            p[ix] = *it;
        }
    }

} // namespace glm
