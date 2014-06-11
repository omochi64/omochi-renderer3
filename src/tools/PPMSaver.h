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

    virtual void Save(int samples, const Color *img, double accumulatedPastTime) {
      if (m_img == nullptr) return;

      std::string name(P_CreateFileName(samples, accumulatedPastTime));
      clock_t begin, end;
      begin = clock();

      CopyColorArrayToImage(img);

      ImageHandler::GetInstance().SaveToPpmFile(name + ".ppm", m_img);

      end = clock();

      std::cerr << "saving time = " << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
    }
  };
}
