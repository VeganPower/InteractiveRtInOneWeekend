#include "common.h"
#include "vertex.h"
#include "shapes.h"
#include <cmath>

using namespace glm;

Mesh make_mesh(ColorVertex const* vx, uint vx_count, GLenum type)
{
    Mesh res;
    glGenVertexArrays(1, &res.vao);
    glBindVertexArray(res.vao);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(float) *3);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 0);

    glGenBuffers(1, &res.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorVertex) * vx_count, vx, GL_STATIC_DRAW);

    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), 0);
    //glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ColorVertex), (void*)(sizeof(float) *2));
    res.prim_count = vx_count;
    res.prim_type  = type;

    glBindVertexArray(0);
    return res;
}

Mesh make_axis()
{
    ColorVertex vertices[6] =
    {
        { 0.f, 0.f, 0.f, 0x0000FF },
        { 1.f, 0.f, 0.f, 0x0000FF },
        { 0.f, 0.f, 0.f, 0x00FF00 },
        { 0.f, 1.f, 0.f, 0x00FF00 },
        { 0.f, 0.f, 0.f, 0xFF0000 },
        { 0.f, 0.f, 1.f, 0xFF0000 },
    };

    return make_mesh(vertices, 6, GL_LINES);
}

Mesh make_grid(GridOptions const& options)
{
    const f32 space = options.space;
    const uint sub_steps = max_of<uint>(options.sub_steps, 1);
    const uint lines_count = options.steps_x * sub_steps + options.steps_y * sub_steps + 2;
    const uint vx_count = lines_count * 2;
    ColorVertex* line_vertices = (ColorVertex*)malloc(sizeof(ColorVertex) * vx_count);
    const float sub_step_size = space / sub_steps;

    const float z = options.height;
    const float h_size_x = space * options.steps_x * 0.5f;
    const float h_size_y = space * options.steps_y * 0.5f;
    int line_idx = 0;
    for (uint i = 0; i < options.steps_x; ++i)
    {
        float x = i * space - h_size_x;
        u32 col = options.line_color;
        if (i == options.steps_x / 2)
        {
           col = options.axis_color;
        }
        line_vertices[line_idx * 2 + 0] = { x, -h_size_y, z, col };
        line_vertices[line_idx * 2 + 1] = { x,  h_size_y, z, col };
        line_idx++;
        for (uint s = 1; s < sub_steps; ++s)
        {
            float xs = x + s * sub_step_size;
            line_vertices[line_idx * 2 + 0] = { xs, -h_size_y, z, options.sub_line_color };
            line_vertices[line_idx * 2 + 1] = { xs,  h_size_y, z, options.sub_line_color };
            line_idx++;
        }
    }
    line_vertices[line_idx * 2 + 0] = { h_size_x, -h_size_y, z, options.line_color };
    line_vertices[line_idx * 2 + 1] = { h_size_x,  h_size_y, z, options.line_color };
    line_idx++;
    for (uint i = 0; i < options.steps_y; ++i)
    {
        float y = i * space - h_size_y;
        u32 col = options.line_color;
        if (i == options.steps_x / 2)
        {
           col = options.axis_color;
        }
        line_vertices[line_idx * 2 + 0] = { -h_size_x, y, z, col };
        line_vertices[line_idx * 2 + 1] = {  h_size_x, y, z, col };
        line_idx++;
        for (uint s = 1; s < sub_steps; ++s)
        {
            float ys = y + s * sub_step_size;
            line_vertices[line_idx * 2 + 0] = { -h_size_x, ys, z, options.sub_line_color };
            line_vertices[line_idx * 2 + 1] = {  h_size_x, ys, z, options.sub_line_color };
            line_idx++;
        }
    }
    line_vertices[line_idx * 2 + 0] = { -h_size_x, h_size_y, z, options.line_color };
    line_vertices[line_idx * 2 + 1] = {  h_size_x, h_size_y, z, options.line_color };
    line_idx++;
    switch(options.plane)
    {
    case GridOptions::Plane::xy: break;
    case GridOptions::Plane::xz:
        for (uint i = 0; i < vx_count; ++i)
        {
            line_vertices[i].z = line_vertices[i].y;
            line_vertices[i].y = 0.f;
        }
        break;
    case GridOptions::Plane::yz: break;
        for (uint i = 0; i < vx_count; ++i)
        {
            line_vertices[i].z = line_vertices[i].x;
            line_vertices[i].x = 0.f;
        }
        break;
    }
    Mesh res = make_mesh(line_vertices, vx_count, GL_LINES);
    free(line_vertices);
    return res;
}

