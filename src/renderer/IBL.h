#pragma once

#include <iostream>
#include <string>
#include "Color.h"
#include "HDRImage.h"
#include "tools/Constant.h"

namespace OmochiRenderer {

  class IBL {
  public:

    IBL(const std::string &hdr_filename)
      : m_image()
    {
      if (!m_image.ReadFromRadianceFile(hdr_filename)) {
        std::cerr << "failed to load HDR file: " << hdr_filename << std::endl;
      }
    }

    inline const Color &Sample(const Vector3 &dir) {
      // (x, y, z) = (r*sin(theta)*cos(phi), r*cos(theta), r*sin(theta)*sin(phi))
      const double theta = acos(dir.y);
      double phi = acos(dir.x / sqrt(dir.x*dir.x + dir.z*dir.z));
      if (dir.z < 0.0) {
        phi = 2.0 * PI - phi;
      }

      const double u = phi / (2.0 * PI);
      //const double v = 1.0 - theta / PI;
      const double v = theta / PI;

      return m_image.GetPixel(
        static_cast<int>(u * m_image.GetWidth()) % m_image.GetWidth(),
        static_cast<int>(v * m_image.GetHeight()) % m_image.GetHeight()
      );
    }

    inline const Color &SampleOriginal(const Vector3 &dir) {
      const float r = (1.0f / PI) * acos(dir.z) / sqrt(dir.x * dir.x + dir.y * dir.y);

      float u = (dir.x * r + 1.0f) / 2.0f;
      float v = (dir.y * r + 1.0f) / 2.0f;
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
    HDRImage m_image;
  };

}