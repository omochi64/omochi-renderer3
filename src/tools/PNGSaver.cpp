#include "stdafx.h"

#include "PNGSaver.h"

namespace OmochiRenderer {
  void PNGSaver::Save(int samples, int saveCount, const Color *img, size_t width, size_t height, double accumulatedPastTime) {
    if (m_img == ImageHandler::INVALID_IMAGE_ID) return;

    std::string name(P_CreateFileName(samples, saveCount, accumulatedPastTime));
    clock_t begin, end;
    begin = clock();

    CopyColorArrayToImage(img, width, height);

    if (auto myImg = ImageHandler::GetInstance().GetImage(m_img))
    {
      ImageHandler::GetInstance().SaveToPngFile(name + ".png", myImg);
    }

    end = clock();

    std::cerr << "saving time = " << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
    //cerr << "Total rendering time = " << accumulatedPastTime << " min." << endl;
  }
}