#pragma once

#include "Renderer.h"
#include "Camera.h"

namespace OmochiRenderer {

  class PhotonMapping : public Renderer {
  public:
    explicit PhotonMapping(const Camera &camera);
    virtual ~PhotonMapping();

    virtual void RenderScene(const Scene &scene);

    virtual const Color *GetResult() const { return m_result; }

    virtual std::string GetCurrentRenderingInfo() const;

  private:
    Color *m_result;
  };
}