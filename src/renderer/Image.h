#pragma once

namespace OmochiRenderer {
  class Image
  {
  public:
    Image()
      : m_image()
      , m_width(0)
      , m_height(0)
    {
    }

    size_t GetHeight() const { return m_height; }
    size_t GetWidth() const { return m_width; }
    const Color &GetPixel(size_t x, size_t y) const {
      assert(x < m_width && y < m_height);

      return m_image[x + y*m_width];
    }

    std::vector<Color> m_image;
    size_t m_width, m_height;
  };
}
