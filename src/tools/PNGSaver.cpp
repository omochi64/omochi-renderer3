#include "stdafx.h"

#include "PNGSaver.h"

namespace OmochiRenderer {
  void PNGSaver::Save(int samples, const Color *img, double accumulatedPastTime) {
    if (m_img == nullptr) return;

    std::string name(P_CreateFileName(samples, accumulatedPastTime));
    clock_t begin, end;
    begin = clock();

    CopyColorArrayToImage(img);

    ImageHandler::GetInstance().SaveToPngFile(name + ".png", m_img);

    end = clock();

    std::cerr << "saving time = " << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
    //cerr << "Total rendering time = " << accumulatedPastTime << " min." << endl;
  }
}