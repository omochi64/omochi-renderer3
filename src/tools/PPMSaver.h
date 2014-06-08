#pragma once
#include <ctime>

#include "FileSaver.h"
#include "PPM.h"

namespace OmochiRenderer {
  class PPMSaver : public FileSaver {
  public:
    explicit PPMSaver(std::shared_ptr<Settings> settings)
      : FileSaver(settings)
    {
    }

    void Save(int samples, const Color *img, double accumulatedPastTime) const {
      std::string name(P_CreateFileName(samples, accumulatedPastTime));
      clock_t begin, end;
      begin = clock();
      PPM::Save(name, img, m_settings->GetWidth(), m_settings->GetHeight());
      end = clock();
      cerr << "saving time = " << (double)(end - begin) / CLOCKS_PER_SEC << endl;
      //cerr << "Total rendering time = " << accumulatedPastTime << " min." << endl;
    }
  };
}
