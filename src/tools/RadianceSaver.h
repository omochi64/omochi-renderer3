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

    virtual void Save(int samples, int saveCount, const Color *img, double accumulatedPastTime) {
      HDRImage hdrImage;
      hdrImage.m_width = m_settings->GetWidth();
      hdrImage.m_height = m_settings->GetHeight();

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
