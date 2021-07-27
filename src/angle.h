#pragma once

struct Angle
{
   static Angle Degree(float x) { return {x * k_pi / 180.f }; }
    static Angle Radian(float x) { return {x}; }

    float degree() const { return rep * 180.f * k_1_pi; }
    float radian() const { return rep; }

    float rep = 0.f;
};

inline Angle operator+(Angle const& a, Angle const& b)
{
    return Angle::Radian( a.rep + b.rep );
}

inline Angle operator-(Angle const& a, Angle const& b)
{
    return Angle::Radian( a.rep - b.rep );
}

inline Angle operator-(Angle const& a)
{
    return Angle::Radian( -a.rep );
}

inline bool operator>(Angle const& a, Angle const& b)
{
    return a.rep > b.rep;
}

inline bool operator<(Angle const& a, Angle const& b)
{
    return a.rep < b.rep;
}

inline bool operator==(Angle const& a, Angle const& b)
{
    return a.rep == b.rep;
}