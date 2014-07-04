#pragma once

#include "tools/Vector.h"
#include "tools/Random.h"
#include "tools/Constant.h"

namespace OmochiRenderer {

  // 絞りのインターフェース
  class Aperture {
  public:
    virtual void SampleOnePoint(double &sampledX, double &sampledY, const Random &rnd) const = 0;
  };

  // 円形の絞り
  class CircleAperture : public Aperture {
  public:
    explicit CircleAperture(double radius = 0.0)
      : m_radius(radius)
    {
    }

    virtual void SampleOnePoint(double &sampledX, double &sampledY, const Random &rnd) const {
      // 1点をサンプリング
      // F(r, θ) = θ/2PI * r^2/R^2

      // F(θ) = θ/2PI
      double theta = 2 * PI * rnd.nextDouble();

      // F(r) = r^2/R^2
      double r = m_radius * sqrt(rnd.nextDouble());

      sampledX = r * cos(theta);
      sampledY = r * sin(theta);
    }

  private:
    double m_radius;
  };

}
