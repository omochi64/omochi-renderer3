#pragma once

#include "tools/Vector.h"

namespace OmochiRenderer {

  class Camera {
  public:
    Camera(size_t screenWidth, size_t screenHeight, const Vector3 &cameraPos = Vector3(50.0, 52.0, 220.0),
      const Vector3 &cameraDir = Vector3(0.0, -0.04, -1.0),
      const Vector3 &cameraUp = Vector3(0.0, 1.0, 0.0),
      const double screenHeightIn3Dcoordinate = 30.0, const double distanceToScreen = 40)
      : m_width(screenWidth)
      , m_height(screenHeight)
      , m_camPos(cameraPos)
      , m_camDir(cameraDir)
      , m_camUp(cameraUp)
      , m_nearScreenHeight(screenHeightIn3Dcoordinate)
      , m_distToScreen(distanceToScreen)
    {
      m_camDir.normalize();
      m_camUp.normalize();

      double nearScreenHeight = m_nearScreenHeight;
      double nearScreenWidth = m_nearScreenHeight * m_width / m_height;

      m_screenCenter = m_camPos + m_camDir * m_distToScreen;

      // スクリーンx方向を張るベクトル
      m_screenXaxis = m_camDir.cross(m_camUp);
      m_screenXaxis.normalize(); m_screenXaxis *= nearScreenWidth;

      // スクリーンy方向を張るベクトル
      m_screenYaxis = m_screenXaxis.cross(m_camDir);
      m_screenYaxis.normalize(); m_screenYaxis *= nearScreenHeight;

    }

    inline const size_t GetScreenWidth() const {return m_width;}
    inline const size_t GetScreenHeight() const {return m_height;}
    inline const Vector3 &GetCameraPosition() const {return m_camPos;}

    inline const Vector3 &GetScreenCenterPosition() const {
      return m_screenCenter;
    }

    inline const Vector3 &GetScreenAxisXvector() const {
      return m_screenXaxis;
    }
    inline const Vector3 &GetScreenAxisYvector() const {
      return m_screenYaxis;
    }

  private:
	  size_t m_width, m_height;
	  //int m_min_samples,m_max_samples,m_step_samples;
	  //int m_supersamples;

	  Vector3 m_camPos;
	  Vector3 m_camDir;
	  Vector3 m_camUp;
	  double m_nearScreenHeight;
	  double m_distToScreen;

    Vector3 m_screenXaxis, m_screenYaxis;
    Vector3 m_screenCenter;

  };

}
