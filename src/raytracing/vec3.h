#ifndef VEC3_H
#define VEC3_H
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
#include <cmath>
#include <random>

using std::sqrt;
using std::fabs;

struct vec3
{
    vec3() = default;
    vec3(Real e0, Real e1, Real e2) : e{e0, e1, e2} {}

    Real x() const { return e[0]; }
    Real y() const { return e[1]; }
    Real z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    Real operator[](int i) const { return e[i]; }
    Real& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(const Real t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const Real t) {
        return *this *= 1/t;
    }

    Real length() const {
        return sqrt(length_squared());
    }

    Real length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    Real smallest_element() const {
        return min_of(min_of(e[0], e[1]), e[2]);
    }

    Real biggest_element() const {
        return min_of(min_of(e[0], e[1]), e[2]);
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    static vec3 zero() { return vec3 { 0.f, 0.f, 0.f }; }
    static vec3 x_axis() { return vec3 { 1.f, 0.f, 0.f }; }
    static vec3 y_axis() { return vec3 { 0.f, 1.f, 0.f }; }
    static vec3 z_axis() { return vec3 { 0.f, 0.f, 1.f }; }

    static vec3 random(std::mt19937& random_gen) {
        std::uniform_real_distribution<Real> uniform(0.0, 1.0);
        return vec3(uniform(random_gen), uniform(random_gen), uniform(random_gen));
    }

    static vec3 random(std::mt19937& random_gen, Real min, Real max) {
        std::uniform_real_distribution<Real> uniform(min, max);
        return vec3(uniform(random_gen), uniform(random_gen), uniform(random_gen));
    }

    Real e[3];
};


// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color


// vec3 Utility Functions

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(Real t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, Real t) {
    return t * v;
}

inline vec3 operator/(vec3 v, Real t) {
    return (1/t) * v;
}

inline vec3 elem_product(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0],
                u.e[1] * v.e[1],
                u.e[2] * v.e[2]);
}

inline vec3 elem_min(const vec3 &u, const vec3 &v)
{
    vec3 res;
    for (int i = 0; i < 3; ++i) res.e[i] = min_of(u.e[i], v.e[i]);
    return res;
}

inline vec3 elem_max(const vec3 &u, const vec3 &v)
{
    vec3 res;
    for (int i = 0; i < 3; ++i) res.e[i] = max_of(u.e[i], v.e[i]);
    return res;
}

inline Real dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}

inline vec3 random_in_unit_disk(std::mt19937& random_gen)
{
    std::uniform_real_distribution<Real> uniform(-1, 1);
    while (true) {
        auto p = vec3(uniform(random_gen), uniform(random_gen), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_in_unit_sphere(std::mt19937& random_gen)
{
    while (true) {
        auto p = vec3::random(random_gen, -1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_unit_vector(std::mt19937& random_gen)
{
    return unit_vector(random_in_unit_sphere(random_gen));
}

inline vec3 random_in_hemisphere(const vec3& normal, std::mt19937& random_gen)
{
    vec3 in_unit_sphere = random_in_unit_sphere(random_gen);
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, Real etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), Real(1.0));
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(Real(1.0) - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}


#endif
