#include "stdafx.h"

#include <fstream>

#include "ImageHandler.h"


#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "HDRImage.h"

namespace OmochiRenderer
{
  ImageHandler::~ImageHandler()
  {
  }

  // 色々読み込み。stb が対応しているものなら何でも読み込み可能
  ImageHandler::IMAGE_ID ImageHandler::LoadFromFile(const std::string &fname, bool doReverseGamma)
  {
    Image *image = nullptr;

    if (fname.find_last_of(".hdr") != std::string::npos)
    {
      HDRImage *hdrImage = new HDRImage;
      if (!hdrImage->ReadFromRadianceFile(fname)) {
        delete hdrImage;  return ImageHandler::INVALID_IMAGE_ID;
      }

      image = hdrImage;
    }
    else
    {
      int bpp;
      int width, height;

      // 既に読み込んでないか確認
      auto findIt = m_filenameToImageIndex.find(fname);
      if (findIt != m_filenameToImageIndex.end())
      {
        auto p = m_images[findIt->second];
        if (p != nullptr)
        {
          return findIt->second;
        }
      }

      // 読み込み実体

      auto pixels = stbi_load(fname.c_str(), &width, &height, &bpp, 0);
      if (pixels == nullptr) {
        return INVALID_IMAGE_ID;
      }

      int bytePerPixel = bpp;

      assert(bytePerPixel >= 3);

      image = new Image;

      image->m_width = width;
      image->m_height = height;
      image->m_image.resize(width*height);

      for (int i = 0; i < width*height; i++)
      {
        image->m_image[i].x = pixels[i*bytePerPixel + 0] / 255.0f;
        image->m_image[i].y = pixels[i*bytePerPixel + 1] / 255.0f;
        image->m_image[i].z = pixels[i*bytePerPixel + 2] / 255.0f;

        if (doReverseGamma)
        {
          image->m_image[i].x = Utils::InvGammaRev(image->m_image[i].x);
          image->m_image[i].y = Utils::InvGammaRev(image->m_image[i].y);
          image->m_image[i].z = Utils::InvGammaRev(image->m_image[i].z);
        }
      }

      stbi_image_free(pixels);
    }

    m_images.push_back(image);
    auto id = m_images.size() - 1;
    m_filenameToImageIndex[fname] = id;

    return id;
  }

  // 解放
  void ImageHandler::ReleaseImage(IMAGE_ID id)
  {
    if (id < 0 || id >= (signed)m_images.size()) {
      return;
    }

    delete m_images[id];
    m_images[id] = nullptr;
  }

  // 空の画像作成
  ImageHandler::IMAGE_ID ImageHandler::CreateImage(size_t width, size_t height)
  {
    Image *img = new Image;
    img->m_width = width;
    img->m_height = height;

    img->m_image.resize(width * height);

    m_images.push_back(img);

    return m_images.size()-1;
  }

  Image * ImageHandler::GetImage(IMAGE_ID id)
  {
    if (id < 0 || id >= (signed)m_images.size())
      return nullptr;

    return m_images[id];
  }

  const Image * ImageHandler::GetImage(IMAGE_ID id) const
  {
    if (id < 0 || id >= (signed)m_images.size())
      return nullptr;

    return m_images[id];
  }

  bool ImageHandler::SaveToPngFile(const std::string &fname, const Image *image) const
  {
    int width = image->GetWidth(); int height = image->GetHeight();
    unsigned char *data = new unsigned char[width * height * 3];

    for (int i = 0; i < width * height; i++)
    {
      data[i * 3 + 0] = Utils::ToRgb(image->m_image[i].x);
      data[i * 3 + 1] = Utils::ToRgb(image->m_image[i].y);
      data[i * 3 + 2] = Utils::ToRgb(image->m_image[i].z);
    }

    stbi_write_png(fname.c_str(), image->GetWidth(), image->GetHeight(), 3, data, 3 * image->GetWidth());

    delete[] data;

    return true;
  }

  bool ImageHandler::SaveToPpmFile(const std::string &fname, const Image *image) const
  {
    int width = image->GetWidth();
    int height = image->GetHeight();

    std::ofstream ofs(fname.c_str());
    ofs << "P3" << std::endl;
    ofs << image->GetWidth() << " " << image->GetHeight() << std::endl;
    ofs << 255 << std::endl;
    std::vector<std::string> lines(image->GetHeight());

    {
      for (int y = 0; y < height; y++) {
        std::stringstream ss;
        for (int x = 0; x < width; x++) {
          const Color &pixel = image->GetPixel(x, y);
          ss << Utils::ToRgb(pixel.x) << " " <<
            Utils::ToRgb(pixel.y) << " " <<
            Utils::ToRgb(pixel.z) << std::endl;
        }
        lines[y] = ss.str();
      }
    }

    for (int i = 0; i < height; i++) {
      ofs << lines[i];
    }
    return true;
  }
  
}