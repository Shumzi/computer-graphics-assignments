#include "Sphere.h"

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    // at^2 + bt + c = 0, see notes in notion.
    // TODO: remove hacky sphere shift when implementing actual transformations.
    Vector3f r0ShiftToSphere = r.getOrigin() - center;
    Vector3f rdShiftToSphere = r.getDirection();
    float a = Vector3f::dot(rdShiftToSphere, rdShiftToSphere);
    float b = 2 * Vector3f::dot(rdShiftToSphere, r0ShiftToSphere);
    float c = Vector3f::dot(r0ShiftToSphere, r0ShiftToSphere) - (radius * radius);
    // check discriminant
    float disc = b * b - 4 * a * c;
    float t;

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
        return true;
    }
    return false;
}

Vector3f Sphere::normalAt(Vector3f p)
{
    Vector3f norm = p - center;
    return norm.normalized();
}