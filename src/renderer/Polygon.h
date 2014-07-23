#pragma once

#include "SceneObject.h"
#include "Ray.h"
#include "tools/Constant.h"
#include "HitInformation.h"
#include "tools/Matrix.h"

namespace OmochiRenderer {

class Polygon : public SceneObject {
public:
  Polygon(const Vector3 &pos1, const Vector3 &pos2, const Vector3 &pos3,
    const Vector3 &uv1, const Vector3 &uv2, const Vector3 &uv3,
    const Vector3 &normal1, const Vector3 &normal2, const Vector3 &normal3,
    const Material &mat, const Vector3 &pos)
    : SceneObject(mat)
  {
    m_posAndEdges[0] = pos1;
    m_posAndEdges[1] = pos2 - pos1;
    m_posAndEdges[2] = pos3 - pos1;
    m_uvOrigAndEdges[0] = uv1;
    m_uvOrigAndEdges[1] = uv2 - uv1;
    m_uvOrigAndEdges[2] = uv3 - uv1;
    m_normalAndDiffs[0] = normal1;
    m_normalAndDiffs[1] = normal2 - normal1;
    m_normalAndDiffs[2] = normal3 - normal1;
    position = pos;
    reconstruct_boundingbox();
  }
  Polygon(const Polygon &polygon)
    : SceneObject(polygon.material)
  {
    for (int i=0; i<3; i++) {
      m_posAndEdges[i] = polygon.m_posAndEdges[i];
      //m_rotatedPosAndEdges[i] = polygon.m_rotatedPosAndEdges[i];
      m_uvOrigAndEdges[i] = polygon.m_uvOrigAndEdges[i];
      m_normalAndDiffs[i] = polygon.m_normalAndDiffs[i];
      //m_rotatedNormalAndDiffs[i] = polygon.m_rotatedNormalAndDiffs[i];
    }
    reconstruct_boundingbox();
  }
  virtual ~Polygon() {}

  static Vector3 CalculateNormal(const Vector3 &anticlockwise_v0, const Vector3 &anticlockwise_v1, const Vector3 &anticlockwise_v2) {
    Vector3 v((anticlockwise_v1-anticlockwise_v0).cross(anticlockwise_v2-anticlockwise_v0));
    v.normalize();
    return v;
  }

  void Transform(const Vector3 &pos, const Vector3 &scale = Vector3::One(), const Matrix &rot = Matrix::Identity()) {
    position = pos;
    for (int i=0; i<3; i++) {
      m_posAndEdges[i] = rot.Apply(m_posAndEdges[i]);
      m_posAndEdges[i].x *= scale.x;
      m_posAndEdges[i].y *= scale.y;
      m_posAndEdges[i].z *= scale.z;
      m_normalAndDiffs[i] = rot.Apply(m_normalAndDiffs[i]);
    }
    reconstruct_boundingbox();
  }

  bool CheckIntersection(const Ray &ray, HitInformation &hit) const {
    // 連立方程式を解く
    // 参考: http://shikousakugo.wordpress.com/2012/07/01/ray-intersection-3/
    const Vector3 &edge1 = m_posAndEdges[1];
    const Vector3 &edge2 = m_posAndEdges[2];

    Vector3 P(ray.dir.cross(edge2));
    double det = P.dot(edge1);

    if (det > EPS) {
      // solve u
      Vector3 T(ray.orig - (m_posAndEdges[0] + position));
      double u = P.dot(T);

      if (u>=0 && u<= det) {
        // solve v
        Vector3 Q(T.cross(edge1));
        double v = Q.dot(ray.dir);

        if (v>=0 && u+v<=det) {
          double t = Q.dot(edge2) / det;

          if (t>=EPS) {
            const Vector3 &uvEdge1 = m_uvOrigAndEdges[1];
            const Vector3 &uvEdge2 = m_uvOrigAndEdges[2];

            double u_rate = (u / det);
            double v_rate = v / det;

            hit.distance = t;
            hit.position = ray.orig + ray.dir*t;
            hit.normal = m_normalAndDiffs[1] * u_rate + m_normalAndDiffs[2] * v_rate + m_normalAndDiffs[0];
            hit.normal.normalize();
            hit.uv = uvEdge1 * u_rate + uvEdge2 * v_rate + m_uvOrigAndEdges[0];

            return true;
          }
        }
      }

    }

    return false;
  }

  Vector3 m_posAndEdges[3];  // m_posAndEdges[3]: pos0。m_pos_AndEdges[1,2]: pos1,2 - pos0
  Vector3 m_uvOrigAndEdges[3]; // m_uvOrigAndEdges[0]: uv0。 m_uvOrigAndEdges[1,2]: uv1,2 - uv0 の値
  Vector3 m_normalAndDiffs[3]; // m_rotatedNormalAndDiffs[0]: normal0。 m_rotatedNormalAndDiffs[1,2]: normal1,2 - normal0 の値

private:
  void reconstruct_boundingbox() {
    auto &pos0 = m_posAndEdges[0];
    auto pos1 = m_posAndEdges[1] + m_posAndEdges[0];
    auto pos2 = m_posAndEdges[2] + m_posAndEdges[0];
    boundingBox.SetBox( Vector3(
      std::min(std::min(pos0.x, pos1.x), pos2.x),
      std::min(std::min(pos0.y, pos1.y), pos2.y),
      std::min(std::min(pos0.z, pos1.z), pos2.z)
      ) + position, 
      Vector3(
        std::max(std::max(pos0.x, pos1.x), pos2.x),
        std::max(std::max(pos0.y, pos1.y), pos2.y),
        std::max(std::max(pos0.z, pos1.z), pos2.z)
      ) + position
    );
  }

private:
  //Vector3 m_rotatedPosAndEdges[3];
  //Vector3 m_rotatedNormalAndDiffs[3]; // m_rotatedNormalAndDiffs[0]: normal0。 m_rotatedNormalAndDiffs[1,2]: normal1,2 - normal0 の値

};

}
