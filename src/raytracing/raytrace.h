#pragma once
#include "aarect.h"
#include "hittable_list.h"
#include "hittable.h"
#include "image.h"
#include "camera.h"

struct RtScene
{

    RtScene();
    hittable_list world;
    std::shared_ptr<xz_rect> lights;
    color background;
    RtCamera cam;

    int samples_per_pixel;
    int max_depth = 8;
};

void raytrace(Image& out_image, RtScene const& scene);