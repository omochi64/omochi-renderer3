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
      , m_img(ImageHandler::INVALID_IMAGE_ID)
    {
      m_img = ImageHandler::GetInstance().CreateImage(settings->GetWidth(), settings->GetHeight());
    }
    virtual ~FileSaver()
    {
      ImageHandler::GetInstance().ReleaseImage(m_img);
      m_img = ImageHandler::INVALID_IMAGE_ID;
    }

    // 保存時に呼ばれる関数
    virtual void Save(int samples, int saveCount, const Color *img, size_t width, size_t height, double accumulatedPastTime) = 0;

  protected:

    // 内部用。引数のデータをこのインスタンスのデータにコピー
    void CopyColorArrayToImage(const Color *img, size_t width, size_t height)
    {
      auto myImg = ImageHandler::GetInstance().GetImage(m_img);
      if (!myImg) return;

      if (width != myImg->GetWidth() || height != myImg->GetHeight())
      {
        ImageHandler::GetInstance().ReleaseImage(m_img);
        m_img = ImageHandler::GetInstance().CreateImage(width, height);
        myImg = ImageHandler::GetInstance().GetImage(m_img);
      }
      if (!myImg) return;
      
      CopyColorArrayToImage(img, myImg->m_image, static_cast<int>(myImg->GetWidth()), static_cast<int>(myImg->GetHeight()));
    }

    // 引数に与えた変数間でColorデータコピー
    static void CopyColorArrayToImage(const Color *img, std::vector<Color> &copyTo, int width, int height, bool gamma = true)
    {
      std::vector<Color> tmp(width*height);

      std::copy(img, img + width * height, tmp.begin());

      // box filter
      //int filter_min = -2, filter_max = 2;
      ///*double table[5][5] = {
      //  {1.0, 4.0, 6, 4, 1},
      //  {4, 16, 24, 16, 4},
      //  {6, 24, 36, 24, 6},
      //  { 4, 16, 24, 16, 4 },
      //  { 1.0, 4.0, 6, 4, 1 }
      //};*/
      //double table[5][5] = {
      //  { 1.0, 1.0, 1, 1, 1 },
      //  { 1, 1, 1, 1, 1 },
      //  { 1, 1, 1, 1, 1 },
      //  { 1, 1, 1, 1, 1 },
      //  { 1.0, 1.0, 1, 1, 1 }
      //};
      //for (int y = 0; y < height; y++)
      //{
      //  for (int x = 0; x < width; x++)
      //  {
      //    Vector3 total; double count = 0;
      //    for (int dy = filter_min; dy <= filter_max; dy++) for (int dx = filter_min; dx <= filter_max; dx++)
      //    {
      //      int x_ = x + dx;
      //      int y_ = y + dy;
      //      if (x_ < 0 || y_ < 0 || x_ >= width || y_ >= height)
      //      {
      //        continue;
      //      }
      //      double weight = table[dy - filter_min][dx - filter_min];
      //      total += img[x_ + y_*width] * weight;
      //      count += weight;
      //    }
      //    tmp[x + y*width] = total / count;
      //  }
      //}

      copyTo.resize(width*height);
      if (gamma)
      {
        for (int y = 0; y < height; y++)
        {
          for (int x = 0; x < width; x++)
          {
            int index = x + y*width;
            copyTo[index].x = Utils::GammaRev(Utils::Clamp(tmp[index].x));
            copyTo[index].y = Utils::GammaRev(Utils::Clamp(tmp[index].y));
            copyTo[index].z = Utils::GammaRev(Utils::Clamp(tmp[index].z));
          }
        }
      }
      else
      {
        for (int y = 0; y < height; y++)
        {
          for (int x = 0; x < width; x++)
          {
            int index = x + y*width;
            copyTo[index].x = Utils::Clamp(tmp[index].x);
            copyTo[index].y = Utils::Clamp(tmp[index].y);
            copyTo[index].z = Utils::Clamp(tmp[index].z);
          }
        }

      }
    }

    // settings や保存時の呼び出し引数からファイル名を生成する内部用メソッド
    std::string P_CreateFileName(int samples, int saveCount, double accumulatedPastTime) const {
      const static std::string file_format_key = "save filename format for pathtracer";

      const static std::string samples01Keyword = "(%samples01%)";
      const static std::string samples02Keyword = "(%samples02%)";
      const static std::string samples03Keyword = "(%samples03%)";
      const static std::string samples04Keyword = "(%samples04%)";
      const static std::string superSamples01Keyword = "(%supersamples01%)";
      const static std::string superSamples02Keyword = "(%supersamples02%)";
      const static std::string superSamples03Keyword = "(%supersamples03%)";
      const static std::string superSamples04Keyword = "(%supersamples04%)";
      const static std::string accumulatedTime01Keyword = "(%accumulatedTime01%)";
      const static std::string accumulatedTime02Keyword = "(%accumulatedTime02%)";
      const static std::string accumulatedTime03Keyword = "(%accumulatedTime03%)";
      const static std::string accumulatedTime04Keyword = "(%accumulatedTime04%)";
      const static std::string widthKeyword = "(%width%)";
      const static std::string heightKeyword = "(%height%)";
      const static std::string saveCount01Keyword = "(%savecount01%)";
      const static std::string saveCount02Keyword = "(%savecount02%)";
      const static std::string saveCount03Keyword = "(%savecount03%)";
      const static std::string saveCount04Keyword = "(%savecount04%)";

      
      std::string base;

      if (m_settings->GetRawSetting(file_format_key).empty()) {
        base = "result_ibl_test_w%d_h%d_%04d_%dx%d_%.2fmin.ppm";
        //sprintf_s(name, 1024, "result_ibl_test_w%d_h%d_%04d_%dx%d_%.2fmin.ppm",
        //  m_settings->GetWidth(), m_settings->GetHeight(), samples, m_settings->GetSuperSamples(), m_settings->GetSuperSamples(), accumulatedPastTime);
      } else {
        base = m_settings->GetRawSetting(file_format_key);
        //sprintf_s(name, 1024, m_settings->GetRawSetting(file_format_key).c_str(),
        //  m_settings->GetWidth(), m_settings->GetHeight(), samples, m_settings->GetSuperSamples(), m_settings->GetSuperSamples(), accumulatedPastTime);
      }

      std::stringstream ss;
      ss.width(1);
      ss.fill('0');
      ss << samples;
      std::string newName = Utils::replace(base, samples01Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(2);
      ss.fill('0');
      ss << samples;
      newName = Utils::replace(newName, samples02Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(3);
      ss.fill('0');
      ss << samples;
      newName = Utils::replace(newName, samples03Keyword, ss.str());
      ss.str("");
      ss.clear();

      ss.width(4);
      ss.fill('0');
      ss << samples;
      newName = Utils::replace(newName, samples04Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(1);
      ss.fill('0');
      ss << m_settings->GetSuperSamples();
      newName = Utils::replace(newName, superSamples01Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(2);
      ss.fill('0');
      ss << m_settings->GetSuperSamples();
      newName = Utils::replace(newName, superSamples02Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(3);
      ss.fill('0');
      ss << m_settings->GetSuperSamples();
      newName = Utils::replace(newName, superSamples03Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(4);
      ss.fill('0');
      ss << m_settings->GetSuperSamples();
      newName = Utils::replace(newName, superSamples04Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.precision(1);
      ss << accumulatedPastTime;
      newName = Utils::replace(newName, accumulatedTime01Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.precision(2);
      ss << accumulatedPastTime;
      newName = Utils::replace(newName, accumulatedTime02Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.precision(3);
      ss << accumulatedPastTime;
      newName = Utils::replace(newName, accumulatedTime03Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss.precision(4);
      ss << accumulatedPastTime;
      newName = Utils::replace(newName, accumulatedTime04Keyword, ss.str());
      ss.clear();
      ss.str("");

      ss << m_settings->GetWidth();
      newName = Utils::replace(newName, widthKeyword, ss.str());
      ss.clear();
      ss.str("");

      ss << m_settings->GetHeight();
      newName = Utils::replace(newName, heightKeyword, ss.str());
      ss.clear();
      ss.str("");

      ss.width(1);
      ss.fill('0');
      ss << saveCount;
      newName = Utils::replace(newName, saveCount01Keyword, ss.str());
      ss.str("");
      ss.clear();

      ss.width(2);
      ss.fill('0');
      ss << saveCount;
      newName = Utils::replace(newName, saveCount02Keyword, ss.str());
      ss.str("");
      ss.clear();

      ss.width(3);
      ss.fill('0');
      ss << saveCount;
      newName = Utils::replace(newName, saveCount03Keyword, ss.str());
      ss.str("");
      ss.clear();

      ss.width(4);
      ss.fill('0');
      ss << saveCount;
      newName = Utils::replace(newName, saveCount04Keyword, ss.str());
      ss.str("");
      ss.clear();

      return newName;
    }

    std::shared_ptr<Settings> m_settings;
    ImageHandler::IMAGE_ID m_img;
  };

}

