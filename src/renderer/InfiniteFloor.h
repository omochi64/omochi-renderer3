#pragma once

#include "SceneObject.h"
#include "Color.h"
#include "Ray.h"
#include "HitInformation.h"
#include "Material.h"

namespace OmochiRenderer {

class InfiniteFloor : public SceneObject {
public:
  InfiniteFloor(double y_, const Material &material_)
    : SceneObject(material_)
    , y(y_)
  {
  }

  bool CheckIntersection(const Ray &ray, HitInformation &hit) const
  {
    // x: the origin of the ray
    // v: normalized direction of the ray
    // c: the center of this sphere

    if (std::abs(ray.dir.y) < EPS) return false;

    double t = (y - ray.orig.y) / ray.dir.y;
    if (t < EPS) return false;

    hit.distance = t;
    hit.position = ray.orig + ray.dir * t;
    hit.normal.x = hit.normal.z = 0;
    if (ray.dir.y < 0) hit.normal.y = 1;
    else hit.normal.y = -1;

    return true;
  }
private:
  double y;


};

}
