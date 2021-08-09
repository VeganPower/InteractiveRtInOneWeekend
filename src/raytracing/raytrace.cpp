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

#include "../common.h"

#include "rtweekend.h"

#include "aarect.h"
#include "box.h"
#include "camera.h"

#include "Material.h"
#include "stopwatch.h"

#include "raytrace.h"

#include <random>
#include <vector>
#include <execution>

color ray_color(
    const ray& r,
    const color& background,
    const hittable& world,
    hittable const* lights,
    int depth,
    std::mt19937& random_gen
) {
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    hit_record rec;
    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    scatter_record srec;
    if (!rec.mat_ptr->scatter(r, rec, srec, random_gen))
        return emitted;
/*
    if (srec.is_specular) {
        return srec.attenuation * ray_color(srec.specular_ray, background, world, lights, depth-1, random_gen);
    }
*/
    // Select PDF
    auto light_pdf = hittable_pdf(lights, rec.p);
    //auto p = &light_pdf;//srec.pdf_ptr;
    //mixture_pdf p(&light_pdf, srec.pdf_ptr.get());

    //vec3 ray_dir = rec.normal + random_unit_vector(random_gen);
    vec3 ray_dir;// = p.generate(random_gen);
    double pdf_val = 1.f;
    {
        // Lambertian only
        const cosine_pdf Material_pdf = cosine_pdf(rec.normal);
        const vec3 Material_ray = Material_pdf.generate(random_gen);
        const double mat_pdf_val0 = Material_pdf.value(Material_ray);
        const double mat_pdf_val1 = light_pdf.value(Material_ray);
        const double weight_mat = mat_pdf_val0 / (mat_pdf_val0 + mat_pdf_val1);

        const vec3 light_ray       = unit_vector(light_pdf.generate(random_gen));
        const double light_pdf_val0 = light_pdf.value(light_ray);
        const double light_pdf_val1 = Material_pdf.value(light_ray);
        const double weight_light = light_pdf_val0 / (light_pdf_val0 + light_pdf_val1);

        const double total = weight_mat + weight_light;
        std::uniform_real_distribution<> random_dis(0.0, 1.0);
        const double w1_change = weight_light / total;
        if (random_dis(random_gen) < w1_change)
        {
            // Choose light pdf
            ray_dir = light_ray;
            pdf_val = total / light_pdf_val0;
        }
        else
        {
            // Choose Material pdf
            ray_dir = Material_ray;
            pdf_val = total / mat_pdf_val0;
        }
        //const double mat_pdf_val   = Material_pdf.value(ray_dir);
        //const double light_pdf_val = light_pdf.value(ray_dir);
        //pdf_val = 2.f / (mat_pdf_val + light_pdf_val);
    }
    ray scattered = ray(rec.p, ray_dir);
    //auto pdf_val = p.value(scattered.direction());
    vec3 result = emitted;
    vec3 attenuation = srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered);
    if (dot(attenuation, attenuation) > 0.0)
    {
        result += attenuation * ray_color(scattered, background, world, lights, depth-1, random_gen) * pdf_val;
    }
    return result;
}


struct IntRectangle
{
    int x_start, y_start;
    int x_end, y_end;
};

RtScene::RtScene()
{
    //auto lights = make_shared<hittable_list>();
    //lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<Material>()));
    //lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<Material>()));
    background = color(0.02f,0.02f,0.02f);
}


void check_nan(color& c)
{
    for (int i = 0; i < 3; ++i)
    {
        if (c[i] != c[i]) c[i] = 0.0;
    }
}

struct TileData
{
    IntRectangle rect;
    std::mt19937 random_gen;
};


void raytrace(Image& out_image, RtScene const& scene)
{
    int image_width = (int)out_image.width;
    int image_height = (int)out_image.height;

    std::random_device rd;
    // Render
    StopWatch sw("Render");
    int tile_size = 128;
    std::vector<TileData> tiles;
    for (int j = 0; j < image_height; j += tile_size)
    {
        for (int i = 0; i < image_width; i += tile_size)
        {
            IntRectangle rect { i, j, min_of(i + tile_size, image_width), min_of(j + tile_size, image_height) };
            tiles.push_back( { rect, std::mt19937(rd()) });
        }
    }
#ifdef NDEBUG
    std::for_each(std::execution::par, tiles.begin(), tiles.end(), [&](TileData& tile)
#else
    std::for_each(tiles.begin(), tiles.end(), [&](TileData& tile)
#endif
    {
        IntRectangle const& rect = tile.rect;
        std::uniform_real_distribution<> random_dis(0.0, 1.0);
        for (int j = rect.y_start; j < rect.y_end; ++j)
        {
            color* line = out_image.get_line(j);
            for (int i = rect.x_start; i < rect.x_end; ++i)
            {
                auto u = (i + random_dis(tile.random_gen)) / (image_width-1);
                auto v = (j + random_dis(tile.random_gen)) / (image_height-1);
                ray r = scene.cam.get_ray(u, v);
                color pixel_color = ray_color(r, scene.background, scene.world, scene.lights.get(), scene.max_depth, tile.random_gen);
                check_nan(pixel_color);
                line[i] += pixel_color;
            }
        }
    });
}
