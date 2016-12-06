#include <cmath>
#include <iostream>
#include <float.h>
#include <algorithm>
#include <assert.h>
#include "trimesh.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
	vertices.push_back( v );
}

void Trimesh::addVertexMap( const Vec2d &v )
{
	verticemaps.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
	materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
	normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
	int vcnt = vertices.size();

	if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

	TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
	newFace->setTransform(this->transform);
	if (!newFace->degen) faces.push_back( newFace );


	// Don't add faces to the scene's object list so we can cull by bounding box
	// scene->add(newFace);
	return true;
}

char* Trimesh::doubleCheck()
	// Check to make sure that if we have per-vertex materials or normals
	// they are the right number.
{
	if( !materials.empty() && materials.size() != vertices.size() )
		return "Bad Trimesh: Wrong number of materials.";
	if( !normals.empty() && normals.size() != vertices.size() )
		return "Bad Trimesh: Wrong number of normals.";

	return 0;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	double tmin = 0.0;
	double tmax = 0.0;
	typedef Faces::const_iterator iter;
	bool have_one = false;
	for( iter j = faces.begin(); j != faces.end(); ++j )
	{
		isect cur;
		if( (*j)->intersectLocal( r, cur ) )
		{
			if( !have_one || (cur.t < i.t) )
			{
				i = cur;
				have_one = true;
			}
		}
	}
	if( !have_one ) i.setT(1000.0);
	return have_one;
}

bool TrimeshFace::intersect(ray& r, isect& i) const {
	return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{

	const Vec3d& a = parent->vertices[ids[0]];
	const Vec3d& b = parent->vertices[ids[1]];
	const Vec3d& c = parent->vertices[ids[2]];

	Vec3d d = r.getDirection();
	
	double tmp_a, t;

	tmp_a = normal * d;
	t = -(normal * r.getPosition() - dist) / tmp_a;

	if (t < RAY_EPSILON)
		return false;

	Vec3d P, vab, vac, vbc, vap, vbp;

	P = r.at(t);
	vab = b-a;
	vac = c-a;
	vbc = c-b;
	vap = P-a;
	vbp = P-b;

	double area_abp, area_acp, area_bcp, area_abc;

	area_abc = (vab^vac).length();
	area_abp = (vab^vap).length();
	area_acp = (vac^vap).length();
	area_bcp = (vbc^vbp).length();

	double bary_a, bary_b, bary_c;
	bary_b = area_acp/area_abc;
	bary_c = area_abp/area_abc;
	bary_a = area_bcp/area_abc;

	if ( bary_a+bary_b+bary_c >= 1-RAY_EPSILON && bary_a+bary_b+bary_c <= 1+RAY_EPSILON)
	{
		i.t = t;
		i.setMaterial(this->getMaterial());
		i.obj = this;

		Vec2d uv_a, uv_b, uv_c;
		if(parent->verticemaps.size()) {
			uv_a = parent->verticemaps[ids[0]];
			uv_b = parent->verticemaps[ids[1]];
			uv_c = parent->verticemaps[ids[2]];
		}

		uv_a *= bary_a;
		uv_b *= bary_b;
		uv_c *= bary_c;
		i.setUVCoordinates(uv_a+uv_b+uv_c);

		if (parent->normals.size()) {
			const Vec3d& normal_a = parent->normals[ids[0]];
			const Vec3d& normal_b = parent->normals[ids[1]];
			const Vec3d& normal_c = parent->normals[ids[2]];

			if(1-bary_c < RAY_EPSILON) {
				i.N = normal_c;
			}
			else {
				i.N = bary_a*normal_a + bary_b * normal_b + bary_c * normal_c;
				i.N.normalize();
			}
		}
		else {
			i.N = normal;
		}
		return true;
	}
	else
		return false;
}

void Trimesh::generateNormals()
	// Once you've loaded all the verts and faces, we can generate per
	// vertex normals by averaging the normals of the neighboring faces.
{
	int cnt = vertices.size();
	normals.resize( cnt );
	int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
	memset( numFaces, 0, sizeof(int)*cnt );

	for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
	{
		Vec3d faceNormal = (**fi).getNormal();

		for( int i = 0; i < 3; ++i )
		{
			normals[(**fi)[i]] += faceNormal;
			++numFaces[(**fi)[i]];
		}
	}

	for( int i = 0; i < cnt; ++i )
	{
		if( numFaces[i] )
			normals[i].normalize();
		//std::cout<<i<<'\t'<<normals[i]<<std::endl;
	}

	delete [] numFaces;
	vertNorms = true;
}
