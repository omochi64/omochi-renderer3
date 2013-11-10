#pragma once

#include <string>
#include <memory>

namespace OmochiRenderer {

  class Camera;
  class PathTracer;
  class ToonMapper;

  class WindowViewer {
  public:
    explicit WindowViewer(const std::string &windowTitle, const Camera &camera, const PathTracer &renderer, const ToonMapper &mapper);
    virtual ~WindowViewer();

    void StartViewerOnThisThread();
    void StartViewerOnNewThread();

    void WaitWindowFinish();

  private:
    bool CreateViewWindow();

  private:
    std::string m_windowTitle;
    const Camera &m_camera;
    const PathTracer &m_renderer;
    const ToonMapper &m_mapper;

    //std::shared_ptr<std::thread> m_windowThread;

    class WindowImpl;
    friend class WindowImpl;
    std::shared_ptr<WindowImpl> m_pWindow;
  };
}
