#pragma once

#include <functional>

#include "Renderer.h"
#include "Color.h"
#include "scenes/Scene.h"
#include "Camera.h"

namespace OmochiRenderer {

class Scene;
class Ray;
class Random;
class IBL;

class PathTracer : public Renderer {
public:
  // レンダリング完了時に呼び出すコールバック用メソッド定義
  typedef std::function<void(int samples, const Color *result, double accumulatedRenderingTime)> RenderingFinishCallbackFunction;

public:
	// コンストラクタ、デストラクタ
  PathTracer(const Camera &camera, int samples, int supersamples);
  PathTracer(const Camera &camera, int min_samples, int max_samples, int step, int supersamples, RenderingFinishCallbackFunction callback);
	virtual ~PathTracer();

  // Next Event Estimation の有効化/無効化の切り替え
  void EnableNextEventEstimation(bool enable = true) {
    m_performNextEventEstimation = enable;
  }
  
  // 実行時に利用可能なスレッド数設定
  void SetMaxThreadCount(size_t count) {
    m_threadCount = count == 0 ? 1 : count;   // 0はありえない
  }

	virtual void RenderScene(const Scene &scene);

	virtual const Color *GetResult() const {return m_result;}
  virtual const int GetCurrentSampleCount() const { return m_currentSamples; }

  virtual std::string GetCurrentRenderingInfo() const;

private:
  // 内部用初期化巻数
  void init(const Camera &camera, int min_samples, int max_samples, int step, int supersamples,
    RenderingFinishCallbackFunction callbackOnOneIterationEnded);

  void SetCamera(const Camera &cam) {
    m_camera = cam;
  }

  // 全ピクセルをスキャンし、レイを飛ばすメソッド
  void scanPixelsAndCastRays(const Scene &scene, int previous_samples, int next_samples);
  void scanPixelsOnYAndCastRays(const Scene &scene, int y, int previous_samples, int next_samples);
  // 与えられたレイについて、その放射輝度を求める
  Color Radiance(const Scene &scene, const Ray &ray, Random &rnd, const int depth);

  //Color DirectRadiance(const Scene &scene, const Ray &ray, Random &rnd, const int depth, const bool intersected, Scene::IntersectionInformation &intersect, const Vector3 &normal);

  Color DirectRadiance_Lambert(const Scene &scene, const Ray &ray, Random &rnd, const int depth, const bool intersected, Scene::IntersectionInformation &intersect, const Vector3 &normal, const Material &material);
  Color Radiance_internal(const Scene &scene, const Ray &ray, Random &rnd, const int depth, const bool intersected, Scene::IntersectionInformation &intersect, const Vector3 &normal);

  Color Radiance_Lambert(const Scene &scene, const Ray &ray, Random &rnd, const int depth, Scene::IntersectionInformation &intersect, const Vector3 &normal, double russian_roulette_prob, const Material &material);
  Color Radiance_Specular(const Scene &scene, const Ray &ray, Random &rnd, const int depth, Scene::IntersectionInformation &intersect, const Vector3 &normal, double russian_roulette_prob, const Material &material);
  Color Radiance_Refraction(const Scene &scene, const Ray &ray, Random &rnd, const int depth, Scene::IntersectionInformation &intersect, const Vector3 &normal, double russian_roulette_prob, const Material &material);

private:
  Camera m_camera;
  int m_currentSamples;
	int m_min_samples,m_max_samples,m_step_samples;
	int m_supersamples;
  int m_previous_samples;
  int m_processed_y_counts;
  RenderingFinishCallbackFunction m_renderFinishCallback;

  int m_checkIntersectionCount;
  int m_omittedRayCount;
  int m_hitToLightCount;
  
  size_t m_threadCount;

	Color *m_result;

  bool m_performNextEventEstimation = false;
};

}
