#pragma once

#include <string>
#include <memory>
#include <thread>
#include <functional>

namespace OmochiRenderer {

  class Camera;
  class PathTracer;
  class ToonMapper;

  class WindowViewer {
  public:
    explicit WindowViewer(const std::string &windowTitle, const Camera &camera, const PathTracer &renderer, const ToonMapper &mapper, const size_t refreshSpanInMsec = 3000);
    virtual ~WindowViewer();

    void StartViewerOnThisThread();
    void StartViewerOnNewThread();

    void SetCallbackFunctionWhenWindowClosed(const std::function<void (void)> func) {
      m_callbackWhenClosed = func;
    }

    void WaitWindowFinish();

  private:
    bool CreateViewWindow();

  private:
    std::string m_windowTitle;
    const Camera &m_camera;
    const PathTracer &m_renderer;
    const ToonMapper &m_mapper;

    std::shared_ptr<std::thread> m_windowThread;
    std::function<void(void)> m_callbackWhenClosed;
    size_t m_refreshTimeInMsec;

    class WindowImpl;
    friend class WindowImpl;
    std::shared_ptr<WindowImpl> m_pWindow;
  };
}
