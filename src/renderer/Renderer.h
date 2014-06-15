#pragma once

#include "Color.h"

namespace OmochiRenderer {

  class Scene;

  class Renderer {
  protected:
    bool m_enableRendering;
  public:
    Renderer() : m_enableRendering(true) {}
    virtual ~Renderer() {};

    virtual void RenderScene(const Scene &scene) = 0;
    virtual const Color *GetResult() const = 0;
    virtual const int GetCurrentSampleCount() const = 0;
    virtual std::string GetCurrentRenderingInfo() const { return ""; };

    void StopRendering() { m_enableRendering = false;  }
  };
}
