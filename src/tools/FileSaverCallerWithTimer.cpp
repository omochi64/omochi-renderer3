#include "stdafx.h"

#include <thread>     //
#include "FileSaverCallerWithTimer.h"
#include "FileSaver.h"
#include "renderer/Renderer.h"

using namespace std;

namespace OmochiRenderer {

  FileSaverCallerWithTimer::FileSaverCallerWithTimer(std::weak_ptr<Renderer> renderer, std::shared_ptr<FileSaver> saver)
    : m_renderer(renderer)
    , m_savers()
    , m_thread()
    , m_stopSignal(false)
    , m_saveSpan(0)
    , m_lastSaveTime(0)
    , m_aimTimeToSaveFile(0)
    , m_saveCount(0)
    , m_maxSaveCount(0)
  {
    m_savers.push_back(saver);
  }

  bool FileSaverCallerWithTimer::StartTimer()
  {
    // タイマーを始められる条件をチェック
    if (m_saveSpan == 0) return false;
    if (m_renderer.expired()) return false;
    if (m_savers.size() == 0) return false;

    // 2つ以上走らせない
    if (m_thread != nullptr) {
      StopAndWaitStopping();
    }

    m_saveCount = 0;
    m_stopSignal = false;
    m_thread = std::shared_ptr<std::thread>(new std::thread(
      [this]() {
        
        clock_t start = 0, end = 0;
        double saveSpan = m_saveSpan*1000;
        double accTime = 0;
        double accDiff = 0;

        while (!m_stopSignal) {

          // Sleep しつつ実時間計測
          unsigned long sleepTime = saveSpan + accDiff - m_aimTimeToSaveFile * 1000;
          cerr << "Begin sleeping...: Sleep(static_cast<DWORD>(" << sleepTime << ")" << endl;
          start = clock();
          std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));

          // 保存実行
          double tmpAccTime = accTime + 1000.0*(clock() - start) / CLOCKS_PER_SEC;
          if (std::shared_ptr<Renderer> render = m_renderer.lock())
          {
            for (auto it = m_savers.begin(); it != m_savers.end(); it++)
            {
              (*it)->Save(render->GetCurrentSampleCount(), m_saveCount, render->GetResult(), tmpAccTime / 1000.0 / 60);
            }
          }
          else
          {
            // レンダラが消えていたのでおしまい
            break;
          }

          end = clock();

          // ぴったり合わなかった時間を次に持越し
          double pastTime = 1000.0*(end - start) / CLOCKS_PER_SEC;
          accDiff += saveSpan - pastTime;

          cerr << pastTime << " second past." << endl;
          accTime += pastTime;

          // 保存回数制限チェック
          m_saveCount++;
          if (m_maxSaveCount > 0 && m_saveCount >= m_maxSaveCount) {
            break;
          }

        }

        //m_thread.reset();
      }
    ));

    return true;
  }

  void FileSaverCallerWithTimer::StopAndWaitStopping()
  {
    if (m_thread == nullptr) return;

    m_stopSignal = true;
    if (m_thread->joinable()) {
      m_thread->join();
    }
    m_thread.reset();
  }
}
