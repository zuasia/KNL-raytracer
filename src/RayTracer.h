#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/ray.h"
#include <time.h>
#include <queue>
#include <thread>

#if defined(_OPENMP)
#define OMP(x) _Pragma(x)
#include <omp.h>
#else
#define OMP(x)
static int omp_get_thread_num (void) { return 0; }
static int omp_get_num_threads (void) { return 1; }
#endif

class Scene;

class RayTracer
{
public:
	RayTracer();
        ~RayTracer();

	Vec3d tracePixel(int i, int j);

	void traceThreadTest();

	void traceImage(int width, int height);
	//void traceImageRange(int width, int height);
	void traceImageRange(int,int,int,int);

	Vec3d trace(double x, double y);
	Vec3d traceRay(ray& r, int depth);

	void getBuffer(unsigned char *&buf, int &w, int &h);
	double aspectRatio();

	void traceSetup( int w, int h );

	bool loadScene(char* fn);
	bool sceneLoaded() { return scene != 0; }

	void setReady(bool ready) { m_bBufferReady = ready; }
	bool isReady() const { return m_bBufferReady; }

	const Scene& getScene() { return *scene; }

public:
        unsigned char *buffer;
        int buffer_width, buffer_height;
        int bufferSize;
        Scene* scene;

        bool m_bBufferReady;
};

#endif // __RAYTRACER_H__
