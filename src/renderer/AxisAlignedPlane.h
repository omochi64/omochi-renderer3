#pragma once

#include "SceneObject.h"
#include "tools/Vector.h"
#include "tools/Constant.h"
#include "HitInformation.h"

namespace OmochiRenderer {
  class AxisAlignedPlane : public SceneObject {
  public:
    enum PLANE_AXIS {
      PLANE_XY, PLANE_XZ, PLANE_YZ
    };

    AxisAlignedPlane(const Material &material_, const PLANE_AXIS axis, double offset = 0.0, bool flip = false)
      : SceneObject(material_)
      , m_axis(axis)
      , m_offset(offset)
      , m_normal(0,0,0)
    {
      switch (axis) {
      case PLANE_XY:
        boundingBox.SetBox(Vector3(-INF, -INF, offset - EPS), Vector3(INF, INF, offset + EPS));
        m_normal.z = flip ? -1 : 1;
        break;
      case PLANE_XZ:
        boundingBox.SetBox(Vector3(-INF, offset - EPS, -INF), Vector3(INF, offset + EPS, INF));
        m_normal.y = flip ? -1 : 1;
        break;
      case PLANE_YZ:
        boundingBox.SetBox(Vector3(offset - EPS, -INF, -INF), Vector3(offset + EPS, INF, INF));
        m_normal.x = flip ? -1 : 1;
        break;
      }
    }
    virtual ~AxisAlignedPlane() {}

    bool CheckIntersection(const Ray &ray, HitInformation &hit) const {
      double t = -INF;
      switch (m_axis) {
      case PLANE_XY:
        if (ray.dir.z == 0) break;
        t = (m_offset - ray.orig.z) / ray.dir.z;
        break;

      case PLANE_XZ:
        if (ray.dir.y == 0) break;
        t = (m_offset - ray.orig.y) / ray.dir.y;
        break;

      case PLANE_YZ:
        if (ray.dir.x == 0) break;
        t = (m_offset - ray.orig.x) / ray.dir.x;
        break;
      }

      if (t < EPS) return false;

      hit.distance = t;
      hit.normal = m_normal;
      hit.position = ray.orig + ray.dir * t;

      return true;
    }

  private:
    PLANE_AXIS m_axis;
    double m_offset;
    Vector3 m_normal;
  };
}
