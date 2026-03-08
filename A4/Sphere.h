#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
/// TODO:
/// Implement functions and add more fields as necessary
class Sphere : public Object3D
{
public:
	Sphere()
	{
		// unit ball at the center
	}

	Sphere(Vector3f c, float r, Material *material) : Object3D(material)
	{
		center = c;
		radius = r;
	}

	~Sphere() {}

	/**
	 * @brief intersect sphere w ray.
	 * @details we don't assume Rd is unit length, so we'll do the who shenanigan.
	 */
	virtual bool intersect(const Ray &r, Hit &h, float tmin)
	{
		// at^2 + bt + c = 0, see notes in notion.
		float a = Vector3f::dot(r.getDirection(), r.getDirection());
		float b = 2 * Vector3f::dot(r.getDirection(), r.getOrigin());
		float c = Vector3f::dot(r.getOrigin(), r.getOrigin()) - (radius * radius);
		// check discriminant
		float disc = b * b - 4 * a * c;
		float t;
		float curT = h.getT();
		if (disc < 0)
			return false;
		else if (disc == 0)
			t = -b / (2 * a);
		else // two solutions.
		{
			float t1 = (-b + sqrt(disc)) / (2 * a);
			float t2 = (-b - sqrt(disc)) / (2 * a);
			if (t1 < tmin) // later on the t is checked for tmin as well.
				t = t2;
			else if (t2 < tmin)
				t = t1;
			else
				t = min(t1, t2);
		}
		if (t > tmin && h.getT() > t) // then new t is better.
		{
			// need to test this.
			h.set(t, material, normalAt(r.getOrigin() + t * r.getDirection()));
		}
	}

protected:
	Vector3f center;
	float radius;
	/**
	 * @brief get normal vector to given point on sphere.
	 */
	Vector3f normalAt(Vector3f p)
	{
		Vector3f norm = p - center;
		return norm.normalized();
	}
};

#endif
