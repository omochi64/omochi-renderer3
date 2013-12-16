#pragma once

#include "Color.h"

namespace OmochiRenderer {

  class Scene;

  class Renderer {
  public:
    virtual ~Renderer() {};

    virtual void RenderScene(const Scene &scene) = 0;
    virtual const Color *GetResult() const = 0;
    virtual std::string GetCurrentRenderingInfo() const { return ""; };
  };
}
