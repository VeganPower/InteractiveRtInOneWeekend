#pragma once
#include <imgui.h>
#include "angle.h"
struct Dimension;
class Camera;

struct ArcBall
{
    float distance = 5.f;
    Angle phi = { Angle::Degree(0.f) } ;
    Angle theta = { Angle::Degree(45.f) };

    Angle start_phi { 0.f };
    Angle start_theta { 0.f };

    Angle v_limit { Angle::Degree(85.f) };
    glm::vec3 focus { 0.f };

    bool update(ImGuiIO const& io, Dimension const& size);
};

void set(Camera& c, ArcBall const& a);
