#ifndef PDF_H
#define PDF_H
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

#include "onb.h"


inline vec3 random_cosine_direction(std::mt19937& random_gen) {
    std::uniform_real_distribution<> uniform(0.0, 1.0);
    auto r1 = uniform(random_gen);
    auto r2 = uniform(random_gen);
    auto z = sqrt(1-r2);

    auto phi = 2*pi*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);

    return vec3(x, y, z);
}


inline vec3 random_to_sphere(double radius, double distance_squared, std::mt19937& random_gen) {
    std::uniform_real_distribution<> uniform(0.0, 1.0);
    const double r1 = uniform(random_gen);
    const double r2 = uniform(random_gen);
    const double z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

    const double phi = 2*pi*r1;
    const double x = cos(phi)*sqrt(1-z*z);
    const double y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}

inline vec3 random_GGX(double roughness, std::mt19937& random_gen)
{
    std::uniform_real_distribution<> uniform(0.0, 1.0);
    const double r0 = uniform(random_gen);
    const double r1 = uniform(random_gen);
    const double a = roughness * roughness;
    const double Phi = 2 * pi * r0;
    const double cos_theta = sqrt( (1 - r1) / ( 1 + (a * a - 1) * r1 ) );
    const double sin_theta = sqrt( 1 - cos_theta * cos_theta );
    vec3 H (sin_theta * cos(Phi),
            sin_theta * sin(Phi),
            cos_theta);
    return H;
}

// a = roughness^2
Real GGD_d(Real a, Real NoH)
{
    Real a2 = a * a;
    Real k = NoH * NoH * (a2- 1.0) + 1.0;
    return a2 / (pi * k * k);
}
/*
Real GGD_g1(Real )

Real GGD_g(Real a, Real NoV, Real NoL)
*/

struct pdf  {
    virtual ~pdf() {}

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate(std::mt19937& random_gen) const = 0;
};


struct cosine_pdf : public pdf {
    cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

    virtual double value(const vec3& direction) const override {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine/pi;
    }

    virtual vec3 generate(std::mt19937& random_gen) const override {
        return uvw.local(random_cosine_direction(random_gen));
    }
    onb uvw;
};


struct GGX_pdf : public pdf
{
    GGX_pdf(const vec3& N, const vec3& V, double r)
        : view(V)
        , normal(N)
       , roughness{r}
    {
        local_space.build_from_w(normal);
    }

    virtual double value(const vec3& direction) const override
    {
        // pdf = D * NoH / 4 * VoH
        vec3 H = unit_vector(normal + direction);
        Real NoH = dot(normal, H);
        Real VoH = dot(view, H);
        Real d = GGD_d(roughness * roughness, NoH);
        return d * NoH / (4.0 * VoH);
    }

    virtual vec3 generate(std::mt19937& r_gen) const override
    {
        vec3 temp = random_GGX(roughness, r_gen);
        vec3 H = local_space.local(temp);
        return reflect(view, H);
    }
    onb local_space;
    vec3 view;
    vec3 normal;
    double roughness;
};


struct hittable_pdf : public pdf {
    hittable_pdf(hittable const* p, const point3& origin) : ptr(p), o(origin) {}

    virtual double value(const vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }

    virtual vec3 generate(std::mt19937& random_gen) const override {
        return ptr->random(o, random_gen);
    }

    point3 o;
    hittable const* ptr;
};


struct mixture_pdf : public pdf {
    mixture_pdf(pdf const* p0, pdf const* p1)
        : p { p0, p1 }
    {
    }

    virtual double value(const vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 *p[1]->value(direction);
    }

    virtual vec3 generate(std::mt19937& random_gen) const override {
        std::uniform_real_distribution<> uniform(0.0, 1.0);
        if (uniform(random_gen) < 0.5)
            return p[0]->generate(random_gen);
        else
            return p[1]->generate(random_gen);
    }

    pdf const* p[2];
};

#endif
