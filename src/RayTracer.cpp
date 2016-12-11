// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>


extern TraceUI* traceUI;

#include <iostream>
#include <fstream>
#define PI 3.1415926

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates (x,y),
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

Vec3d RayTracer::trace(double x, double y)
{
  // Clear out the ray cache in the scene for debugging purposes,
  //if (TraceUI::m_debug) scene->intersectCache.clear();
  ray r(Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY);

  scene->getCamera().rayThrough(x,y,r);

  bool dof;
  double focal_dist, aperture_size;
  int simulate_count;
  Vec3d ret;

  dof = traceUI->getDofCheck();
  simulate_count = traceUI->getDofSampleNum();
  focal_dist = traceUI->getDofDist();
  aperture_size = traceUI->getDofApertureSize();

  //dof = true;
  //simulate_count = 20;
  //focal_dist = -8;
  //aperture_size = 0.03;

  if (dof) {
	  double cos_ray, image_dist;
	  Vec3d focal_point, image_point;

	  cos_ray = r.d[2];

	  image_dist  = scene->getCamera().getEye()[2];
	  image_point = r.at(image_dist/cos_ray);

	  focal_point = r.at((image_dist+focal_dist)/cos_ray);

	  double rand_x, rand_y;
	  Vec3d temp_dir;
	  for(int i=0; i<simulate_count; ++i) {
		  rand_x = ((double)rand() / (double)RAND_MAX - 0.5)* aperture_size+x;
		  rand_y = ((double)rand() / (double)RAND_MAX - 0.5)* aperture_size+y;

		  rand_x = min(rand_x,1.0);
		  rand_x = max(rand_x,0.0);
		  rand_y = min(rand_y,1.0);
		  rand_y = max(rand_y,0.0);

		  scene->getCamera().rayThrough(rand_x,rand_y,r);
		  image_point = r.at(image_dist/cos_ray);

		  temp_dir = focal_point - image_point;
		  temp_dir.normalize();
		  ray temp_r(image_point,temp_dir,ray::VISIBILITY);

		  ret += traceRay(temp_r, traceUI->getDepth());
	  }
	  ret /= (double) simulate_count;
  }
  else {
	  //cout<<x<<'\t'<<y<<'\n';
	  ret = traceRay(r, traceUI->getDepth());
  }

  ret.clamp();
  return ret;
}

void RayTracer::traceImage(int width, int height)
{
	int total_pix_num, thread_num;
	total_pix_num = height * width;
	thread_num = 8;

	std::vector<std::thread> render_threads;

	int thread_pix_num, thread_cnt, init_row, init_col;
	thread_pix_num = total_pix_num / thread_num;

	for(thread_cnt = 0; thread_cnt < thread_num; ++thread_cnt)
	{
		init_row = thread_pix_num * thread_cnt / width;
		init_col = thread_pix_num * thread_cnt - init_row * width;
		render_threads.push_back(std::thread(&RayTracer::traceImageRange, this, init_row, init_col, thread_pix_num, width));
	}

	init_row = thread_pix_num * thread_cnt / width;
	init_col = thread_pix_num * thread_cnt - init_row * width;
	render_threads.push_back(std::thread(&RayTracer::traceImageRange, this, init_row, init_col, total_pix_num - thread_pix_num*thread_num, width));

	for(auto&& i: render_threads) {
		i.join();
	}
}



void RayTracer::traceImageRange(int start_row, int start_col, int pix_num, int width)
{
	int x, y;
	for (int i=0; i< pix_num; ++i)
	{
		x = start_row + (start_col + i) / width; 
		y = (start_col + i) % width;

		//if (stopTrace) break;
		tracePixel(x, y);
	}
}

Vec3d RayTracer::tracePixel(int i, int j)
{
	Vec3d col(0,0,0);

	if( ! sceneLoaded() ) return col;

	double x, y;
	if(traceUI->getAntiA()) {
		double sample_offset;
		int sample_num, cnt1, cnt2;
		sample_num = traceUI->getAntiSample();
		for(cnt1=0;cnt1<sample_num;++cnt1) {
			sample_offset = ((double)rand() / (double)RAND_MAX)/(double)sample_num;
			x = (double(i) + 1.0/(double)sample_num*(double)cnt1 + sample_offset) /double(buffer_width);

			for(cnt2=0;cnt2<sample_num;++cnt2) {
				sample_offset = ((double)rand() / (double)RAND_MAX)/(double)sample_num;
				y = (double(j) + 1.0/(double)sample_num*(double)cnt2 + sample_offset) /double(buffer_height);

				col += trace(x, y);
			}
		}
		col *= 1.0/double(sample_num*sample_num);
	}
	else {
		x = double(i)/double(buffer_width);
		y = double(j)/double(buffer_height);
		col = trace(x, y);
	}


	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
	return col;
}


// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
Vec3d RayTracer::traceRay(ray& r, int depth)
{
	Vec3d colorC = Vec3d(0.0,0.0,0.0);
	Vec3d reflect_color = Vec3d(0.0,0.0,0.0);
	Vec3d refract_color = Vec3d(0.0,0.0,0.0);

	if (depth >= 0)
	{
		--depth;
		isect i;
		if(scene->intersect(r, i)) {

			const Material& m = i.getMaterial();
			colorC = m.shade(scene, r, i);

			Vec3d reflect_dir, refract_dir, income_dir, tmp_color, pos;

			income_dir = r.d;
			income_dir.normalize();
			pos = r.at(i.t);

			double cos_phi, sin_phi;
			cos_phi = -income_dir * i.N;
			sin_phi = sqrt(1-cos_phi*cos_phi);

			Vec3d base1, base2, base3;

			base1 = income_dir + i.N*cos_phi;
			reflect_dir = base1 + i.N*cos_phi;


			if (traceUI->getBrdfReflectCheck()) {
				//generate bases
				base2 = -i.N * sin_phi * sin_phi / cos_phi + base1;
				base2.normalize();

				base3 = i.N ^ income_dir;
				base3.normalize();

				Vec3d disperse_dir, final_dir;

				double random_num1, random_num2, angle, range, coefficient, total_coefficient;
				int num;

				range = tan(traceUI->getBrdfReflectMaxAngle()/180.0*PI);
				num=traceUI->getBrdfReflectSampleNum();
				total_coefficient=0.0;

				for(int cnt=0; cnt<num; cnt++){
					//generate distribution
					random_num1 = ((double)rand() / (double)RAND_MAX)*2.0-1.0;
					random_num2 = ((double)rand() / (double)RAND_MAX)*2.0-1.0;

					disperse_dir = random_num1 * base2 + random_num2 * base3;
					disperse_dir.normalize();

					random_num1 = (double)rand() / (double)RAND_MAX * range*2.0-range;
					final_dir = random_num1 * disperse_dir + reflect_dir;
					final_dir.normalize();
					angle = final_dir * reflect_dir;

					ray reflect(pos,final_dir,ray::VISIBILITY);

					//add to final color
					tmp_color = traceRay(reflect, depth);
					tmp_color %= m.kr(i);
					coefficient = pow(angle,traceUI->getBrdfReflectShininess());
					total_coefficient += coefficient;
					tmp_color *= coefficient;
					reflect_color += tmp_color;
				}
				reflect_color /= total_coefficient;
			}
			else {
				ray reflect(pos,reflect_dir,ray::VISIBILITY);
				tmp_color = traceRay(reflect, depth);
				tmp_color %= m.kr(i);
				reflect_color += tmp_color ;
			}
			colorC += reflect_color;
			
			
			if (!(m.kt(i)[0]<=0 && m.kt(i)[1]<=0 && m.kt(i)[2]<=0))
			{
				double n_i, n_t;
				Vec3d normal = i.N;

				if (income_dir * i.N <0) {
					n_i =1;
					n_t = m.index(i);
					normal = i.N;
				}
				else {
					n_i = m.index(i);
					n_t =1;
					normal = -i.N;
				}


				double r,c;
				r = n_i/n_t;
				c = -i.N * income_dir;

				double sin_refract;
				sin_refract = r*sqrt(1-c*c);


				if ( sin_refract <1 ) {
					Vec3d tangent;
					tangent = (income_dir + c*normal);
					tangent.normalize();
					refract_dir = tangent * sin_refract / sqrt(1-sin_refract*sin_refract) - normal;
					refract_dir.normalize();

					if (traceUI->getBrdfRefractCheck()) {
						//generate bases
						base2 = -i.N * sin_refract - base1/(sin_phi)*sqrt(1-sin_refract*sin_refract);
						base3 = i.N ^ income_dir;
						base3.normalize();

						Vec3d disperse_dir, final_dir;

						double random_num1, random_num2, angle, range, coefficient, total_coefficient;
						int num;

						range = tan(traceUI->getBrdfRefractMaxAngle()/180.0*PI);
						num=traceUI->getBrdfRefractSampleNum();
						total_coefficient=0.0;

						for(int cnt=0; cnt<num; cnt++){
							//generate distribution
							random_num1 = ((double)rand() / (double)RAND_MAX)*2.0-1.0;
							random_num2 = ((double)rand() / (double)RAND_MAX)*2.0-1.0;

							disperse_dir = random_num1 * base2 + random_num2 * base3;
							disperse_dir.normalize();

							random_num1 = (double)rand() / (double)RAND_MAX * range*2.0-range;
							final_dir = random_num1 * disperse_dir + refract_dir;
							final_dir.normalize();
							angle = final_dir * refract_dir;

							ray refract(pos,final_dir,ray::VISIBILITY);

							//add to final color
							tmp_color = traceRay(refract, depth);
							tmp_color %= m.kt(i);
							coefficient = pow(angle,traceUI->getBrdfRefractShininess());
							total_coefficient += coefficient;
							tmp_color *= coefficient;
							refract_color += tmp_color;
						}
						refract_color /= total_coefficient;
					}
					else {
						ray refract(pos,refract_dir,ray::VISIBILITY);
						tmp_color = traceRay(refract, depth);
						tmp_color %= m.kt(i);
						refract_color += tmp_color ;
					}
					colorC += refract_color;
				}
			}
		}
	}

	return colorC;
}

RayTracer::RayTracer()
	: scene(0), buffer(0), buffer_width(256), buffer_height(256), m_bBufferReady(false)
{}

RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn ) {
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos ) path = ".";
	else path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if( !sceneLoaded() ) return false;

	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	if (buffer_width != w || buffer_height != h)
	{
		buffer_width = w;
		buffer_height = h;
		bufferSize = buffer_width * buffer_height * 3;
		delete[] buffer;
		buffer = new unsigned char[bufferSize];
	}
	memset(buffer, 0, w*h*3);
	m_bBufferReady = true;
}

