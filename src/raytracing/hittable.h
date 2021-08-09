#ifndef HITTABLE_H
#define HITTABLE_H
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

#include "aabb.h"
#include "Mat3.h"
#include "../angle.h"

struct Material;

struct hit_record
{
    point3 p;
    vec3 normal;
    Material const* mat_ptr;
    Real t;
    Real u;
    Real v;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

struct hittable
{
    hittable()
    {
        rotation = Mat3::identity();
        rotation_inv = Mat3::identity();
        translation = vec3::zero();
        translation_inv = vec3::zero();
    }

    aabb bounding_box() const
    {
        aabb ret{};

        // output_box = bound;
        return ret;
    }

    virtual Real pdf_value(const vec3& o, const vec3& v) const {
        return 0.0;
    }

    virtual vec3 random(const vec3& o, std::mt19937& random_gen) const {
        return vec3(1,0,0);
    }

    hittable& translate(point3 const& T)
    {
        translation_inv = -T * rotation_inv;
        return *this;
    }

    hittable& transform(point3 const& T, Mat3 const& R)
    {
        translation = T;
        rotation = R;
        rotation_inv = transpose(R);
        translation_inv = -T * rotation_inv;
        return *this;
    }

    bool hit(const ray& r, Real t_min, Real t_max, hit_record& rec) const
    {
        ray r_start((r.origin() * rotation_inv + translation_inv), r.direction() * rotation_inv);
        if (!hit_priv(r_start, t_min, t_max, rec))
            return false;
        rec.p = r.at(rec.t);
        rec.normal = rec.normal * rotation;
        rec.set_face_normal(r, rec.normal);
        return true;
    }

protected:
    virtual bool hit_priv(const ray& r, Real t_min, Real t_max, hit_record& rec) const = 0;

    Mat3 rotation;
    Mat3 rotation_inv;
    vec3 translation;
    vec3 translation_inv;
};

#endif
