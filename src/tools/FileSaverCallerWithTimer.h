#pragma once
#include <thread>

namespace OmochiRenderer {
  
  class FileSaver;
  class Renderer;
  
  class FileSaverCallerWithTimer {
  public:
    explicit FileSaverCallerWithTimer(std::weak_ptr<Renderer> renderer, std::shared_ptr<FileSaver> saver);

    void SetSaveTimerInformation(double saveSpanInSec) {
      m_saveSpan = saveSpanInSec;
    }

    void AddSaver(const std::shared_ptr<FileSaver> &saver) {
      m_savers.push_back(saver);
    }

    void SetMaxSaveCount(int maxCount) {
      m_maxSaveCount = maxCount;
    }

    void SetAimTimeToSaveFile(double timeToSave) {
      m_aimTimeToSaveFile = timeToSave;
    }

    // 別スレッドでのタイマー監視を開始する。必要な設定が行われていなかった場合は失敗して false が返ってくる
    bool StartTimer();

    // タイマーストップを命令し、ストップするまで待つ
    void StopAndWaitStopping();

  private:
    std::weak_ptr<Renderer> m_renderer;
    std::vector<std::shared_ptr<FileSaver>> m_savers;

    std::shared_ptr<std::thread> m_thread;

    bool m_stopSignal;
    double m_saveSpan;
    double m_lastSaveTime;
    double m_aimTimeToSaveFile;
    int m_saveCount;
    int m_maxSaveCount;
  };
}
