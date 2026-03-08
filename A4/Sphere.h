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

	Sphere(Vector3f center, float radius, Material *material) : Object3D(material), center(center), radius(radius) {};

	~Sphere() {}

	/**
	 * @brief intersect sphere w ray. return true if the Hit was updated (i.e. intersected && closer)
	 * @details we don't assume Rd is unit length, so we'll do the whole shenanigan.
	 */
	virtual bool intersect(const Ray &r, Hit &h, float tmin);

protected:
	Vector3f center;
	float radius;
	/**
	 * @brief get normal vector to given point on sphere.
	 */
	Vector3f normalAt(Vector3f p);
};

#endif
