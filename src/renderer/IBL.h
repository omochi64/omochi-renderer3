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
      // 2v-1 = cosθ と変数変換 (pdf を計算するときにその計算を容易にするため)
      //  E_{θ=a} = E_θ(2v-1)*cos(θ-a)
      // これの E_θ(2v-1) をテーブルにしている (cos(θ-a)の部分は、実際にサンプリングするときに計算する)
      for (size_t yi = 0; yi < height; yi++) {
        for (size_t xi = 0; xi < width; xi++) {
          const double u = (xi + 0.5) / width;
          const double v = (yi + 0.5) / height;

          const double phi = u * 2.0 * PI;
          const double y = 2 * v - 1.0;
          //double phi, theta;
          //UVToPhiTheta(u, v, phi, theta);
          //const double y = cos(theta);

          xyToEnv[yi*width + xi] = Sample(Vector3(
            sqrt(1.0 - y*y) * cos(phi),
            y,
            sqrt(1.0 - y*y) * sin(phi)
            ));
        }
      }

      const double width_scale = 1.0 * width / m_importance_map_size;
      const double height_scale = 1.0 * height / m_importance_map_size;

      m_luminance_map.clear(); m_luminance_map.resize(m_importance_map_size * m_importance_map_size);
      m_direction_map.clear(); m_direction_map.resize(m_importance_map_size * m_importance_map_size);

      // importance map の作成
      // map の index から accumulated luminance と ray direction を引けるようにしておく
      for (size_t yi = 0; yi < m_importance_map_size; yi++) {
        for (size_t xi = 0; xi < m_importance_map_size; xi++) {
          const double u = (xi + 0.5) / m_importance_map_size;
          const double v = (yi + 0.5) / m_importance_map_size;

          //double phi, theta;
          //UVToPhiTheta(u, v, phi, theta);
          //double y = cos(theta);

          const double phi = u * 2.0 * PI;
          const double y = 2 * v - 1.0;   // cosθ
          const Vector3 vec(sqrt(1.0 - y*y)*cos(phi), y, sqrt(1.0 - y*y)*sin(phi));

          size_t index = yi * m_importance_map_size + xi;

          m_direction_map[index] = vec;

          Color tmp;
          for (int yi_ = static_cast<int>(yi * height_scale); yi_ < static_cast<int>((yi + 1)*height_scale); yi_++) {
            for (int xi_ = static_cast<int>(xi * width_scale); xi_ < static_cast<int>((xi + 1)*width_scale); xi_++) {
              tmp += xyToEnv[yi_ * width + xi_];
            }
          }
          m_luminance_map[index] = 0.298912 * tmp.x + 0.586611 * tmp.y + 0.114478 * tmp.z;
        }
      }

      // importance map の (x,y) を選ぶ確率密度関数を
      //  C * luminance_rate(x,y) (以降、luminance_rate(x,y) = l_{x,y})
      // とする。これを積分する
      //  C∬l_{x,y}*sinθdθdφ (θ:0->PI, φ:0->2PI)
      // これらを、上記で変換しているように、
      //  cosθ = 2*v - 1, φ = 2*u*PI
      // と変数変換すると、
      //  4*PI*C∬l_{x,y}*dudv (u:0->1, v:-0.5->0.5)
      // となる。ここで、l_{x,y}はu,vについて、それぞれ1辺を 1/map_size とする正方形の代表値としてあらわされている
      // つまり、その領域内では値が一定のため、上記の積分は
      //  4PI*CΣl_{(0.5+xi)/map_size, (0.5+yi)/map_size}*ΔuΔv
      // のように置き換えられる。ΔuΔvは l_{x,y} が代表値をとっている領域の面積なので、 1/map_size^2 となる。
      // また、l は正規化されたものを使うので、その合計値は1となる。つまり、
      //  C∬l_{x,y}*sinθdθdφ = 4PI/map_size^2 * C
      // となるので、 C = map_size^2/4PI で、 pdf = luminance_rate(x,y) * map_size^2 / 4PI となる。
      // 上記の map 作成時、 y (=cosθ) = 2v - 1 とせず、θ = v*PI とすると、また計算が異なってくる

    }

  private:
    HDRImage m_image;
    static const int m_importance_map_size = 64;
    std::vector<double> m_luminance_map;
    std::vector<Vector3> m_direction_map;

    double m_radius;
    Vector3 m_center;
  };
}