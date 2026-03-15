#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
	// generate rays for each screen-space coordinate
	virtual Ray generateRay(const Vector2f &point) = 0;

	virtual float getTMin() const = 0;
	virtual ~Camera() {}

protected:
	Vector3f center;
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;
};

/// TODO: Implement Perspective camera
/// Fill in functions and add more fields if necessary
class PerspectiveCamera : public Camera
{
public:
	/**
	 * @param aspectRatio - e.g. 16/9, 4/3, etc. in a non-square case, the angle is just for x.
	 */
	PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, float angle)
	{
		this->center = center;
		this->direction = direction;
		this->up = up;
		aspectRatio = 1.f;
		w = direction; // i.e. z
		u = Vector3f::cross(w, up); // i.e. x
		v = Vector3f::cross(u, w); // i.e. y
		float d = 1 / tan(angle / 2);
		imgCenter = center + direction * d;
	}

	void setAspectRatio(float aspectRatio)
	{
		this->aspectRatio = aspectRatio;
	}

	/**
	 * @brief create ray from camera origin to point, where -1<=point<=1 in both x & y.
	 *
	 */
	virtual Ray generateRay(const Vector2f &point)
	{
		// ray goes from center to imgcenter + relevant offset.
		Vector3f rd(imgCenter - center + point.y() * v + point.x() * aspectRatio * u);
		rd.normalize();
		return Ray(center, rd);
	}

	virtual float getTMin() const
	{
		return 0.0f;
	}

private:
	Vector3f imgCenter; // dist to image plane. plane is always scaled to -1 to 1 in x & y. distorts.
	// TODO: figure out what's the point of up in this case.
	
	// basis vectors (based on direction & up).
	Vector3f u; // up
	Vector3f v; // right
	Vector3f w; // forward
	float aspectRatio;
};

#endif // CAMERA_H
