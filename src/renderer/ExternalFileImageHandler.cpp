#include "stdafx.h"

#include "ExternalFileImageHandler.h"


#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

namespace OmochiRenderer
{
  Image * ExternalFileImageHandler::LoadFromFile(const std::string &fname, bool doReverseGamma)
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

  void ExternalFileImageHandler::ReleaseImage(Image *p)
  {
    delete p;
  }

  bool ExternalFileImageHandler::SaveToFile(const std::string &fname)
  {
    return false;
  }
  
}