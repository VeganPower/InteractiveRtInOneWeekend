#pragma once
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

#include "hittable.h"

#include <memory>
#include <vector>
#include <cassert>


class hittable_list {
    public:
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        bool hit(const ray& r, Real t_min, Real t_max, hit_record& rec) const;

        void update_bounds();
        // virtual Real pdf_value(const vec3 &o, const vec3 &v) const override;
        // virtual vec3 random(const vec3 &o, std::mt19937& random_gen) const override;

    public:
        std::vector<shared_ptr<hittable>> objects;
        std::vector<aabb> bounds;
};

inline bool hittable_list::hit(const ray& r, Real t_min, Real t_max, hit_record& rec) const {
    hit_record temp_rec;
    auto hit_anything = false;
    auto closest_so_far = t_max;

    for (size_t i = 0 ; i < objects.size(); ++i)
    {
        const auto& object = objects[i];
        if (bounds[i].hit(r, t_min, t_max) && object->hit(r, t_min, closest_so_far, temp_rec))
        // if (object->hit(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

inline void hittable_list::update_bounds()
{
    bounds.clear();
    for (const auto& obj : objects)
    {
        aabb bound = obj->local_bounding_box();
        vec3 low = obj->local_to_world(bound.min());
        vec3 high = low;
        vec3 p;
        p = obj->local_to_world(vec3(bound.min().x(), bound.min().z(), bound.max().z())); low = elem_min(low, p); high = elem_max(high, p);
        p = obj->local_to_world(vec3(bound.min().x(), bound.max().z(), bound.min().z())); low = elem_min(low, p); high = elem_max(high, p);
        p = obj->local_to_world(vec3(bound.min().x(), bound.max().z(), bound.max().z())); low = elem_min(low, p); high = elem_max(high, p);
        p = obj->local_to_world(vec3(bound.max().x(), bound.min().z(), bound.min().z())); low = elem_min(low, p); high = elem_max(high, p);
        p = obj->local_to_world(vec3(bound.max().x(), bound.min().z(), bound.max().z())); low = elem_min(low, p); high = elem_max(high, p);
        p = obj->local_to_world(vec3(bound.max().x(), bound.max().z(), bound.min().z())); low = elem_min(low, p); high = elem_max(high, p);
        p = obj->local_to_world(bound.max());                                             low = elem_min(low, p); high = elem_max(high, p);

        bounds.push_back(aabb { low, high });
    }
}

// inline bool hittable_list::bounding_box(aabb& output_box) const {
//     if (objects.empty()) return false;
//     aabb temp_box;
//     bool first_box = true;
//     for (const auto& object : objects) {
//         if (!object->bounding_box(temp_box)) return false;
//         output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
//         first_box = false;
//     }
//     return true;
// }
//
// inline Real hittable_list::pdf_value(const point3& o, const vec3& v) const {
//     auto weight = Real{1.0}/objects.size();
//     auto sum = Real{0.0};
//
//     for (const auto& object : objects)
//         sum += weight * object->pdf_value(o, v);
//
//     return sum;
// }
//
// inline vec3 hittable_list::random(const vec3 &o, std::mt19937& random_gen) const {
//     auto int_size = static_cast<int>(objects.size());
//     std::uniform_int_distribution<> uniform(0, int_size-1);
//     int idx = uniform(random_gen);
//     assert(idx >= 0 && idx < int_size);
//     return objects[idx]->random(o, random_gen);
// }

