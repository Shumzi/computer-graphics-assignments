#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
/// TODO: Implement Plane representing an infinite plane
/// choose your representation , add more fields and fill in the functions
class Plane : public Object3D
{
public:
	Plane() {}
	Plane(const Vector3f &normal, float d, Material *m) : Object3D(m), normal(normal), d(d)
	{
	}
	~Plane() {}
	virtual bool intersect(const Ray &r, Hit &h, float tmin)
	{
		float numerator = d + Vector3f::dot(r.getOrigin(), normal);
		float denominator = Vector3f::dot(normal, r.getDirection());
		if (abs(denominator) < 1e-5f) // effectively ortho
			return false;
		float t = numerator / denominator;
		if (t < tmin || h.getT() < t)
			return false;
		h.set(t,material, normal);
		return true;
	}

protected:
	Vector3f normal;
	float d;
};
#endif // PLANE_H
