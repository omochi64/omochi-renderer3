#include "stdafx.h"

#include <fstream>

#include "ImageHandler.h"


#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

namespace OmochiRenderer
{
  Image * ImageHandler::LoadFromFile(const std::string &fname, bool doReverseGamma) const
  {
    int bpp;
    int width, height;
    
    auto pixels = stbi_load(fname.c_str(), &width, &height, &bpp, 0);

    int bytePerPixel = bpp;

    assert(bytePerPixel >= 3);

    Image *image = new Image;

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

    return image;
  }

  void ImageHandler::ReleaseImage(Image *p) const
  {
    delete p;
  }

  Image * ImageHandler::CreateImage(size_t width, size_t height) const
  {
    Image *img = new Image;
    img->m_width = width;
    img->m_height = height;

    img->m_image.resize(width * height);

    return img;
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