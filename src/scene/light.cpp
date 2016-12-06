#include <cmath>

#include "light.h"

using namespace std;

double DirectionalLight::distanceAttenuation(const Vec3d& P) const
{
  // distance to light is infinite, so f(di) goes to 0.  Return 1.
  return 1.0;
}


int DirectionalLight::shadowAttenuation(ray& r) const
{
  // YOUR CODE HERE:
  // You should implement shadow-handling code here.

  isect i;

  if(scene->intersect(r,i)) {
	  return 0;
  }
  else {
	  return 1;
  }
}

Vec3d DirectionalLight::getColor() const
{
  return color;
}

Vec3d DirectionalLight::getDirection(const Vec3d& P) const
{
  // for directional light, direction doesn't depend on P
  return -orientation;
}

double PointLight::distanceAttenuation(const Vec3d& P) const
{

  // YOUR CODE HERE

  // You'll need to modify this method to attenuate the intensity 
  // of the light based on the distance between the source and the 
  // point P.  For now, we assume no attenuation and just return 1.0

  double factor, distance;

  distance = (P-position).length();
  factor = 0.3 / (quadraticTerm * distance * distance + linearTerm * distance + constantTerm);
  //factor = 1.0 / (quadraticTerm * distance * distance + linearTerm * distance + constantTerm);
  //factor = (quadraticTerm * distance * distance + linearTerm * distance + constantTerm);
  //cout<<distance<<'\t'<<factor<<endl;
  return factor;
}

Vec3d PointLight::getColor() const
{
  return color;
}

Vec3d PointLight::getDirection(const Vec3d& P) const
{
  Vec3d ret = position - P;
  return ret;
}


int PointLight::shadowAttenuation(ray& r) const
{
  // YOUR CODE HERE:
  // You should implement shadow-handling code here.
  isect i;

  if(scene->intersect(r,i)) {
	  if (abs(i.t) < (r.p-position).length()) {
		  return 0;
	  }
	  else {
		  return 1;
	  }
  }
  else {
	  return 1;
  }
}
