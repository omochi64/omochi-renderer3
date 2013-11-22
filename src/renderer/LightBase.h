#pragma once

#include "tools/Vector.h"

namespace OmochiRenderer {

  class Random;

  class LightBase {
  public:
    virtual ~LightBase() {}

    virtual void SampleOnePoint(Vector3 &point, double &pdf, const Random &rnd) const = 0;
    virtual double TotalPower() const = 0;
  };
}
