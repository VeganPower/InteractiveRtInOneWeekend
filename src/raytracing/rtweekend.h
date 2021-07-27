#ifndef RTWEEKEND_H
#define RTWEEKEND_H
//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

#include "../utils.h"

// Usings
using Real = double;
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const Real infinity = std::numeric_limits<Real>::infinity();
const Real pi = 3.1415926535897932385;

// Utility Functions

inline Real degrees_to_radians(Real degrees) {
    return degrees * ::pi / 180.0;
}

#if 0
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}
#endif
// Common Headers

#include "ray.h"
#include "vec3.h"


#endif
