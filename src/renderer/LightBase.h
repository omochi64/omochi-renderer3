#pragma once

#include "tools/Vector.h"

namespace OmochiRenderer {

  class Random;

  class LightBase {
  public:
    virtual ~LightBase() {}

    virtual void SampleOnePoint(Vector3 &point, Vector3 &normal, double &pdf, const Random &rnd) const = 0;
    // targetPoint から可視である可能性が高い位置でサンプリングする
    virtual bool SampleOnePointWithTargetPoint(Vector3 &sampledPoint, Vector3 &sampledPointNormal, double &pdf, const Vector3 &targetPoint, const Random &rnd) const = 0;
    virtual double TotalPower() const = 0;
  };
}
