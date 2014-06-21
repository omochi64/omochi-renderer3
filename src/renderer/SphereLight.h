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
      
      // 
      
      
      
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

    // targetPoint から可視である可能性が高い位置でサンプリングする
    virtual bool SampleOnePointWithTargetPoint(Vector3 &sampledPoint, Vector3 &sampledPointNormal, double &pdf, const Vector3 &targetPoint, const Random &rnd) const {

      Vector3 diff = targetPoint - this->position;

      // 可視でありうるθの範囲 (target, 球の中心, targetを通る球の接線 を引いて、cosθの値を計算するとこうなる)
      double diff_size = diff.length();
      double max_cos_shita = m_radius / diff_size;

      if (max_cos_shita > 1) return false; // 球の内部に入り込んでいる

      Vector3 &normal = diff;
      normal /= diff_size;

      Vector3 axis1, axis2;
      Utils::GetCrossAxes(normal, axis1, axis2);

      // max_cos_shita より上から uniform にサンプリング
      pdf = 1.0 / (2 * PI * m_radius * m_radius * ( 1 - max_cos_shita ) );

      // F(θ, φ) = φ/2π*(1-cosθ)/(1-max_cos)
      double phi = 2 * PI*rnd.nextDouble();
      double cos_shita = 1 - (1 - max_cos_shita) * rnd.nextDouble();
      double sin_shita = sqrt(1 - cos_shita*cos_shita);

      Vector3 dir = normal * cos_shita + axis1 * sin_shita * sin(phi) + axis2 * sin_shita * cos(phi);

      sampledPoint = this->position + dir * (m_radius + EPS);
      sampledPointNormal = dir;

      return true;
    }

    virtual double TotalPower() const {
      return 4 * PI * m_radius * m_radius * material.emission.length();
    }
  };
}
