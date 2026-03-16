#ifndef MESH_H
#define MESH_H
#include <vector>
#include <ostream>
#include "Object3D.h"
#include "Triangle.h"
#include "Vector2f.h"
#include "Vector3f.h"
//by default counterclockwise winding is front face
struct Trig{
  Trig(){x[0]=0;x[1]=0;x[2]=0;}
  int & operator[](const int i) {return x[i];}
  
  int x[3];
  int texID[3];
  friend std::ostream& operator<<(std::ostream& os, const Trig& t) {
    return os << "<" << t.x[0] << " " << t.x[1] << " " << t.x[2] << ">";
  }
};

class Mesh:public Object3D{
public:
  Mesh(const char * filename, Material* m);
  std::vector<Vector3f>v;
  std::vector<Trig>t;
  std::vector<Vector3f>n;
  std::vector<Vector2f>texCoord; 

  virtual bool intersect( const Ray& r , Hit& h , float tmin );
private:
  void compute_norm();
};

#endif
