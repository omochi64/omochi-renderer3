#pragma once

#include "Color.h"
#include "scenes/Scene.h"
#include "Camera.h"

namespace OmochiRenderer {

class Scene;
class Ray;
class Random;
class IBL;

class PathTracer {
public:
  class RenderingFinishCallback {
  public:
    virtual void operator()(int samples, const Color *result) = 0;
  };
public:
	PathTracer(const Camera &camera, int samples, int supersamples);
  PathTracer(const Camera &camera, int min_samples, int max_samples, int step, int supersamples, RenderingFinishCallback *callback, const std::string &hdrFileForIBL = "");
	~PathTracer();

	void SetCamera(const Camera &cam) {
    m_camera = cam;
	}

	void RenderScene(const Scene &scene);

	const Color *GetResult() const {return m_result;}

  std::string GetCurrentRenderingInfo() const;

private:
  void init(const Camera &camera, int min_samples, int max_samples, int step, int supersamples, RenderingFinishCallback *callback, const std::string &hdrFileForIBL);

  void ScanPixelsAndCastRays(const Scene &scene, int previous_samples, int next_samples);
  Color Radiance(const Scene &scene, const Ray &ray, Random &rnd, const int depth);

  Color Radiance_Lambert(const Scene &scene, const Ray &ray, Random &rnd, const int depth, Scene::IntersectionInformation &intersect, const Vector3 &normal, double russian_roulette_prob);
  Color Radiance_Specular(const Scene &scene, const Ray &ray, Random &rnd, const int depth, Scene::IntersectionInformation &intersect, const Vector3 &normal, double russian_roulette_prob);
  Color Radiance_Refraction(const Scene &scene, const Ray &ray, Random &rnd, const int depth, Scene::IntersectionInformation &intersect, const Vector3 &normal, double russian_roulette_prob);

private:
  Camera m_camera;
	int m_min_samples,m_max_samples,m_step_samples;
	int m_supersamples;
  int m_previous_samples;
  int m_processed_y_counts;
  RenderingFinishCallback *m_renderFinishCallback;

  int m_checkIntersectionCount;
  int m_omittedRayCount;
  int m_absorbedInObjectRayCount;

  std::auto_ptr<IBL> m_ibl;
	
	Color *m_result;
};

}
