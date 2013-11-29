#pragma once

#include "LightBase.h"
#include "Sphere.h"
#include "tools/Random.h"

namespace OmochiRenderer {
  class SphereLight : public Sphere, public LightBase {
  public:
    SphereLight(double r, const Vector3 &pos, const Material &material_)
      : Sphere(r, pos, material_)
      , LightBase()
    {
    }
    virtual ~SphereLight() {}

    virtual void SampleOnePoint(Vector3 &point, Vector3 &normal, double &pdf, const Random &rnd) const {
      // 全球から uniform にサンプリング
      // pdf = 1.0 / 半径rの球の表面積
      pdf = 1.0 / (4 * PI *m_radius * m_radius);
       
      // F(θ, φ) = φ/2π*(1-cosθ)/2
      double phi = 2 * PI*rnd.nextDouble();
      double cos_shita = 1 - 2 * rnd.nextDouble();
      double sin_shita = sqrt(1-cos_shita*cos_shita);

      Vector3 dir(sin_shita*cos(phi), cos_shita, sin_shita*sin(phi));

      point = this->position + dir * (m_radius + EPS);
      normal = dir;
    }
    virtual double TotalPower() const {
      return 4 * PI * m_radius * m_radius * material.emission.length();
    }
  };
}
