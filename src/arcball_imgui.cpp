#include "common.h"
#include "arcball_imgui.h"

#include "camera.h"
#include "dimension.h"

glm::vec2 pos_to_angle(glm::vec2 pos)
{
    const float d = dot(pos, pos);
    if (d >= 1.f)
    {
        glm::vec2 t = pos / sqrt(d);
        float a = asinf(t.y);
        float b = k_pi_2;
        if (pos.x < 0.f) b = -k_pi_2;
        return glm::vec2(a, b);
    }
    float a = asinf(pos.y);
    float t = sqrt(1.f - pos.y * pos.y);
    float b = asinf(pos.x / t); //
    return glm::vec2(a, b);
}

bool ArcBall::update(ImGuiIO const& io, Dimension const& size)
{
    distance = max_of(0.5f, distance + io.MouseWheel * 0.1f);
    if (!io.MouseDown[0])
    {
        start_phi = phi;
        start_theta = theta;
        return false;
    }
    if (!ImGui::IsMouseDragging(0)) return false;

    const glm::vec2 c = glm::vec2(size.width, size.height) * 0.5f;
    const float radius = min_of(c.x, c.y) * 0.9f;
    const float r2 = sqr(radius);
    const glm::vec2 m = glm::vec2(io.MousePos.x, io.MousePos.y);
    const glm::vec2 v = (m - c) / radius;
    const glm::vec2 s = (glm::vec2(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y) - c) / radius;// * (1.f / radius);

    glm::vec2 start   =  pos_to_angle(s);
    glm::vec2 current =  pos_to_angle(v);
    glm::vec2 delta = start - current;

    phi   = start_phi + Angle::Radian(delta.y);//Angle::Radian(fmod(start_phi.radian() + delta.y, k_pi));
    theta = ::clamp<Angle>(start_theta - Angle::Radian(delta.x), -v_limit, v_limit);
    return true;
}

void set(Camera& c, ArcBall const& a)
{
    float sp = sinf(a.phi.radian());
    float cp = cosf(a.phi.radian());
    float st = sinf(a.theta.radian());
    float ct = cosf(a.theta.radian());

    float x = sp * ct * a.distance;
    float y = st * a.distance;
    float z = cp * ct * a.distance;
    const glm::vec3 eye(x, y, z);
    const glm::vec3 k_up(0.f, 1.f, 0.f);

    glm::vec3 V_z = glm::normalize(a.focus - eye);
    glm::vec3 V_x = glm::normalize(cross(k_up, V_z));
    glm::vec3 V_y = glm::cross(V_z, V_x);
    glm::mat4 look_pose = glm::mat4( glm::vec4(-V_x, 0.f),
       glm::vec4(-V_y, 0.f),
       glm::vec4(V_z, 0.f),
       glm::vec4(eye, 1.f));
    c.pose(look_pose);
}

