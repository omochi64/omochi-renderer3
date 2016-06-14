#pragma once
#include <ctime>

#include "FileSaver.h"
#include "ImageHandler.h"

namespace OmochiRenderer {
  class PPMSaver : public FileSaver {
  public:
    explicit PPMSaver(std::shared_ptr<Settings> settings)
      : FileSaver(settings)
    {
    }
    virtual ~PPMSaver()
    {
    }

    virtual void Save(int samples, int saveCount, const Color *img, size_t width, size_t height, double accumulatedPastTime) {
      if (m_img == ImageHandler::INVALID_IMAGE_ID) return;

      std::string name(P_CreateFileName(samples, saveCount, accumulatedPastTime));
      clock_t begin, end;
      begin = clock();

      CopyColorArrayToImage(img, width, height);

      if (auto myImg = ImageHandler::GetInstance().GetImage(m_img))
      {
        ImageHandler::GetInstance().SaveToPpmFile(name + ".ppm", myImg);
      }

      end = clock();

      std::cerr << "saving time = " << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
    }
  };
}
