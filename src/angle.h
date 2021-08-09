#pragma once

struct Angle
{
    static Angle Degree(Real x) { return {x * k_pi / Real(180) }; }
    static Angle Radian(Real x) { return {x}; }

    Real degree() const { return rep * Real(180) * k_1_pi; }
    Real radian() const { return rep; }

    Real rep;
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