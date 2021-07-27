#pragma once
#include "aarect.h"
#include "color.h"
#include "hittable_list.h"
#include "hittable.h"
#include "image.h"

struct RtScene
{

    RtScene();
    hittable_list world;
    std::shared_ptr<xz_rect> lights;
    color background;

    int samples_per_pixel;
    int max_depth = 8;
};

void raytrace(Image& out_image, RtScene const& scene);