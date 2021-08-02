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
#include "box.h"
#include "camera.h"

#include "material.h"
#include "sphere.h"
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
        const cosine_pdf material_pdf = cosine_pdf(rec.normal);
        const vec3 material_ray = material_pdf.generate(random_gen);
        const double mat_pdf_val0 = material_pdf.value(material_ray);
        const double mat_pdf_val1 = light_pdf.value(material_ray);
        const double weight_mat = mat_pdf_val0 / (mat_pdf_val0 + mat_pdf_val1);

        const vec3 light_ray       = unit_vector(light_pdf.generate(random_gen));
        const double light_pdf_val0 = light_pdf.value(light_ray);
        const double light_pdf_val1 = material_pdf.value(light_ray);
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
            // Choose material pdf
            ray_dir = material_ray;
            pdf_val = total / mat_pdf_val0;
        }
        //const double mat_pdf_val   = material_pdf.value(ray_dir);
        //const double light_pdf_val = light_pdf.value(ray_dir);
        //pdf_val = 2.f / (mat_pdf_val + light_pdf_val);
    }
    ray scattered = ray(rec.p, ray_dir, r.time());
    //auto pdf_val = p.value(scattered.direction());
    vec3 result = emitted;
    vec3 attenuation = srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered);
    if (dot(attenuation, attenuation) > 0.0)
    {
        result += attenuation * ray_color(scattered, background, world, lights, depth-1, random_gen) * pdf_val;
    }
    return result;
}


hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    // shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.2);
    shared_ptr<material> aluminum = make_shared<lambertian>(color(0.8, 0.85, 0.88));
    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    auto glass = make_shared<lambertian>(color(0.8, 0.8, 0.8));
    // auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190,90,190), 90 , glass));

    return objects;
}

struct Rectangle
{
    int x_start, y_start;
    int x_end, y_end;
};

RtScene::RtScene()
{

    // World

    //auto lights = make_shared<hittable_list>();
    //lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    //lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));
    lights = make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>());

    world = cornell_box();
    background = color(0,0,0);
}

thread_local std::random_device rd;
thread_local std::mt19937 random_gen(rd());

void check_nan(color& c)
{
    for (int i = 0; i < 3; ++i)
    {
        if (c[i] != c[i]) c[i] = 0.0;
    }
}

void raytrace(Image& out_image, RtScene const& scene)
{
    int image_width = (int)out_image.width;
    int image_height = (int)out_image.height;

    float aspect_ratio =  (float)image_width / (float)image_height;
    // Camera
    point3 lookfrom(278, 278, -800);
    point3 lookat(278, 278, 0);
    vec3 vup(0, 1, 0);

    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    auto time0 = 0.0;
    auto time1 = 1.0;

    camera cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time0, time1);

    // Render
    StopWatch sw("Render");
    int tile_size = 128;
    std::vector<Rectangle> tiles;
    for (int j = 0; j < image_height; j += tile_size)
    {
        for (int i = 0; i < image_width; i += tile_size)
        {
            Rectangle rect { i, j, min_of(i + tile_size, image_width), min_of(j + tile_size, image_height) };
            tiles.push_back(rect);
        }
    }
#ifdef NDEBUG
    std::for_each(std::execution::par, tiles.begin(), tiles.end(), [&](Rectangle const& rect)
#else
    std::for_each(tiles.begin(), tiles.end(), [&](Rectangle const& rect)
#endif
    {
        std::uniform_real_distribution<> random_dis(0.0, 1.0);
        for (int j = rect.y_start; j < rect.y_end; ++j)
        {
            //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            color* line = out_image.get_line(j);
            for (int i = rect.x_start; i < rect.x_end; ++i)
            {
                // color pixel_color(0,0,0);
                // for (int s = 0; s < scene.samples_per_pixel; ++s)
                // {
                    auto u = (i + random_dis(random_gen)) / (image_width-1);
                    auto v = (j + random_dis(random_gen)) / (image_height-1);
                    ray r = cam.get_ray(u, 1.f - v, random_gen);
                    color pixel_color = ray_color(r, scene.background, scene.world, scene.lights.get(), scene.max_depth, random_gen);
                // }
                    check_nan(pixel_color);

                line[i] += pixel_color;
            }
        }
    });
}
