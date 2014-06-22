#pragma once

#include "tools/Vector.h"

namespace OmochiRenderer {

class Ray {
public:
  Ray(const Vector3 &begin_, const Vector3 &dir_)
    : orig(begin_)
    , dir(dir_)
  {
  }

  Vector3 operator()(double t) const {
    return orig + dir*t;
  }

  Vector3 orig;
  Vector3 dir;
};

}
