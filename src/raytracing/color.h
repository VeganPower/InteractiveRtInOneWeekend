#ifndef COLOR_H
#define COLOR_H
//==============================================================================================
// Originally written in 2020 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "vec3.h"

inline double lin_to_sRGB(double x)
{
    double dark = x * 12.92;
    double light = pow((x * 1.055), 1.0 / 2.4) - 0.055;
    return x < 0.0031308 ? dark : light;
}

#endif
