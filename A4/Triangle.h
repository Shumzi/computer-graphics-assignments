#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
/// TODO: implement this class.
/// Add more fields as necessary,
/// but do not remove hasTex, normals or texCoords
/// they are filled in by other components
class Triangle : public Object3D
{
public:
	Triangle();
	///@param a b c are three vertex positions of the triangle
	Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m) : Object3D(m), a(a), b(b), c(c)
	{
		hasTex = false;
		// just for the heck of it setting default normals, but they'll be edited in actual meshes.
		Vector3f e1 = b - a;
		Vector3f e2 = c - a;
		Vector3f n = Vector3f::cross(e1, e2);
		normals[0] = n;
		normals[1] = n;
		normals[2] = n;
	}

	/**
	 * @brief intersect ray & triangle using Cramer's rule to recover t, beta and gamma, then alpha = 1-beta-gamma.
	 * note: we assume all triangles will be used from within a mesh, otherwise normals stay uninitilized and f up.
	 *
	 */
	virtual bool intersect(const Ray &ray, Hit &hit, float tmin)
	{
		Vector3f r0 = ray.getOrigin();
		Vector3f rd = ray.getDirection();
		Vector3f abCol = a - b;
		Vector3f acCol = a - c;
		Vector3f ar0Col = a - r0;
		Matrix3f betaNumeratorMatrix = Matrix3f(ar0Col, acCol, rd);
		float betaNumerator = betaNumeratorMatrix.determinant();
		Matrix3f gammaNumeratorMatrix = Matrix3f(abCol, ar0Col, rd);
		float gammaNumerator = gammaNumeratorMatrix.determinant();
		Matrix3f tNumeratorMatrix = Matrix3f(abCol, acCol, ar0Col);
		float tNumerator = tNumeratorMatrix.determinant();
		Matrix3f A = Matrix3f(abCol, acCol, rd);
		float aDet = A.determinant();

		float t = tNumerator / aDet;
		float beta = betaNumerator / aDet;
		float gamma = gammaNumerator / aDet;
		float alpha = 1 - beta - gamma;
		if (beta >= 0 && gamma >= 0 && alpha >= 0) // then we're inside the triangle
		{
			if (hit.getT() > t && t > tmin) // then we actually should change the value
			{
				hit.set(t, material, (normals[0] * alpha + normals[1] * beta + normals[2] * gamma).normalized());
				return true;
			}
			return false;
		}
		return false;
	}
	bool hasTex;
	Vector3f normals[3]; // relevant only for meshes, in the regular case its just
	Vector3f a, b, c;
	Vector2f texCoords[3];

protected:
};

#endif // TRIANGLE_H