Mesh make_circle(float radius, uint segments, u32 color)
{
    const uint vx_count = segments;
    ColorVertex* line_vertices = (ColorVertex*)malloc(sizeof(ColorVertex) * vx_count);

    float step_alpha = k_tau / (float)segments;
    for (uint i = 0; i < segments; ++i)
    {
        float x = radius * cosf(step_alpha * i);
        float y = radius * sinf(step_alpha * i);
        line_vertices[i] = { x, y, 0.f, color };
    }

    Mesh res = make_mesh(line_vertices, vx_count, GL_LINE_LOOP);
    free(line_vertices);
    return res;
}

constexpr int one_zero(bool x)
{
    return x ? 1 : 0;
}

void draw(Mesh const& grid)
{
    glBindVertexArray(grid.vao);
    glBindVertexBuffer(0, grid.vbo, 0, sizeof(ColorVertex));

    glDrawArrays(grid.prim_type, 0, grid.prim_count);
}

vec2 OctWrap( vec2 v )
{
    float x = (1 - abs(v.y)) * (v.x >= 0.f ? 1.f : -1.f);
    float y = (1 - abs(v.x)) * (v.y >= 0.f ? 1.f : -1.f);
    return vec2( x, y);
}

vec2 Encode( vec3 n )
{
    n /= ( abs( n.x ) + abs( n.y ) + abs( n.z ) );
    vec2 n2 (n.x, n.y);
    n2 = n.z >= 0.f ? n2 : OctWrap( n2 );
    n2 = n2 * 0.5f + 0.5f;
    return n2;
}

/*
float3 Decode( float2 f )
{
    f = f * 2.0 - 1.0;

    // https://twitter.com/Stubbesaurus/status/937994790553227264
    float3 n = float3( f.x, f.y, 1.0 - abs( f.x ) - abs( f.y ) );
    float t = saturate( -n.z );
    n.xy += n.xy >= 0.0 ? -t : t;
    return normalize( n );
}
*/

vec3 Decode( vec2 f )
{
    f = f * 2.f - 1.f;
    vec3 n = vec3( f.x, f.y, 1.0 - abs( f.x ) - abs( f.y ) );
    float t = saturate( -n.z );

    n.x += (n.x >= 0.f) ? -t : t;
    n.y += (n.y >= 0.f) ? -t : t;
    return normalize(n);
}

Mesh make_points()
{
    const uint segments = 128;
    const uint slices = 128;
    const uint vx_count = segments * slices * 1;
    ColorVertex* line_vertices = (ColorVertex*)malloc(sizeof(ColorVertex) * vx_count);

    uint vx = 0;
#if 0
    float step_alpha = k_tau / (float)slices;
    float step_beta  = k_pi / (float)segments;
    for (uint i = 0; i < segments; ++i)
    {
        float phi = -k_pi_2 + step_beta * (i+1);
        float y = sinf(phi);
        float k = cosf(phi);
        for (uint j = 0; j < slices; ++j)
        {
            float x = cosf(step_alpha * j) * k;
            float z = sinf(step_alpha * j) * k;
            //line_vertices[vx++] = { x, y, z, 0xFFFFFF };

            vec2 p = Encode(vec3(x, y, z));

            if (p.x > 1.f)
            {
             //  p.x -= 1.f;
            }
            if (p.y > 1.f)
            {
            //   p.y -= 1.f;
            }
            uint8_t r = x > 0.f ? 255 : 128;
            uint8_t g = y > 0.f ? 255 : 128;
            uint8_t b = z > 0.f ? 255 : 128;
            uint32_t C = r | (g << 8) | (b << 16);

            line_vertices[vx++] = { p.x, 0.f, p.y, C };
        }
    }
#else
    float step_i = 1.f / (float)slices;
    float step_j = 1.f / (float)segments;
    for (uint i = 0; i < segments; ++i)
    {
        float ii = (i + 0.5f) * step_i;
        for (uint j = 0; j < slices; ++j)
        {
            float jj = (j + 0.5f) * step_j;
            vec3 p = Decode(vec2(ii, jj));
            //uint8_t r = x > 0.f ? 255 : 128;
            //uint8_t g = y > 0.f ? 255 : 128;
            //uint8_t b = z > 0.f ? 255 : 128;
            uint32_t C = 0xFFFFFF;//r | (g << 8) | (b << 16);

            line_vertices[vx++] = { p.x, p.y, p.z, C };
        }
    }
#endif
    Mesh res = make_mesh(line_vertices, vx_count, GL_POINTS);
    free(line_vertices);
    return res;
}
