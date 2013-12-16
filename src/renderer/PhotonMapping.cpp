#include "PhotonMapping.h"
#include "scenes/Scene.h"


namespace OmochiRenderer {
  PhotonMapping::PhotonMapping(const Camera &camera)
    : m_result(new Color[camera.GetScreenHeight()*camera.GetScreenWidth()])
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
