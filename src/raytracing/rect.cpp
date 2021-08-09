#include "../common.h"
#include "aarect.h"

bool SubPlane::hit_priv(const ray& r, Real t_min, Real t_max, hit_record& rec) const
{
    Real num   = r.orig.z();
    Real denom = r.dir.z();
    if (denom == 0.f) return false;
    auto t = -num / denom;
    if (t < t_min || t > t_max)
        return false;
    point3 hit_point = r.at(t);
    if (rect.outside(hit_point.x(), hit_point.y()))
        return false;
    rec.u = (hit_point.x()-rect.x0)/(rect.x1-rect.x0);
    rec.v = (hit_point.y()-rect.y0)/(rect.y1-rect.y0);
    rec.t = t;
    rec.set_face_normal(r, vec3(0.f, 0.f, 1.f));
    rec.mat_ptr = mp.get();
    rec.p = hit_point;
    return true;
}

Real SubPlane::pdf_value(const point3& origin, const vec3& v) const
{
    hit_record rec;
    if (!this->hit(ray(origin, v), Real{0.001}, infinity, rec))
        return 0;
    auto area = rect.area();
    auto distance_squared = rec.t * rec.t * v.length_squared();
    auto cosine = fabs(dot(v, rec.normal) / v.length());

    return distance_squared / (cosine * area);
}

vec3 SubPlane::random(const point3& origin, std::mt19937& random_gen) const
{
    std::uniform_real_distribution<Real> uniform_x(rect.x0, rect.x1);
    std::uniform_real_distribution<Real> uniform_y(rect.y0, rect.y1);
    auto random_point = vec3(uniform_x(random_gen), uniform_y(random_gen), Real{0});
    vec3 p_w = random_point * rotation + translation;
    return p_w - origin;
}
