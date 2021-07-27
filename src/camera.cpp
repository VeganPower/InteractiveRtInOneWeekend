#include "common.h"
#include "camera.h"
#include <glm/gtc/matrix_inverse.hpp>

using namespace glm;

Camera::Camera()
   : proj_matrix(1.f)
   , proj_view_matrix(1.f)
   , pose_matrix(1.f)
{
}

void Camera::projection(mat4 const& proj)
{
   proj_matrix = proj;
   const mat4 camera_view = affineInverse(pose_matrix);
   proj_view_matrix = proj_matrix * camera_view;
}

void Camera::pose(mat4 const& camera_matrix)
{
   pose_matrix = camera_matrix;
   const mat4 camera_view = affineInverse(pose_matrix);
   proj_view_matrix = proj_matrix * camera_view;
}

void Camera::view(mat4 const& view_matrix)
{
   pose_matrix = affineInverse(view_matrix);
   proj_view_matrix = proj_matrix * view_matrix;
}

struct DepthParams
{
    float A, B;
};

DepthParams compute_depth_params(const float near_plane, const float far_plane, const bool invert_depth, const bool infinite_far)
{
    float A = 0;
    if (infinite_far)
        A = 1.f;
    else
        A = far_plane / (far_plane - near_plane);
    float B = -A * near_plane;
    if (invert_depth)
    {
        A = A - 1.f;
        B = -B;
    }
    return DepthParams{ A, B};
}

mat4 compute_projection_matrix(const float width, const float height, const float tan_h, const float near_plane, const float far_plane, const bool invert_depth, const bool infinite_far)
{
    DepthParams depth = compute_depth_params(near_plane, far_plane, invert_depth, infinite_far);
    const float tan_top      =  tan_h;
    const float tan_bottom   = -tan_h;

    const float aspect_ratio = width / height;
    const float tan_right    = aspect_ratio * tan_top;
    const float tan_left     = aspect_ratio * tan_bottom;

    const float left_c   = tan_left;
    const float right_c  = tan_right;
    const float bottom_c = tan_bottom;
    const float top_c    = tan_top;

    const float C = 2.f / (right_c - left_c);
    const float D = 2.f / (top_c - bottom_c);

    return mat4(  C, 0.f, 0.f, 0.f,
                0.f,   D, 0.f, 0.f,
                0.f, 0.f, depth.A, 1.f,
                0.f, 0.f, depth.B, 0.f);
}

mat4 compute_orthographic_matrix(const float width, const float height, const float near_plane, const float far_plane, const bool invert_depth, const bool infinite_far)
{
    DepthParams depth = compute_depth_params(near_plane, far_plane, invert_depth, infinite_far);
    const float C = height / width;
    const float D = 1.f;

    return mat4(  C, 0.f, 0.f, 0.f,
                0.f,   D, 0.f, 0.f,
                0.f, 0.f, depth.A, 0.f,
                0.f, 0.f, depth.B, 1.f);
}