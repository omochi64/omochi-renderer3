#pragma once

#include <thread>

namespace OmochiRenderer {
  class Renderer;

  class StopRendererWithTimer {
  public:
    explicit StopRendererWithTimer(std::weak_ptr<Renderer> renderer);
    ~StopRendererWithTimer();

    void SetTimer(double timeToStop) {
      m_timeToStop = timeToStop;
    }

    // 別スレッドでのタイマー監視を開始する。必要な設定が行われていなかった場合は失敗して false が返ってくる
    bool StartTimer();

  private:
    std::weak_ptr<Renderer> m_renderer;

    std::shared_ptr<std::thread> m_thread;

    double m_timeToStop;
  };
}