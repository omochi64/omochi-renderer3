#pragma once
#include <ctime>

#include "FileSaver.h"
#include "ImageHandler.h"
#include "HDRImage.h"

namespace OmochiRenderer {
  class RadianceSaver : public FileSaver {
  public:
    explicit RadianceSaver(std::shared_ptr<Settings> settings)
      : FileSaver(settings)
    {
    }
    virtual ~RadianceSaver()
    {
    }

    virtual void Save(int samples, int saveCount, const Color *img, size_t width, size_t height, double accumulatedPastTime) {
      HDRImage hdrImage;
      hdrImage.m_width  = width;
      hdrImage.m_height = height;

      std::string name(P_CreateFileName(samples, saveCount, accumulatedPastTime));
      clock_t begin, end;
      begin = clock();

      CopyColorArrayToImage(img, hdrImage.m_image,
                            static_cast<int>(hdrImage.m_width),
                            static_cast<int>(hdrImage.m_height), false);

      hdrImage.WriteToRadianceFile(name + ".hdr");

      end = clock();

      std::cerr << "saving time = " << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
    }
  };
}
