#pragma once

#include <iostream>
#include <string>
#include "Color.h"
#include "HDRImage.h"
#include "tools/Constant.h"

namespace OmochiRenderer {

  class IBL {
  public:

    IBL(const std::string &hdr_filename, const double radius = 1000.0, const Vector3 &centerPosition = Vector3::Zero())
      : m_image()
      , m_radius(radius)
      , m_center(centerPosition)
    {
      if (!m_image.ReadFromRadianceFile(hdr_filename)) {
        std::cerr << "failed to load HDR file: " << hdr_filename << std::endl;
        return;
      }

      CreateImportanceSamplingMap();
    }

    inline const Color &Sample(const Ray &ray) const {
      return Sample(ConvertRayToNormalizedDir(ray));
    }
    inline const Color &Sample(const Vector3 &dir) const {
      // (x, y, z) = (r*sin(theta)*cos(phi), r*cos(theta), r*sin(theta)*sin(phi))
      const double theta = acos(dir.y);
      double phi = acos(dir.x / sqrt(dir.x*dir.x + dir.z*dir.z));
      if (dir.z < 0.0) {
        phi = 2.0 * PI - phi;
      }

      double u, v;
      PhiThetaToUV(phi, theta, u, v);

      return m_image.GetPixel(
        static_cast<int>(u * m_image.GetWidth()) % m_image.GetWidth(),
        static_cast<int>(v * m_image.GetHeight()) % m_image.GetHeight()
      );
    }

    inline const Color &SampleOriginal(const Vector3 &dir) {
      const float r = static_cast<float>((1.0f / PI) * acos(dir.z) / sqrt(dir.x * dir.x + dir.y * dir.y));

      float u = static_cast<float>((dir.x * r + 1.0f) / 2.0f);
      float v = static_cast<float>((dir.y * r + 1.0f) / 2.0f);
      //float v = 1 - (dir.y * r + 1.0f) / 2.0f;

      if (u < 0.0f)
        u += 1.0f;
      if (v < 0.0f)
        v += 1.0f;

      const int x = (int)(u * m_image.GetWidth()) % m_image.GetWidth();
      const int y = m_image.GetHeight() - 1 - (int)(v * m_image.GetHeight()) % m_image.GetHeight();

      return m_image.GetPixel(x, y);
    }

  private:

    static void PhiThetaToUV(const double phi, const double theta, double &u, double &v) {
      u = phi / (2.0 * PI);
      //const double v = 1.0 - theta / PI;
      v = theta / PI;
    }
    static void UVToPhiTheta(const double u, const double v, double &phi, double &theta) {
      phi = u * (2.0 * PI);
      theta = v * PI;
    }

    Vector3 ConvertRayToNormalizedDir(const Ray &ray) const {
      const Vector3 &x = ray.orig;
      const Vector3 &v = ray.dir;
      const Vector3 &c = m_center;

      // 判別式
      Vector3 c_minus_x = c - x;
      double v_dot_c_minus_x = v.dot(c_minus_x);
      double D1 = v_dot_c_minus_x * v_dot_c_minus_x;
      double D2 = v.lengthSq() * (c_minus_x.lengthSq() - m_radius*m_radius);
      double D = D1 - D2;
      assert(D >= 0);

      double sqrtD = sqrt(D);
      double t1 = sqrtD + v_dot_c_minus_x;
      double t2 = -sqrtD + v_dot_c_minus_x;

      if (t1 <= EPS && t2 <= EPS) return ray.dir;

      double distance;
      if (t2 > EPS) distance = t2;
      else distance = t1;

      Vector3 res(x + v * distance); res.normalize();
      return res;
    }

    void CreateImportanceSamplingMap() {
      std::vector<Color> xyToEnv(m_image.GetWidth()*m_image.GetHeight());

      size_t width = m_image.GetWidth();
      size_t height = m_image.GetHeight();

      // (x, y) ピクセル => cos項を考慮したサンプル値 に変換
      // 各方向への放射輝度を、theta = a (aは任意) から見た時の放射輝度に変換している
      // E_{θ=a} = E_θ(cosθ)*cos(θ-a)
      // 2v-1 = cosθ と変数変換 (y 方向へのサンプルがリニアになるようにするため？それとも計算が軽いから、というだけ？):
      //  E_{θ=a} = E_θ(2v-1)*cos(θ-a)
      // これの E_θ(2v-1) をテーブルにしている (cos(θ-a)の部分は、実際にサンプリングするときに計算する)
      for (size_t yi = 0; yi < height; yi++) {
        for (size_t xi = 0; xi < width; xi++) {
          const double u = (xi + 0.5) / width;
          const double v = (yi + 0.5) / height;

          //const double phi = u * 2.0 * PI;
          //const double y = 2 * v - 1.0;
          double phi, theta;
          UVToPhiTheta(u, v, phi, theta);
          const double y = cos(theta);

          xyToEnv[yi*width + xi] = Sample(Vector3(
            sqrt(1.0 - y*y) * cos(phi),
            y,
            sqrt(1.0 - y*y) * sin(phi)
          ));
        }
      }

      const int width_scale = width / m_importance_map_size;
      const int height_scale = height / m_importance_map_size;

      for (size_t y = 0; y < m_importance_map_size; y++) {
        for (size_t x = 0; x < m_importance_map_size; x++) {
          const double u = (x + 0.5) / m_importance_map_size;
          const double v = (y + 0.5) / m_importance_map_size;

          double phi, theta;
          UVToPhiTheta(u, v, phi, theta);

          double y = cos(theta);
          const Vector3 vec(sqrt(1.0 - y*y)*cos(phi), y, sqrt(1.0 - y*y)*sin(phi));


        }
      }

    }

  private:
    HDRImage m_image;
    static const int m_importance_map_size = 64;
    std::vector<double> m_luminamce_map;

    double m_radius;
    Vector3 m_center;
  };
}