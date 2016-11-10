//
// GraphicalUI.h
//
// The header file for the graphical UI
//

#ifndef __GraphicalUI_h__
#define __GraphicalUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>

#include "TraceUI.h"
#include "TraceGLWindow.h"
#include "debuggingWindow.h"
#include "CubeMapChooser.h"

class ModelerView;

class GraphicalUI : public TraceUI {
public:
	GraphicalUI();

	int run();

	void alert( const string& msg );

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_thresholdSlider;
	Fl_Slider*			m_blockSlider;
	Fl_Slider*			m_aaSampleSlider;
	Fl_Slider*			m_aaThreshSlider;
	Fl_Slider*			m_refreshSlider;
	Fl_Slider*			m_treeDepthSlider;
	Fl_Slider*			m_filterSlider;

	Fl_Slider*			m_dof_focal_distSlider;
	Fl_Slider*			m_dof_aperture_sizeSlider;
	Fl_Slider*			m_dof_aperture_sample_cntSlider;

	Fl_Slider*			m_brdf_reflect_max_angleSlider;
	Fl_Slider*			m_brdf_refract_max_angleSlider;

	Fl_Slider*			m_brdf_reflect_shininessSlider;
	Fl_Slider*			m_brdf_refract_shininessSlider;

	Fl_Slider*			m_brdf_reflect_sample_cntSlider;
	Fl_Slider*			m_brdf_refract_sample_cntSlider;

	Fl_Slider*			m_area_light_sample_cntSlider;

	Fl_Check_Button*	m_debuggingDisplayCheckButton;
	Fl_Check_Button*	m_aaCheckButton;
	Fl_Check_Button*	m_dof_CheckButton;
	Fl_Check_Button*	m_brdf_reflectCheckButton;
	Fl_Check_Button*	m_brdf_refractCheckButton;
	Fl_Check_Button*	m_area_lightCheckButton;
	Fl_Check_Button*	m_gen_textureCheckButton;

	Fl_Check_Button*	m_kdCheckButton;
	Fl_Check_Button*	m_cubeMapCheckButton;
	Fl_Check_Button*	m_ssCheckButton;
	Fl_Check_Button*	m_shCheckButton;
	Fl_Check_Button*	m_bfCheckButton;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	CubeMapChooser*     m_cubeMapChooser;

	TraceGLWindow*		m_traceGlWindow;

	DebuggingWindow*	m_debuggingWindow;

	// member functions
	void setRayTracer(RayTracer *tracer);
	RayTracer* getRayTracer() { return raytracer; }

	static void stopTracing();

	// static vars
	static char *traceWindowLabel;
	
private:

	clock_t refreshInterval;

	// static class members
	static Fl_Menu_Item menuitems[];

	static GraphicalUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);
	static void cb_exit3(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_refreshSlides(Fl_Widget* o, void* v);
	static void cb_aaSampleSlides(Fl_Widget* o, void* v);
	static void cb_dof_focal_distSlides(Fl_Widget* o, void* v);
	static void cb_dof_aperture_sizeSlides(Fl_Widget* o, void* v);
	static void cb_dof_aperture_sampleSlides(Fl_Widget* o, void* v);
	static void cb_brdf_reflect_sampleSlides(Fl_Widget* o, void* v);
	static void cb_brdf_refract_sampleSlides(Fl_Widget* o, void* v);
	static void cb_brdf_reflect_max_angleSlides(Fl_Widget* o, void* v);
	static void cb_brdf_refract_max_angleSlides(Fl_Widget* o, void* v);
	static void cb_brdf_reflect_shininessSlides(Fl_Widget* o, void* v);
	static void cb_brdf_refract_shininessSlides(Fl_Widget* o, void* v);

	static void cb_area_light_sampleSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);

	static void cb_stop(Fl_Widget* o, void* v);
	static void cb_debuggingDisplayCheckButton(Fl_Widget* o, void* v);
	static void cb_aaCheckButton(Fl_Widget* o, void* v);
	static void cb_ssCheckButton(Fl_Widget* o, void* v);
	static void cb_shCheckButton(Fl_Widget* o, void* v);
	static void cb_bfCheckButton(Fl_Widget* o, void* v);
	static void cb_dof_CheckButton(Fl_Widget* o, void* v);
	static void cb_brdf_reflectCheckButton(Fl_Widget* o, void* v);
	static void cb_brdf_refractCheckButton(Fl_Widget* o, void* v);
	static void cb_area_lightCheckButton(Fl_Widget* o, void* v);
	static void cb_gen_textureCheckButton(Fl_Widget* o, void* v);

	static bool stopTrace;
	static bool doneTrace;
	static GraphicalUI* pUI;
};

#endif
