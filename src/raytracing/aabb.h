#ifndef AABB_H
#define AABB_H
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


class aabb {
    public:
        aabb() = default;
        aabb(const point3& a, const point3& b) { minimum = a; maximum = b; }

        point3 min() const {return minimum; }
        point3 max() const {return maximum; }

        bool hit(const ray& r, Real ray_t_min, Real ray_t_max) const
        {
            vec3 t_lower = (minimum - r.orig) * r.inv_dir;
            vec3 t_upper = (maximum - r.orig) * r.inv_dir;
            vec3 t_min = elem_min(t_lower, t_upper);
            vec3 t_max = elem_max(t_lower, t_upper);
            Real box_min = max_of(t_min.biggest_element(), ray_t_min);
            Real box_max = min_of(t_max.smallest_element(), ray_t_max);
            return box_min <= box_max;
        }

        double area() const {
            auto a = maximum.x() - minimum.x();
            auto b = maximum.y() - minimum.y();
            auto c = maximum.z() - minimum.z();
            return 2*(a*b + b*c + c*a);
        }

        int longest_axis() const {
            auto a = maximum.x() - minimum.x();
            auto b = maximum.y() - minimum.y();
            auto c = maximum.z() - minimum.z();
            if (a > b && a > c)
                return 0;
            else if (b > c)
                return 1;
            else
                return 2;
        }

    public:
        point3 minimum;
        point3 maximum;
};

inline aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small(fmin(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));

    vec3 big  (fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));

    return aabb(small,big);
}


#endif
