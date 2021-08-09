#include "../common.h"
#include "box.h"

box::box(const vec3& _s, shared_ptr<Material> ptr)
    : size { _s }
{
    vec3 half_size = size * Real{0.5};

    sides[1] = SubPlane(Rectangle{ -half_size.z(), half_size.z(), -half_size.y(), half_size.y() }, ptr);
    sides[0] = SubPlane(Rectangle{ -half_size.z(), half_size.z(), -half_size.y(), half_size.y() }, ptr);
    sides[2] = SubPlane(Rectangle{ -half_size.x(), half_size.x(), -half_size.y(), half_size.y() }, ptr);
    sides[3] = SubPlane(Rectangle{ -half_size.x(), half_size.x(), -half_size.y(), half_size.y() }, ptr);
    sides[4] = SubPlane(Rectangle{ -half_size.x(), half_size.x(), -half_size.z(), half_size.z() }, ptr);
    sides[5] = SubPlane(Rectangle{ -half_size.x(), half_size.x(), -half_size.z(), half_size.z() }, ptr);

    sides[0].transform(point3( 0.f, 0.f, half_size.z()), Mat3::rotation_y(Angle::Degree(  0.f)));
    sides[1].transform(point3( 0.f, 0.f,-half_size.z()), Mat3::rotation_y(Angle::Degree(180.f)));
    sides[2].transform(point3( half_size.x(), 0.f, 0.f), Mat3::rotation_y(Angle::Degree( 90.f)));
    sides[3].transform(point3(-half_size.x(), 0.f, 0.f), Mat3::rotation_y(Angle::Degree(270.f)));
    sides[4].transform(point3( 0.f, half_size.y(), 0.f), Mat3::rotation_x(Angle::Degree( 90.f)));
    sides[5].transform(point3( 0.f,-half_size.y(), 0.f), Mat3::rotation_x(Angle::Degree(-90.f)));
}

bool box::hit_priv(const ray& r, Real t_min, Real t_max, hit_record& rec) const
{
    hit_record temp_rec;
    auto hit_anything = false;
    auto closest_so_far = t_max;
    for (const auto& s : sides)
    {
        if (s.hit(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}
