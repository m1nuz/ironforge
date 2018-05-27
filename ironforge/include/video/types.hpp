#pragma once

#include <video/video.hpp>

namespace video {

    struct vertices_source {
        gl::vertex_array    array;
        gl::buffer          vertices;
        gl::buffer          elements;
    };

    struct mesh {
        vertices_desc   desc;
        vertices_source source;
        vertices_draw   draw;
    };

} // namespace video
