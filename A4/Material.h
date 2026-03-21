#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
/// TODO:
/// Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
  Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) : diffuseColor(d_color), specularColor(s_color), shininess(s)
  {
  }

  virtual ~Material()
  {
  }

  virtual Vector3f getDiffuseColor() const
  {
    return diffuseColor;
  }

  /**
   * @brief shade object by given light source & diffuse color.
   * TODO: currently implements diffuse & phong shading.
   */
  Vector3f Shade(const Ray &ray, const Hit &hit,
                 const Vector3f &dirToLight, const Vector3f &lightColor)
  {
    Vector3f n = hit.getNormal();
    Vector3f d = ray.getDirection();
    Vector3f diffuseLight;
    Vector3f kd;
    if (t.valid())
      kd = t(hit.texCoord.x(), hit.texCoord.y());
    else // use diffuse clr.
      kd = diffuseColor;
    float diffuseDot = Vector3f::dot(dirToLight, n);
    if (diffuseDot > 0)
      diffuseLight = diffuseDot * lightColor * kd;
    // for phong shading.
    Vector3f reflectedRay = (d - 2 * (Vector3f::dot(d, n) * n)).normalized();
    float phongDot = Vector3f::dot(reflectedRay, dirToLight);
    Vector3f phongLight;
    if (phongDot > 0)
      phongLight = pow(phongDot, shininess) * lightColor * specularColor;
    return diffuseLight + phongLight;
  }

  void loadTexture(const char *filename)
  {
    t.load(filename);
  }

protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;
  Texture t;
};

#endif // MATERIAL_H
