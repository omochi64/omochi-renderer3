#pragma once

namespace OmochiRenderer {
  class WindowViewer {
  public:
    WindowViewer();
    virtual ~WindowViewer();

    void StartViewerOnThisThread();
    void StartViewerOnNewThread();
  };
}
