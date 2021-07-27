
#pragma once
#include <glm/gtc/matrix_access.hpp>

class Camera
{
public:
   Camera();

   glm::vec3 get_view_direction() const;

   void view(glm::mat4 const& view_matrix);
   void pose(glm::mat4 const& camera_pose);
   glm::mat4 const& pose() const;

   void projection(glm::mat4 const& proj);

   glm::mat4 const& projection() const;
   glm::mat4 const& projection_view() const;
private:
   glm::mat4 proj_matrix;
   glm::mat4 proj_view_matrix;
   glm::mat4 pose_matrix;
};

inline glm::vec3 Camera::get_view_direction() const
{
   return glm::column(pose_matrix, 2);
}

inline glm::mat4 const& Camera::projection_view() const
{
   return proj_view_matrix;
}

inline glm::mat4 const& Camera::projection() const
{
   return proj_matrix;
}

inline glm::mat4 const& Camera::pose() const
{
   return pose_matrix;
}

glm::mat4 compute_projection_matrix(const float width, const float height, const float tan_h, const float near_plane, const float far_plane, const bool invert_depth, const bool infinite_far);
glm::mat4 compute_orthographic_matrix(const float width, const float height, const float near_plane, const float far_plane, const bool invert_depth, const bool infinite_far);

