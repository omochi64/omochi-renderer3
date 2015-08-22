#pragma once

#include "SceneObject.h"
#include "Color.h"
#include "Ray.h"
#include "HitInformation.h"
#include "Material.h"

namespace OmochiRenderer {

class Sphere : public SceneObject {
public:
  Sphere(double r, const Vector3 &pos, const Material &material_)
    : SceneObject(material_)
    , m_radius(r)
  {
    position_ = pos;
    boundingBox_.SetBox(pos - Vector3(r,r,r), pos + Vector3(r,r,r));
  }

  bool CheckIntersection(const Ray &ray, HitInformation &hit) const
  {
    // x: the origin of the ray
    // v: normalized direction of the ray
    // c: the center of this sphere
    const Vector3 &x = ray.orig;
    const Vector3 &v = ray.dir;
    const Vector3 &c = position_;

    // 判別式
    Vector3 c_minus_x = c-x;
    double v_dot_c_minus_x = v.dot(c_minus_x);
    double D1 = v_dot_c_minus_x * v_dot_c_minus_x;
    double D2 = v.lengthSq() * (c_minus_x.lengthSq() - m_radius*m_radius);
    double D = D1 - D2;
    if (D < 0) return false;

    double sqrtD = sqrt(D);
    double t1 = sqrtD + v_dot_c_minus_x;
    double t2 = -sqrtD + v_dot_c_minus_x;

    if (t1 <= EPS && t2 <= EPS) return false;
    
    if (t2 > EPS) hit.distance = t2;
    else hit.distance = t1;

    hit.position = x + v * hit.distance;
    hit.normal = hit.position - position_; hit.normal.normalize();

    // 緯度・経度形式のテクスチャを仮定して uv を計算する
    // texture を持ってなかったら計算しない
    bool has_uv = false;
    for (auto &mat : materials_)
    {
      if (mat.material_.texture_id != ImageHandler::INVALID_IMAGE_ID)
      {
        has_uv = true;
        break;
      }
    }
    if (has_uv)
    {
      // ここのu,vは画像左下をoriginとする座標
      double v = (std::asin(hit.normal.y) + PI*0.5)/PI;
      while (v > 1) v -= 1;
      while (v < 0) v += 1;

      double u = (std::atan2(hit.normal.z, hit.normal.x) + PI) / (2*PI);
      while (u > 1) u -= 1;
      while (u < 0) u += 1;

      // 左上originに変更
      hit.uv.x = u;
      hit.uv.y = 1-v;
    }

    return true;
  }

protected:
  double m_radius;
};

}
