//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

// who the hell cares if my identifiers are longer than 255 characters:
#pragma warning(disable : 4786)

#include <string>

using std::string;

class RayTracer;

class TraceUI {
public:
	TraceUI() : m_nDepth(0), m_nSize(512), m_threadSize(4), m_displayDebuggingInfo(false),
	m_aaInfo(false), m_aaSample(3),
	m_dof_Check(false), m_dof_dist(2.0), m_dof_aperture_size(0.05), m_dof_sample_num(10), 
	m_brdf_reflect_Check(0), m_brdf_reflect_sample_num(1), m_brdf_reflect_max_angle(6.0),m_brdf_reflect_shininess(3.0),
	m_brdf_refract_Check(0), m_brdf_refract_sample_num(1), m_brdf_refract_max_angle(6.0),m_brdf_refract_shininess(3.0),
	m_area_light_Check(0), m_area_light_sample_num(10),
	m_gen_texture_Check(0),
	m_shadows(true), m_smoothshade(true), raytracer(0),
	m_nFilterWidth(1)
	{}

	virtual int	run() = 0;

	// Send an alert to the user in some manner
	virtual void alert(const string& msg) = 0;

	// setters
	virtual void setRayTracer( RayTracer* r ) { raytracer = r; }
	void setCubeMap(bool b) { m_gotCubeMap = b; }
	void useCubeMap(bool b) { m_usingCubeMap = b; }

	// accessors:
	int	getSize() const { return m_nSize; }
	int	getDepth() const { return m_nDepth; }
	int	getAntiA() const { return m_aaInfo; }
	int	getAntiSample() const { return m_aaSample; }
	int	getThreadSize() const { return m_threadSize; }
	int	getFilterWidth() const { return m_nFilterWidth; }

	int	getDofCheck() const { return m_dof_Check; }
	int	getDofSampleNum() const { return m_dof_sample_num; }
	double	getDofDist() const { return m_dof_dist; }
	double	getDofApertureSize() const { return m_dof_aperture_size; }

	bool	shadowSw() const { return m_shadows; }
	bool	smShadSw() const { return m_smoothshade; }

	int	getBrdfReflectCheck() const { return m_brdf_reflect_Check; }
	int	getBrdfReflectSampleNum() const { return m_brdf_reflect_sample_num; }
	double	getBrdfReflectMaxAngle() const { return m_brdf_reflect_max_angle; }
	double	getBrdfReflectShininess() const { return m_brdf_reflect_shininess; }

	int	getBrdfRefractCheck() const { return m_brdf_refract_Check; }
	int	getBrdfRefractSampleNum() const { return m_brdf_refract_sample_num; }
	double	getBrdfRefractMaxAngle() const { return m_brdf_refract_max_angle; }
	double	getBrdfRefractShininess() const { return m_brdf_refract_shininess; }

	int	getAreaLightCheck() const { return m_area_light_Check; }
	int	getAreaLightSampleNum() const { return m_area_light_sample_num; }

	int	getGenTextureCheck() const { return m_gen_texture_Check; }

	static bool m_debug;

protected:
	RayTracer*	raytracer;

	int	m_nSize;	// Size of the traced image
	int	m_nDepth;	// Max depth of recursion

	// Determines whether or not to show debugging information
	// for individual rays.  Disabled by default for efficiency
	// reasons.
	bool m_aaInfo;
	int m_aaSample;
	int m_threadSize;
	bool m_displayDebuggingInfo;
	bool m_shadows;  // compute shadows?
	bool m_smoothshade;  // turn on/off smoothshading?
	bool		m_usingCubeMap;  // render with cubemap
	bool		m_gotCubeMap;  // cubemap defined
	int m_nFilterWidth;  // width of cubemap filter

	bool m_dof_Check;
	double m_dof_dist;
	double m_dof_aperture_size;
	int m_dof_sample_num;

	int m_brdf_reflect_sample_num;
	int m_brdf_reflect_Check;
	double m_brdf_reflect_max_angle;
	double m_brdf_reflect_shininess;

	int m_brdf_refract_sample_num;
	int m_brdf_refract_Check;
	double m_brdf_refract_max_angle;
	double m_brdf_refract_shininess;

	int m_area_light_Check;
	int m_area_light_sample_num;

	bool m_gen_texture_Check;
};

#endif
