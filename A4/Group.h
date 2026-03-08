#ifndef GROUP_H
#define GROUP_H

#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using namespace std;

/// TODO:
/// Implement Group
/// Add data structure to store a list of Object*
class Group : public Object3D
{
public:
  Group()
  {
  }

  Group(int num_objects) : numObjects(num_objects)
  {
    objects = new Object3D *[num_objects];
  }

  ~Group()
  {
    for (int i = 0; i < numObjects; ++i)
    {
      delete objects[i];
    }
    delete objects;
  }

  virtual bool intersect(const Ray &r, Hit &h, float tmin)
  {
    bool modified = false;
    for (int i = 0; i < numObjects; ++i)
      modified |= objects[i]->intersect(r, h, tmin);
    return modified;
  }

  void addObject(int index, Object3D *obj)
  {
    // note this doesn't handle existing object overwrite... will leak oi vei.
    objects[index] = obj;
  }

  int getGroupSize()
  {
    return numObjects;
  }

private:
  Object3D **objects;
  int numObjects;
};

#endif
