#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
  /**
   * for points - M^(-1)*p
   * for normals - M^(-T).
   */
  Transform( const Matrix4f& m, Object3D* obj ):o(obj) 
  {
    rayMat = m.inverse();
    normalMat = rayMat.transposed();
  }
  ~Transform(){
  }
  virtual bool intersect( const Ray& r , Hit& h , float tmin){
    Vector3f newOrigin = (rayMat * Vector4f(r.getOrigin(),1)).xyz();
    Vector3f newDirection = (rayMat * Vector4f(r.getDirection(), 0)).xyz();    
    Ray newR(newOrigin, newDirection);
    // note that normal at point will be unaltered completely bc we only changed the ray, not the geometry.
    bool intersected = o->intersect( newR , h , tmin); 
    if(intersected)
    {
      // fix normals.
      Vector3f newNormal = (normalMat * Vector4f(h.getNormal(), 0)).xyz().normalized();
      h.set(h.getT(), h.getMaterial(), newNormal);
    }
  }

 protected:
  Object3D* o; //un-transformed object	
  Matrix4f rayMat;
  Matrix4f normalMat;
};

#endif //TRANSFORM_H
