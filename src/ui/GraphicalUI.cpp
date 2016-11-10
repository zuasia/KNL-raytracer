//
// GraphicalUI.cpp
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#ifndef COMMAND_LINE_ONLY

#include <FL/fl_ask.H>
#include "debuggingView.h"

#include "GraphicalUI.h"
#include "../RayTracer.h"

#define MAX_INTERVAL 500

#ifdef _WIN32
#define print sprintf_s
#else
#define print sprintf
#endif

bool GraphicalUI::stopTrace = false;
bool GraphicalUI::doneTrace = true;
GraphicalUI* GraphicalUI::pUI = NULL;
char* GraphicalUI::traceWindowLabel = "Raytraced Image";
bool TraceUI::m_debug = false;

//------------------------------------- Help Functions --------------------------------------------
GraphicalUI* GraphicalUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ((GraphicalUI*)(o->parent()->user_data()));
}

//--------------------------------- Callback Functions --------------------------------------------
void GraphicalUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	pUI = whoami(o);

	static char* lastFile = 0;
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			print(buf, "Ray <%s>", newfile);
			stopTracing();	// terminate the previous rendering
		} else print(buf, "Ray <Not Loaded>");

		pUI->m_mainWindow->label(buf);
		pUI->m_debuggingWindow->m_debuggingView->setDirty();

		if( lastFile != 0 && strcmp(newfile, lastFile) != 0 )
			pUI->m_debuggingWindow->m_debuggingView->resetCamera();

		pUI->m_debuggingWindow->redraw();
	}
}

void GraphicalUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	pUI = whoami(o);

	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void GraphicalUI::cb_exit(Fl_Menu_* o, void* v)
{
	pUI = whoami(o);

	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_debuggingWindow->hide();
	TraceUI::m_debug = false;
}

void GraphicalUI::cb_exit2(Fl_Widget* o, void* v) 
{
	pUI = (GraphicalUI *)(o->user_data());

	// terminate the rendering
	stopTracing();

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
	pUI->m_debuggingWindow->hide();
	TraceUI::m_debug = false;
}

void GraphicalUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project for CS384g.");
}

void GraphicalUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());

	// terminate the rendering so we don't get crashes
	stopTracing();

	pUI->m_nSize=int(((Fl_Slider *)o)->value());
	int width = (int)(pUI->getSize());
	int height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow(width, height);
}

void GraphicalUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void GraphicalUI::cb_refreshSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->refreshInterval=clock_t(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_aaSampleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_aaSample=int(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_dof_focal_distSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_dof_dist=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_dof_aperture_sizeSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_dof_aperture_size=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_dof_aperture_sampleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_dof_sample_num=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_brdf_reflect_sampleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_brdf_reflect_sample_num=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_brdf_reflect_max_angleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_brdf_reflect_max_angle=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_brdf_reflect_shininessSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_brdf_reflect_shininess=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_brdf_refract_sampleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_brdf_refract_sample_num=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_brdf_refract_max_angleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_brdf_refract_max_angle=double(((Fl_Slider *)o)->value()) ;
}

void GraphicalUI::cb_brdf_refract_shininessSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_brdf_refract_shininess=double(((Fl_Slider *)o)->value()) ;
}


void GraphicalUI::cb_area_light_sampleSlides(Fl_Widget* o, void* v)
{
	((GraphicalUI*)(o->user_data()))->m_area_light_sample_num=double(((Fl_Slider *)o)->value()) ;
}

//void GraphicalUI::cb_threadSizeSlides(Fl_Widget* o, void* v)
//{
//	((GraphicalUI*)(o->user_data()))->m_threadSize=int(((Fl_Slider *)o)->value()) ;
//}

void GraphicalUI::cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_displayDebuggingInfo = (((Fl_Check_Button*)o)->value() == 1);
	if (pUI->m_displayDebuggingInfo)
	  {
	    pUI->m_debuggingWindow->show();
	    pUI->m_debug = true;
	  }
	else
	  {
	    pUI->m_debuggingWindow->hide();
	    pUI->m_debug = false;
	  }
}

