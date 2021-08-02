#ifndef CAMERA_H
#define CAMERA_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

class RtCamera {
public:
    RtCamera() = default;

    RtCamera(glm::mat4 const& proj_view_inv)
    {
        glm::vec4 o = proj_view_inv * glm::vec4( 0.0,  0.0, -1.0, 1.0);
        glm::vec4 c = proj_view_inv * glm::vec4(-1.0, -1.0,  0.0, 1.0);
        glm::vec4 x = proj_view_inv * glm::vec4( 1.0, -1.0,  0.0, 1.0);
        glm::vec4 y = proj_view_inv * glm::vec4(-1.0,  1.0,  0.0, 1.0);
        o = o / o[3];
        c = c / c[3];
        x = x / x[3];
        y = y / y[3];
        glm::vec4 h = x - c;
        glm::vec4 v = y - c;

        origin     = vec3(o[0], o[1], o[2]);
        lower_left_corner = vec3(c[0], c[1], c[2]);
        horizontal = vec3(h[0], h[1], h[2]);
        vertical   = vec3(v[0], v[1], v[2]);
    }


    ray get_ray(double s, double t) const {
        return ray(
            origin,
            lower_left_corner + s*horizontal + t*vertical - origin
        );
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};

#endif
