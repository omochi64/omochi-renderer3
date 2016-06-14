//
//  ScreenPixels.hpp
//  omochi-renderer3
//
//  Created by omochi on 2016/06/14.
//  Copyright © 2016年 omochi. All rights reserved.
//

#ifndef ScreenPixels_hpp
#define ScreenPixels_hpp

#include "Color.h"

namespace OmochiRenderer
{

/**
 * @brief スクリーンのピクセルを表現するためのラッパー。レンダリング結果を入れるためのもの
 */
class ScreenPixels
{
public:
  /**
   * @brief ScreenPixels が表現する画像情報の一部分を示したもの
   */
  class ScreenViewport
  {
  public:
    friend class ScreenPixels;
    
    ScreenViewport();
    ~ScreenViewport();
    
    // Viewport内の [x,y] に色を設定。Screen全体のx,yではない
    void SetColor(size_t x, size_t y, const Color &color);
    const Color &GetColor(size_t x, size_t y) const;
  
  private:
    void initialize(ScreenPixels *screen, size_t xOffset, size_t yOffset);
    size_t calcIndex(size_t x, size_t y) const;
    
    size_t m_xOffset;
    size_t m_yOffset;
    
    ScreenPixels *m_screen;
  };

public:
  ScreenPixels();
  ~ScreenPixels();
  
  void InitializePixels(size_t widthPerViewport, size_t heightPerViewport, size_t xViewportCount, size_t yViewportCount);
  
  ScreenViewport *GetViewport(size_t xViewport, size_t yViewport);
  const Color *GetScreen() const { return m_pixels; }
  
  size_t CalcScreenWidth()  const { return m_xCount * m_widthPerView; }
  size_t CalcScreenHeight() const { return m_yCount * m_heightPerView; }

private:
  Color *m_pixels;  // ピクセル全体はこちらで管理

  ScreenViewport *m_viewportList;  // ((m_viewportList + y * xCount) + x) で、管理先のViewport情報が観れる
  
  size_t m_widthPerView;   // Viewportごとの幅
  size_t m_heightPerView;  // Viewportごとの高さ
  
  size_t m_xCount;  // x方向に何個Viewportがあるか
  size_t m_yCount;  // y方向に何個Viewportがあるか
  
};

}


#endif /* ScreenPixels_hpp */
