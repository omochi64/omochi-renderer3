#include "stdafx.h"

#include "PhotonMapping.h"
#include "scenes/Scene.h"
#include "renderer/ScreenPixels.hpp"


namespace OmochiRenderer {
  PhotonMapping::PhotonMapping(const Camera &camera)
    : m_result(nullptr)
  {
    // TODO:
  }

  PhotonMapping::~PhotonMapping()
  {
    delete[] m_result;
  }

  void PhotonMapping::RenderScene(const Scene &scene) {
    // TODO:
  }

  std::string PhotonMapping::GetCurrentRenderingInfo() const {
    // TODO:
    return "";
  }
}
