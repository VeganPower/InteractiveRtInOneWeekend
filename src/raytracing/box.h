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

#include "aarect.h"
#include "hittable_list.h"


class box : public hittable  {
public:
    box() = default;
    box(const vec3& size, shared_ptr<Material> ptr);

    virtual bool hit_priv(const ray& r, Real t_min, Real t_max, hit_record& rec) const override;

    // virtual bool bounding_box(aabb& output_box) const override {
    //     output_box = aabb(box_min, box_max);
    //     return true;
    // }

private:
    point3 size;
    SubPlane sides[6];
};
