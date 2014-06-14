#pragma once

#include <memory>

#include "Image.h"
#include "ImageHandler.h"
#include "renderer/Settings.h"

namespace OmochiRenderer {

  // ファイル保存のための基底クラス。実体化不可
  class FileSaver {
  public:
    explicit FileSaver(std::shared_ptr<Settings> settings)
      : m_settings(settings)
      , m_img(nullptr)
    {
      m_img = ImageHandler::GetInstance().CreateImage(settings->GetWidth(), settings->GetHeight());
    }
    virtual ~FileSaver()
    {
      ImageHandler::GetInstance().ReleaseImage(m_img);
      m_img = nullptr;
    }

    // 保存時に呼ばれる関数
    virtual void Save(int samples, const Color *img, double accumulatedPastTime) = 0;

  protected:

    void CopyColorArrayToImage(const Color *img)
    {
      CopyColorArrayToImage(img, m_img->m_image, m_img->GetWidth(), m_img->GetHeight());
    }
    static void CopyColorArrayToImage(const Color *img, std::vector<Color> &copyTo, int width, int height, bool gamma = true)
    {
      copyTo.resize(width*height);
      if (gamma)
      {
        for (size_t y = 0; y < height; y++)
        {
          for (size_t x = 0; x < width; x++)
          {
            size_t index = x + y*width;
            copyTo[index].x = Utils::GammaRev(Utils::Clamp(img[index].x));
            copyTo[index].y = Utils::GammaRev(Utils::Clamp(img[index].y));
            copyTo[index].z = Utils::GammaRev(Utils::Clamp(img[index].z));
          }
        }
      }
      else
      {
        for (size_t y = 0; y < height; y++)
        {
          for (size_t x = 0; x < width; x++)
          {
            size_t index = x + y*width;
            copyTo[index].x = img[index].x;
            copyTo[index].y = img[index].y;
            copyTo[index].z = img[index].z;
          }
        }

      }
    }

    // settings や保存時の呼び出し引数からファイル名を生成する内部用メソッド
    std::string P_CreateFileName(int samples, double accumulatedPastTime) const {
      char name[1024];
      const static std::string file_format_key = "save filename format for pathtracer";
      if (m_settings->GetRawSetting(file_format_key).empty()) {
        sprintf_s(name, 1024, "result_ibl_test_w%d_h%d_%04d_%dx%d_%.2fmin.ppm",
          m_settings->GetWidth(), m_settings->GetHeight(), samples, m_settings->GetSuperSamples(), m_settings->GetSuperSamples(), accumulatedPastTime);
      } else {
        sprintf_s(name, 1024, m_settings->GetRawSetting(file_format_key).c_str(),
          m_settings->GetWidth(), m_settings->GetHeight(), samples, m_settings->GetSuperSamples(), m_settings->GetSuperSamples(), accumulatedPastTime);
      }

      return name;
    }

    std::shared_ptr<Settings> m_settings;
    Image * m_img;
  };

}

