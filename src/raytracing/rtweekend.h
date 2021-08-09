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
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const Real infinity = std::numeric_limits<Real>::infinity();
const Real pi = Real{3.1415926535897932385};

// Utility Functions

inline Real degrees_to_radians(Real degrees) {
    return degrees * ::pi / Real(180.0);
}

// Common Headers

#include "ray.h"
#include "vec3.h"


#endif
