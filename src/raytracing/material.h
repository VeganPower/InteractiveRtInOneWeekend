#ifndef Material_H
#define Material_H
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

#include "pdf.h"
#include "texture.h"


struct scatter_record
{
    //ray specular_ray;
    // bool is_specular;
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
};

struct Material
{
    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
    {
        return color(0, 0, 0);
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec, std::mt19937& random_gen) const
    {
        return false;
    }

    virtual double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const
    {
        return 0;
    }
};

class lambertian : public Material
{
public:
    lambertian(const color& a) : albedo(make_solid_color(a)) {}
    lambertian(texture const& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, scatter_record& srec, std::mt19937& random_gen
    ) const override
    {
        // srec.is_specular = false;
        srec.attenuation = albedo.value(rec.u, rec.v, rec.p);
        //srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
        return true;
    }

    double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const override
    {
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / pi;
    }

public:
    texture albedo;
};
/*
class metal : public Material
{
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, scatter_record& srec, std::mt19937& random_gen
    ) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        srec.specular_ray =
            ray(rec.p, reflected + fuzz * random_in_unit_sphere(random_gen), r_in.time());
        srec.attenuation = albedo;
        // srec.is_specular = true;
        //srec.pdf_ptr = nullptr;
        return true;
    }

public:
    color albedo;
    double fuzz;
};


class dielectric : public Material
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, scatter_record& srec, std::mt19937& random_gen
    ) const override
    {
        // srec.is_specular = true;
        //srec.pdf_ptr = nullptr;
        srec.attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        std::uniform_real_distribution<> uniform(0.0, 1.0);
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > uniform(random_gen))
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        srec.specular_ray = ray(rec.p, direction, r_in.time());
        return true;
    }

public:
    double ir; // Index of Refraction

private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
*/

class GGX : public Material
{
public:
    GGX(texture const& a, Real r, Real m)
        : albedo{a}
        , roughness{r}
        , metallicity{m}
    {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec, std::mt19937& r_gen) const override
    {
        srec.attenuation = albedo.value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<GGX_pdf>(rec.normal, r_in.direction(), roughness);
        return true;
    }

    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override
    {
        // pdf = D * NoH / 4 * VoH
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / pi;
    }

public:
    texture albedo;
    Real roughness;
    Real metallicity;
    // Real normal_map;
};


class diffuse_light : public Material
{
public:
    diffuse_light(texture const& a) : emit(a) {}
    diffuse_light(color c) : emit(make_solid_color(c)) {}

    virtual color emitted(
        const ray& r_in, const hit_record& rec, double u, double v, const point3& p
    ) const override
    {
        if (!rec.front_face)
            return color(0, 0, 0);
        return emit.value(u, v, p);
    }

public:
    texture emit;
};

/*
class isotropic : public Material {
    public:
        isotropic(color c) : albedo(make_solid_color(c)) {}
        isotropic(texture const& a) : albedo(a) {}

        #if 0
        // Issue #669
        // This method doesn't match the signature in the base `Material` class, so this one's
        // never actually called. Disabling this definition until we sort this out.

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
        #endif

    public:
        texture albedo;
};
*/

#endif
