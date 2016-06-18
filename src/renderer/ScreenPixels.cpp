//
//  ScreenPixels.cpp
//  omochi-renderer3
//
//  Created by omochi on 2016/06/14.
//  Copyright © 2016年 omochi. All rights reserved.
//

#include "stdafx.h"
#include "ScreenPixels.hpp"

namespace OmochiRenderer
{

/**
 * @brief ScreenPixels が表現する画像情報の一部分を示したもの
 */
ScreenPixels::ScreenViewport::ScreenViewport()
  : m_screen(nullptr)
  , m_xOffset(0)
  , m_yOffset(0)
{
}

ScreenPixels::ScreenViewport::~ScreenViewport()
{
}
  
void ScreenPixels::ScreenViewport::initialize(ScreenPixels *screen, size_t xOffset, size_t yOffset)
{
  m_screen = screen;
  m_xOffset = xOffset;
  m_yOffset = yOffset;
}

// Viewport内の [x,y] に色を設定。Screen全体のx,yではない
void ScreenPixels::ScreenViewport::SetColor(size_t x, size_t y, const Color &color)
{
  auto index = calcIndex(x, y);
  
  m_screen->m_pixels[index] = color;
}
  
const Color &ScreenPixels::ScreenViewport::GetColor(size_t x, size_t y) const
{
  auto index = calcIndex(x, y);
  return m_screen->m_pixels[index];
}

size_t ScreenPixels::ScreenViewport::calcIndex(size_t x, size_t y) const
{
  assert(x < m_screen->m_widthPerView);
  assert(y < m_screen->m_heightPerView);
  
  auto width  = m_screen->CalcScreenWidth();
  auto height = m_screen->CalcScreenHeight();
  
  auto targetX = x + m_xOffset;
  auto targetY = y + m_yOffset;
  
  assert( targetX < width );
  assert( targetY < height );
  
  return targetX + (height - targetY - 1)*width;
}

/**
 * @brief スクリーンのピクセルを表現するためのラッパー。レンダリング結果を入れるためのもの
 */
ScreenPixels::ScreenPixels()
  : m_pixels(nullptr)
  , m_viewportList(nullptr)
  , m_widthPerView(0)
  , m_heightPerView(0)
  , m_xCount(0)
  , m_yCount(0)
{
  
}
  
ScreenPixels::~ScreenPixels()
{
  delete [] m_pixels;
  m_pixels = nullptr;
  
  delete [] m_viewportList;
  m_viewportList = nullptr;
}

void ScreenPixels::InitializePixels(size_t widthPerViewport, size_t heightPerViewport, size_t xViewportCount, size_t yViewportCount)
{
  delete [] m_pixels;
  delete [] m_viewportList;
  
  auto totalPixels = widthPerViewport * heightPerViewport * xViewportCount * yViewportCount;
  
  m_pixels = new Color[totalPixels];
  m_viewportList = new ScreenViewport[xViewportCount * yViewportCount];
  
  for (size_t y=0; y<yViewportCount; y++)
  {
    for (size_t x=0; x<xViewportCount; x++)
    {
      m_viewportList[x + y*xViewportCount].initialize(this, x * widthPerViewport, y * heightPerViewport);
    }
  }
  
  m_widthPerView  = widthPerViewport;
  m_heightPerView = heightPerViewport;
  m_xCount = xViewportCount;
  m_yCount = yViewportCount;
  
}

ScreenPixels::ScreenViewport *ScreenPixels::GetViewport(size_t xViewport, size_t yViewport)
{
  if (xViewport >= m_xCount) return nullptr;
  if (yViewport >= m_yCount) return nullptr;
  
  return &m_viewportList[xViewport + yViewport * m_xCount];
}

}
