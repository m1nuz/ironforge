#pragma once

#include <core/common.hpp>
#include <video/video.hpp>

namespace renderer {
    namespace phong {
        struct material {
            glm::vec3   ke; // emission
            glm::vec3   ka;
            glm::vec3   kd;
            glm::vec3   ks;
            float       ns;
            float       tr;
            float       reflectivity;
            //float       refraction;

            video::texture emission_tex;
            video::texture diffuse_tex;
            video::texture specular_tex;
            video::texture gloss_tex;
            video::texture normal_tex;
        };

        struct ambient_light {
            glm::vec3   la; // Ambient intensity
        };

        struct point_light {
            glm::vec3  ld; // Diffuse intensity
            glm::vec3  ls; // Specular intensity
            glm::vec3  attenuation; // Constant, Linear, Quadratic
        };

        struct spot_light {

        };

        struct directional_light {
            glm::vec3  direction;
            glm::vec3  ld;
            glm::vec3  ls;
        };
    } // namespace phong
} // namespace renderer
