#pragma once
#include "vec3.h"
#include "../angle.h"

struct Mat3
{
    vec3 cols[3];

    static Mat3 identity()
    {
        Mat3 res;
        res.cols[0] = vec3(1.f, 0.f, 0.f);
        res.cols[1] = vec3(0.f, 1.f, 0.f);
        res.cols[2] = vec3(0.f, 0.f, 1.f);
        return res;
    }
    static Mat3 rotation_x(Angle a)
    {
        Real ca = cosf(a.radian());
        Real sa = sinf(a.radian());
        Mat3 res;
        res.cols[0] = vec3(1.f, 0.f, 0.f);
        res.cols[1] = vec3(0.f,  ca,  sa);
        res.cols[2] = vec3(0.f, -sa,  ca);
        return res;
    }
    static Mat3 rotation_y(Angle a)
    {
        Real ca = cosf(a.radian());
        Real sa = sinf(a.radian());
        Mat3 res;
        res.cols[0] = vec3( ca, 0.f, -sa);
        res.cols[1] = vec3(0.f, 1.f, 0.f);
        res.cols[2] = vec3( sa, 0.f,  ca);
        return res;
    }
};

inline vec3 operator*(vec3 const& a, Mat3 const& b)
{
    vec3 res {
        dot(a, b.cols[0]),
        dot(a, b.cols[1]),
        dot(a, b.cols[2]),
    };
    return res;
}

inline Mat3 operator*(Mat3 const& a, Mat3 const& b)
{
    Mat3 res;
    for (int r = 0; r < 3; ++r)
    {
        vec3 row { a.cols[0][r], a.cols[1][r], a.cols[2][r] };
        for (int c = 0; c < 3; ++c)
            res.cols[c][r] = dot(row, b.cols[c]);
    }
    return res;
}

inline Mat3 transpose(Mat3 const& a)
{
    Mat3 res;
    res.cols[0] = vec3{a.cols[0].x(), a.cols[1].x(), a.cols[2].x() };
    res.cols[1] = vec3{a.cols[0].y(), a.cols[1].y(), a.cols[2].y() };
    res.cols[2] = vec3{a.cols[0].z(), a.cols[1].z(), a.cols[2].z() };
    return res;
}