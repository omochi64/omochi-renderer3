#pragma once

namespace OmochiRenderer {
  class WindowViewer {
  public:
    explicit WindowViewer();
    virtual ~WindowViewer();

    void StartViewerOnThisThread();
    void StartViewerOnNewThread();
  };
}
