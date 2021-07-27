#ifndef TEXTURE_H
#define TEXTURE_H
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

#include "perlin.h"

#include <iostream>


struct ColorGen {
    virtual color value(double u, double v, const vec3& p) const = 0;
};


struct texture
{
    color value(double u, double v, const vec3& p) const
    {
        color res = color_value;
        if (generator)
        {
            res = elem_product(res, generator->value(u, v, p));
        }
        return res;
    }

    shared_ptr<ColorGen> generator;
    color color_value;
};

struct constant : public ColorGen
{
    virtual color value(double u, double v, const vec3& p) const override {
        return c_value;
    };
    color c_value;
};

class checker_texture : public ColorGen
{
public:
    checker_texture() {}
    checker_texture(ColorGen const* c1, ColorGen const* c2)
        : even(c1) , odd(c2) {}

    virtual color value(double u, double v, const vec3& p) const override {
        auto sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    ColorGen const* odd;
    ColorGen const* even;
};


class noise_texture : public ColorGen {
    public:
        noise_texture(std::mt19937& random_gen)
            : noise(random_gen)
        {}
        noise_texture(std::mt19937& random_gen, double sc)
            : noise(random_gen)
            , scale(sc)
        {}

        virtual color value(double u, double v, const vec3& p) const override {
            // return color(1,1,1)*0.5*(1 + noise.turb(scale * p));
            // return color(1,1,1)*noise.turb(scale * p);
            return color(1,1,1)*0.5*(1 + sin(scale*p.z() + 10*noise.turb(p)));
        }

    public:
        perlin noise;
        double scale;
};


inline texture make_solid_color(vec3 const& c0)
{
    return { nullptr, c0 };
}


#endif
