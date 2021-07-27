#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float k_pi    = 3.14159265358979323846f;
constexpr float k_pi_2  = 1.57079632679489661923f; // k_pi / 2
constexpr float k_1_pi  = 0.31830988618379067154f; // 1 / k_pi;
constexpr float k_tau   = 6.28318530717958647693f;  // 2.f * k_pi;
constexpr float k_1_tau = 0.15915494309189533577f; // 1.f / k_tau;

// using namespace glm;

constexpr float pow5(float x)
{
    float p2 = x * x;
    return p2 * p2 * x;
}

constexpr float sqr(float x)
{
    return x * x;
}

/*

inline float rsqrt(float x)
{
    return 1.f / sqrtf(x);
}

struct float2
{
    float x, y;
};

struct float3
{
    float x, y, z;
};

struct float4
{
    float x, y, z, w;
};

constexpr float2 operator+(float2 a, float2 b)
{
    return float2{ a.x + b.x, a.y + b.y };
}

constexpr float2 operator-(float2 a, float2 b)
{
    return float2{ a.x - b.x, a.y - b.y };
}

constexpr float2 operator-(float2 a, float b)
{
    return float2{ a.x - b, a.y - b };
}

constexpr float2 operator*(float2 a, float2 b)
{
    return float2{ a.x * b.x, a.y * b.y };
}

constexpr float2 operator*(float2 a, float b)
{
    return float2{ a.x * b, a.y * b };
}

constexpr bool operator!=(float2 a, float2 b)
{
    return a.x != b.x || a.y != b.y;
}

constexpr float3 operator+(float3 a, float b)
{
    return float3{ a.x + b, a.y + b, a.z + b };
}

constexpr float3 operator+(float a, float3 b)
{
    return float3{ a + b.x, a + b.y, a + b.z };
}

constexpr float3 operator-(float3 a, float b)
{
    return float3{ a.x - b, a.y - b, a.z - b };
}

constexpr float3 operator-(float a, float3 b)
{
    return float3{ a - b.x, a - b.y, a - b.z };
}

constexpr float3 operator+(float3 a, float3 b)
{
    return float3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

constexpr float3 operator-(float3 a, float3 b)
{
    return float3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

constexpr float3 operator*(float3 a, float b)
{
    return float3{a.x * b, a.y * b, a.z*b};
}

constexpr float3 operator*(float a, float3 b)
{
    return b * a;
}

constexpr float dot(float2 a, float2 b)
{
    return a.x * b.x + a.y * b.y;
}

constexpr float dot(float3 a, float3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr float3 cross(float3 u, float3 v)
{
   float a = u.y * v.z - u.z * v.y;
   float b = u.z * v.x - u.x * v.z;
   float c = u.x * v.y - u.y * v.x;
   return float3{ a, b, c };
}

constexpr float3 normalize(float3 v)
{
    float len2 = dot(v, v);
    return v * rsqrt(len2);
}
*/