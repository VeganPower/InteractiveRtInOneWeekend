#pragma once
#include "maths.h"

struct Mesh
{
    GLuint vao;
    GLuint vbo;

    GLuint prim_count;
    GLuint prim_type;
};


struct GridOptions
{
    enum class Plane
    {
        xy,
        xz,
        yz,
    };
    uint steps_x = 1, steps_y = 1;
    uint sub_steps = 10;
    Plane plane = Plane::xz;
    f32 space = 1.f;
    f32 height = 0.f;
    u32 axis_color = 0xFFFFFFFF;
    u32 line_color = 0xFF808080;
    u32 sub_line_color = 0xFF404040;
};

Mesh make_axis();
Mesh make_points();
Mesh make_grid(GridOptions const& options);
Mesh make_circle(float radius, uint segments, u32 color);
void draw(Mesh const& grid);