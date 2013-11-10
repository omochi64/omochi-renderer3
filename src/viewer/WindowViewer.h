#pragma once

#include <string>
#include <memory>

namespace OmochiRenderer {

  class Camera;
  class PathTracer;

  class WindowViewer {
  public:
    explicit WindowViewer(const std::string &windowTitle, const Camera &camera, const PathTracer &renderer);
    virtual ~WindowViewer();

    void StartViewerOnThisThread();
    void StartViewerOnNewThread();

  private:
    bool CreateViewWindow();

  private:
    std::string m_windowTitle;
    const Camera &m_camera;
    const PathTracer &m_renderer;

    class WindowImpl;
    friend class WindowImpl;
    std::shared_ptr<WindowImpl> m_pWindow;
  };
}
