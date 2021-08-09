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

struct Rectangle
{
    Real x0, x1, y0, y1;
    bool outside(Real x, Real y) const
    {
        return x < x0 || x > x1 || y < y0 || y > y1;
    }
    Real area() const
    {
        return (x1 - x0) * (y1 - y0);
    }
};

class SubPlane : public hittable
{
public:
    SubPlane() = default;
    SubPlane(Rectangle const& _rect, shared_ptr<Material> mat)
        : rect(_rect)
        , mp(mat)
    {
    }

    virtual Real pdf_value(const point3& origin, const vec3& v) const override;
    virtual vec3 random(const point3& origin, std::mt19937& random_gen) const override;

private:
    virtual bool hit_priv(const ray& r, Real t_min, Real t_max, hit_record& rec) const override;
    shared_ptr<Material> mp;
    Rectangle rect;
};