void GraphicalUI::cb_aaCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_aaInfo = (((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_dof_CheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_dof_Check = (((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_brdf_reflectCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_brdf_reflect_Check = (((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_brdf_refractCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_brdf_refract_Check = (((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_area_lightCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_area_light_Check = (((Fl_Check_Button*)o)->value() == 1);
}

void GraphicalUI::cb_gen_textureCheckButton(Fl_Widget* o, void* v)
{
	pUI=(GraphicalUI*)(o->user_data());
	pUI->m_gen_texture_Check = (((Fl_Check_Button*)o)->value() == 1);
}
/*
void GraphicalUI::cb_render(Fl_Widget* o, void* v) 
{

	char buffer[256];

	pUI = (GraphicalUI*)(o->user_data());
	doneTrace = stopTrace = false;
	if (pUI->raytracer->sceneLoaded())
	{
		int width = pUI->getSize();
		int height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		int origPixels = width * height;
		pUI->m_traceGlWindow->resizeWindow(width, height);
		pUI->m_traceGlWindow->show();
		pUI->raytracer->traceSetup(width, height);

		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		clock_t now, prev;
		now = prev = clock();
		clock_t intervalMS = pUI->refreshInterval * 100;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (stopTrace) break;
				// check for input and refresh view every so often while tracing
				now = clock();
				if ((now - prev)/CLOCKS_PER_SEC * 1000 >= intervalMS)
				{
					prev = now;
					sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
					pUI->m_traceGlWindow->label(buffer);
					pUI->m_traceGlWindow->refresh();
					Fl::check();
					if (Fl::damage()) { Fl::flush(); }
				}
				// look for input and refresh window
				pUI->raytracer->tracePixel(x, y);
				pUI->m_debuggingWindow->m_debuggingView->setDirty();
			}
			if (stopTrace) break;
		}
		doneTrace = true;
		stopTrace = false;
		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);
		pUI->m_traceGlWindow->refresh();
	}
}
*/

void GraphicalUI::cb_render(Fl_Widget* o, void* v) 
{

	char buffer[256];

	pUI = (GraphicalUI*)(o->user_data());
	doneTrace = stopTrace = false;
	if (pUI->raytracer->sceneLoaded())
	{
		int width = pUI->getSize();
		int height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		int origPixels = width * height;
		pUI->m_traceGlWindow->resizeWindow(width, height);
		pUI->m_traceGlWindow->show();
		pUI->raytracer->traceSetup(width, height);

		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		clock_t now, prev;
		now = prev = clock();
		clock_t intervalMS = pUI->refreshInterval * 100;

		pUI->raytracer->traceImage(width, height);

		doneTrace = true;
		stopTrace = false;
		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);
		pUI->m_traceGlWindow->refresh();

	}
}

void GraphicalUI::cb_stop(Fl_Widget* o, void* v)
{
	pUI = (GraphicalUI*)(o->user_data());
	stopTracing();
}

int GraphicalUI::run()
{
	Fl::visual(FL_DOUBLE|FL_INDEX);

	m_mainWindow->show();

	return Fl::run();
}

void GraphicalUI::alert( const string& msg )
{
	fl_alert( "%s", msg.c_str() );
}

void GraphicalUI::setRayTracer(RayTracer *tracer)
{
	TraceUI::setRayTracer(tracer);
	m_traceGlWindow->setRayTracer(tracer);
	m_debuggingWindow->m_debuggingView->setRayTracer(tracer);
}

// menu definition
Fl_Menu_Item GraphicalUI::menuitems[] = {
	{ "&File", 0, 0, 0, FL_SUBMENU },
	{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)GraphicalUI::cb_load_scene },
	{ "&Save Image...", FL_ALT + 's', (Fl_Callback *)GraphicalUI::cb_save_image },
	{ "&Exit", FL_ALT + 'e', (Fl_Callback *)GraphicalUI::cb_exit },
	{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
	{ "&About",	FL_ALT + 'a', (Fl_Callback *)GraphicalUI::cb_about },
	{ 0 },

	{ 0 }
};

void GraphicalUI::stopTracing()
{
	stopTrace = true;
}

GraphicalUI::GraphicalUI() : refreshInterval(10) {
	// init.
	m_mainWindow = new Fl_Window(100, 40, 450, 620, "Ray <Not Loaded>");
	m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
	// install menu bar
	m_menubar = new Fl_Menu_Bar(0, 0, 440, 25);
	m_menubar->menu(menuitems);

	// set up "render" button
	m_renderButton = new Fl_Button(360, 37, 70, 25, "&Render");
	m_renderButton->user_data((void*)(this));
	m_renderButton->callback(cb_render);

	// set up "stop" button
	m_stopButton = new Fl_Button(360, 65, 70, 25, "&Stop");
	m_stopButton->user_data((void*)(this));
	m_stopButton->callback(cb_stop);

	// install depth slider
	m_depthSlider = new Fl_Value_Slider(10, 40, 180, 20, "Recursion Depth");
	m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_depthSlider->type(FL_HOR_NICE_SLIDER);
	m_depthSlider->labelfont(FL_COURIER);
	m_depthSlider->labelsize(12);
	m_depthSlider->minimum(0);
	m_depthSlider->maximum(10);
	m_depthSlider->step(1);
	m_depthSlider->value(m_nDepth);
	m_depthSlider->align(FL_ALIGN_RIGHT);
	m_depthSlider->callback(cb_depthSlides);

	// install size slider
	m_sizeSlider = new Fl_Value_Slider(10, 65, 180, 20, "Screen Size");
	m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_sizeSlider->type(FL_HOR_NICE_SLIDER);
	m_sizeSlider->labelfont(FL_COURIER);
	m_sizeSlider->labelsize(12);
	m_sizeSlider->minimum(64);
	m_sizeSlider->maximum(1024);
	m_sizeSlider->step(2);
	m_sizeSlider->value(m_nSize);
	m_sizeSlider->align(FL_ALIGN_RIGHT);
	m_sizeSlider->callback(cb_sizeSlides);

	// install refresh interval slider
	m_refreshSlider = new Fl_Value_Slider(10, 90, 180, 20, "Screen Refresh Interval (0.1 sec)");
	m_refreshSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_refreshSlider->type(FL_HOR_NICE_SLIDER);
	m_refreshSlider->labelfont(FL_COURIER);
	m_refreshSlider->labelsize(12);
	m_refreshSlider->minimum(1);
	m_refreshSlider->maximum(300);
	m_refreshSlider->step(1);
	m_refreshSlider->value(refreshInterval);
	m_refreshSlider->align(FL_ALIGN_RIGHT);
	m_refreshSlider->callback(cb_refreshSlides);

	// set up alias checkbox
	m_aaCheckButton = new Fl_Check_Button(10, 130, 140, 20, "Antialias");
	m_aaCheckButton->user_data((void*)(this));
	m_aaCheckButton->callback(cb_aaCheckButton);
	m_aaCheckButton->value(m_aaInfo);

	// install upsampling slider
	m_aaSampleSlider = new Fl_Value_Slider(130, 130, 180, 20, "Upsampling diameter");
	m_aaSampleSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_aaSampleSlider->type(FL_HOR_NICE_SLIDER);
	m_aaSampleSlider->labelfont(FL_COURIER);
	m_aaSampleSlider->labelsize(12);
	m_aaSampleSlider->minimum(1);
	m_aaSampleSlider->maximum(100);
	m_aaSampleSlider->step(1);
	m_aaSampleSlider->value(3);
	m_aaSampleSlider->align(FL_ALIGN_RIGHT);
	m_aaSampleSlider->callback(cb_aaSampleSlides);

	// set up dof checkbox
	m_dof_CheckButton = new Fl_Check_Button(10, 180, 140, 20, "DOF effect");
	m_dof_CheckButton->user_data((void*)(this));
	m_dof_CheckButton->callback(cb_dof_CheckButton);
	m_dof_CheckButton->value(m_dof_Check);

	// install dof distance slider
	m_dof_focal_distSlider = new Fl_Value_Slider(130, 180, 180, 20, "Focal Distance");
	m_dof_focal_distSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_dof_focal_distSlider->type(FL_HOR_NICE_SLIDER);
	m_dof_focal_distSlider->labelfont(FL_COURIER);
	m_dof_focal_distSlider->labelsize(12);
	m_dof_focal_distSlider->minimum(-30);
	m_dof_focal_distSlider->maximum(30);
	m_dof_focal_distSlider->step(0.1);
	m_dof_focal_distSlider->value(2);
	m_dof_focal_distSlider->align(FL_ALIGN_RIGHT);
	m_dof_focal_distSlider->callback(cb_dof_focal_distSlides);

	// install aperture size slider
	m_dof_aperture_sizeSlider = new Fl_Value_Slider(130, 210, 180, 20, "Aperture Diameter");
	m_dof_aperture_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_dof_aperture_sizeSlider->type(FL_HOR_NICE_SLIDER);
	m_dof_aperture_sizeSlider->labelfont(FL_COURIER);
	m_dof_aperture_sizeSlider->labelsize(12);
	m_dof_aperture_sizeSlider->minimum(0);
	m_dof_aperture_sizeSlider->maximum(0.06);
	m_dof_aperture_sizeSlider->step(0.001);
	m_dof_aperture_sizeSlider->value(0.03);
	m_dof_aperture_sizeSlider->align(FL_ALIGN_RIGHT);
	m_dof_aperture_sizeSlider->callback(cb_dof_aperture_sizeSlides);

	// install aperture size slider
	m_dof_aperture_sizeSlider = new Fl_Value_Slider(130, 240, 180, 20, "Aperture Sample cnt");
	m_dof_aperture_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_dof_aperture_sizeSlider->type(FL_HOR_NICE_SLIDER);
	m_dof_aperture_sizeSlider->labelfont(FL_COURIER);
	m_dof_aperture_sizeSlider->labelsize(12);
	m_dof_aperture_sizeSlider->minimum(0);
	m_dof_aperture_sizeSlider->maximum(50);
	m_dof_aperture_sizeSlider->step(1);
	m_dof_aperture_sizeSlider->value(10);
	m_dof_aperture_sizeSlider->align(FL_ALIGN_RIGHT);
	m_dof_aperture_sizeSlider->callback(cb_dof_aperture_sampleSlides);

	// set up brdf reflect checkbox
	m_brdf_reflectCheckButton = new Fl_Check_Button(10, 290, 140, 20, "BRDF Reflect");
	m_brdf_reflectCheckButton->user_data((void*)(this));
	m_brdf_reflectCheckButton->callback(cb_brdf_reflectCheckButton);
	m_brdf_reflectCheckButton->value(m_brdf_reflect_Check);

	// install brdf reflect sample slider
	m_brdf_reflect_sample_cntSlider = new Fl_Value_Slider(130, 290, 180, 20, "Reflect Sample cnt");
	m_brdf_reflect_sample_cntSlider->user_data((void*)(this));	
	m_brdf_reflect_sample_cntSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_reflect_sample_cntSlider->labelfont(FL_COURIER);
	m_brdf_reflect_sample_cntSlider->labelsize(12);
	m_brdf_reflect_sample_cntSlider->minimum(1);
	m_brdf_reflect_sample_cntSlider->maximum(10);
	m_brdf_reflect_sample_cntSlider->step(1);
	m_brdf_reflect_sample_cntSlider->value(1);
	m_brdf_reflect_sample_cntSlider->align(FL_ALIGN_RIGHT);
	m_brdf_reflect_sample_cntSlider->callback(cb_brdf_reflect_sampleSlides);

	// install brdf reflect max angle slider
	m_brdf_reflect_max_angleSlider = new Fl_Value_Slider(130, 320, 180, 20, "Reflect Max Angle");
	m_brdf_reflect_max_angleSlider->user_data((void*)(this));	
	m_brdf_reflect_max_angleSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_reflect_max_angleSlider->labelfont(FL_COURIER);
	m_brdf_reflect_max_angleSlider->labelsize(12);
	m_brdf_reflect_max_angleSlider->minimum(0);
	m_brdf_reflect_max_angleSlider->maximum(45);
	m_brdf_reflect_max_angleSlider->step(0.1);
	m_brdf_reflect_max_angleSlider->value(5);
	m_brdf_reflect_max_angleSlider->align(FL_ALIGN_RIGHT);
	m_brdf_reflect_max_angleSlider->callback(cb_brdf_reflect_max_angleSlides);

	// install brdf reflect shininess slider
	m_brdf_reflect_shininessSlider = new Fl_Value_Slider(130, 350, 180, 20, "Reflect Shininess");
	m_brdf_reflect_shininessSlider->user_data((void*)(this));	
	m_brdf_reflect_shininessSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_reflect_shininessSlider->labelfont(FL_COURIER);
	m_brdf_reflect_shininessSlider->labelsize(12);
	m_brdf_reflect_shininessSlider->minimum(0);
	m_brdf_reflect_shininessSlider->maximum(100);
	m_brdf_reflect_shininessSlider->step(0.1);
	m_brdf_reflect_shininessSlider->value(3);
	m_brdf_reflect_shininessSlider->align(FL_ALIGN_RIGHT);
	m_brdf_reflect_shininessSlider->callback(cb_brdf_reflect_shininessSlides);

	// set up brdf refract checkbox
	m_brdf_refractCheckButton = new Fl_Check_Button(10, 400, 140, 20, "BRDF Refract");
	m_brdf_refractCheckButton->user_data((void*)(this));
	m_brdf_refractCheckButton->callback(cb_brdf_refractCheckButton);
	m_brdf_refractCheckButton->value(m_brdf_refract_Check);

	// set up brdf refract sample slider
	m_brdf_refract_sample_cntSlider = new Fl_Value_Slider(130, 400, 180, 20, "Refract Sample cnt");
	m_brdf_refract_sample_cntSlider->user_data((void*)(this));	
	m_brdf_refract_sample_cntSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_refract_sample_cntSlider->labelfont(FL_COURIER);
	m_brdf_refract_sample_cntSlider->labelsize(12);
	m_brdf_refract_sample_cntSlider->minimum(1);
	m_brdf_refract_sample_cntSlider->maximum(10);
	m_brdf_refract_sample_cntSlider->step(1);
	m_brdf_refract_sample_cntSlider->value(1);
	m_brdf_refract_sample_cntSlider->align(FL_ALIGN_RIGHT);
	m_brdf_refract_sample_cntSlider->callback(cb_brdf_refract_sampleSlides);

	// install brdf refract max angle slider
	m_brdf_refract_max_angleSlider = new Fl_Value_Slider(130, 430, 180, 20, "Refract Max Angle");
	m_brdf_refract_max_angleSlider->user_data((void*)(this));	
	m_brdf_refract_max_angleSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_refract_max_angleSlider->labelfont(FL_COURIER);
	m_brdf_refract_max_angleSlider->labelsize(12);
	m_brdf_refract_max_angleSlider->minimum(0);
	m_brdf_refract_max_angleSlider->maximum(45);
	m_brdf_refract_max_angleSlider->step(0.1);
	m_brdf_refract_max_angleSlider->value(5);
	m_brdf_refract_max_angleSlider->align(FL_ALIGN_RIGHT);
	m_brdf_refract_max_angleSlider->callback(cb_brdf_refract_max_angleSlides);

	// install brdf refract shininess slider
	m_brdf_refract_shininessSlider = new Fl_Value_Slider(130, 460, 180, 20, "Refract Shininess");
	m_brdf_refract_shininessSlider->user_data((void*)(this));	
	m_brdf_refract_shininessSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_refract_shininessSlider->labelfont(FL_COURIER);
	m_brdf_refract_shininessSlider->labelsize(12);
	m_brdf_refract_shininessSlider->minimum(0);
	m_brdf_refract_shininessSlider->maximum(100);
	m_brdf_refract_shininessSlider->step(0.1);
	m_brdf_refract_shininessSlider->value(3);
	m_brdf_refract_shininessSlider->align(FL_ALIGN_RIGHT);
	m_brdf_refract_shininessSlider->callback(cb_brdf_refract_shininessSlides);

	// set up area light checkbox
	m_brdf_refractCheckButton = new Fl_Check_Button(10, 510, 140, 20, "Area light");
	m_brdf_refractCheckButton->user_data((void*)(this));
	m_brdf_refractCheckButton->callback(cb_area_lightCheckButton);
	m_brdf_refractCheckButton->value(m_area_light_Check);

	// set up area light sample slider
	m_brdf_refract_sample_cntSlider = new Fl_Value_Slider(130, 510, 180, 20, "Area Sample cnt");
	m_brdf_refract_sample_cntSlider->user_data((void*)(this));	
	m_brdf_refract_sample_cntSlider->type(FL_HOR_NICE_SLIDER);
	m_brdf_refract_sample_cntSlider->labelfont(FL_COURIER);
	m_brdf_refract_sample_cntSlider->labelsize(12);
	m_brdf_refract_sample_cntSlider->minimum(1);
	m_brdf_refract_sample_cntSlider->maximum(20);
	m_brdf_refract_sample_cntSlider->step(1);
	m_brdf_refract_sample_cntSlider->value(10);
	m_brdf_refract_sample_cntSlider->align(FL_ALIGN_RIGHT);
	m_brdf_refract_sample_cntSlider->callback(cb_area_light_sampleSlides);

	// set up generate texture checkbox
	m_gen_textureCheckButton = new Fl_Check_Button(10, 560, 140, 20, "Generated Texture");
	m_gen_textureCheckButton->user_data((void*)(this));
	m_gen_textureCheckButton->callback(cb_gen_textureCheckButton);
	m_gen_textureCheckButton->value(m_gen_texture_Check);

	// set up debugging display checkbox
	m_debuggingDisplayCheckButton = new Fl_Check_Button(10, 590, 140, 20, "Debugging display");
	m_debuggingDisplayCheckButton->user_data((void*)(this));
	m_debuggingDisplayCheckButton->callback(cb_debuggingDisplayCheckButton);
	m_debuggingDisplayCheckButton->value(m_displayDebuggingInfo);


	//// install thread number slider
	//m_threadSizeSlider = new Fl_Value_Slider(10, 170, 180, 20, "Thread number");
	//m_threadSizeSlider->user_data((void*)(this));	
	//m_threadSizeSlider->type(FL_HOR_NICE_SLIDER);
	//m_threadSizeSlider->labelfont(FL_COURIER);
	//m_threadSizeSlider->labelsize(12);
	//m_threadSizeSlider->minimum(1);
	//m_threadSizeSlider->maximum(8);
	//m_threadSizeSlider->step(1);
	//m_threadSizeSlider->value(4);
	//m_threadSizeSlider->align(FL_ALIGN_RIGHT);
	//m_threadSizeSlider->callback(cb_threadSizeSlides);


	//main window
	m_mainWindow->callback(cb_exit2);
	m_mainWindow->when(FL_HIDE);
	m_mainWindow->end();
	

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, traceWindowLabel);
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);

	// debugging view
	m_debuggingWindow = new DebuggingWindow();
}

#endif
