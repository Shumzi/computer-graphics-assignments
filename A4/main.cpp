#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth(float depthInput, float depthMin, float depthMax);
#include "bitmap_image.hpp"
float depthMax = 15.f;
float depthMin = 5.f;
float lightEps = 1e-2f; // dist along object to start bounce light rays (so it doesn't intersect w itself).

enum class ImageType
{
  FULL,
  DEPTH,
  NORMAL
};

struct RenderSetting
{
  Image *image;
  char *filename;
  ImageType imageType;
};

void parseArgs(int argc, char *argv[], SceneParser *&sp, vector<RenderSetting> &rs);

void renderImage(const SceneParser *sp, const RenderSetting &rs);

int main(int argc, char *argv[])
{
  // Fill in your implementation here.

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  SceneParser *sp;
  vector<RenderSetting> rs;
  parseArgs(argc, argv, sp, rs);
  // TODO: adjust aspect ratio if needed for PerspectiveCamera.
  for (int i = 0; i < rs.size(); ++i)
  {
    renderImage(sp, rs.at(i));
    cout << "saving image " << rs.at(i).filename << endl;
    rs.at(i).image->SaveImage(rs.at(i).filename);
  }

  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene. Write the color at the intersection to that
  // pixel in your output image.

  return 0;
}

Vector3f getShading(const Ray &r, const Hit &h, const SceneParser *&sp)
{
  Vector3f lightOnPoint = sp->getAmbientLight();

  /**
   * for each light,
   * check if the light gets to the object,
   * if so light it accordingly. */

  Group *g = sp->getGroup();

  for (int numLight = 0; numLight < sp->getNumLights(); numLight++)
  {
    float distToLight;
    Vector3f dir, col;
    Light *light = sp->getLight(numLight);
    Vector3f hitPoint = r.pointAtParameter(h.getT());
    Hit isShadowed;
    light->getIllumination(hitPoint, dir, col, distToLight);
    Ray shadowRay(hitPoint, dir);
    bool intersected = g->intersect(shadowRay, isShadowed, lightEps);
    /**
     * assume dir is normalized i guess..
     * if the light isn't obstructed
     * (i.e. didn't intersect w other obj or obj was farther away)
     */
    if (!intersected || h.getT() > distToLight)
    {
      lightOnPoint = lightOnPoint + h.getMaterial()->Shade(r, h, dir, col);
    }
  }
  return lightOnPoint;
}

void renderImage(const SceneParser *sp, const RenderSetting &rs)
{
  Group *g = sp->getGroup();
  Image *img = rs.image;
  int width = img->Width();
  int height = img->Height();
  Camera *cam = sp->getCamera();

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      /** normalizing from -1 to 1.
       * note that we start from the tb corner, i.e.
       * (-1,1)--(1,1)
       * (0,h)---(w,h)
       * |          |
       * |          |
       * (0,0)---(w,0)
       * (-1,-1)-(1,-1)
       * so:
       * i=0 -> -1 in xdir img
       * j=0 -> -1 in ydir img
       * */

      Vector2f point(2.f * (i - (width / 2.f)) / width,
                     2.f * (j - (height / 2.f)) / height);
      Ray r = cam->generateRay(point);
      Hit h;
      bool intersected = g->intersect(r, h, cam->getTMin());
      float depth;
      if (intersected && h.getT() < 20)
      {
        switch (rs.imageType)
        {
        case ImageType::FULL:
          img->SetPixel(i, j, getShading(r, h, sp));
          break;
        case ImageType::DEPTH:
          depth = clampedDepth(h.getT() * r.getDirection().abs(), depthMin, depthMax);
          img->SetPixel(i, j, Vector3f(depth));
          break;
        case ImageType::NORMAL:
          img->SetPixel(i, j, h.getNormal());
          break;
        default:
          cout << "no image type, did nothing to img." << endl;
          break;
        }
      }
      else
      {
        img->SetPixel(i, j, sp->getBackgroundColor());
      }
    }
  }
}

/**
 * @brief turns out the closer you are the brighter (that's what the soln did so i did too).
 */
float clampedDepth(float depthInput, float depthMin, float depthMax)
{
  if (depthInput < depthMin)
    return 1;
  else if (depthInput > depthMax)
    return 0;
  return (depthMax - depthInput) / (depthMax - depthMin);
}

void parseArgs(int argc, char *argv[], SceneParser *&sp, vector<RenderSetting> &rs)
{
  int width = 0;
  int height = 0;

  for (int argNum = 1; argNum < argc; ++argNum)
  {
    std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
  }

  for (int argNum = 1; argNum < argc; ++argNum)
  {
    assert(argv[argNum][0] == '-' && "values must be preceeded by a flag arg (-input, -size, etc.)");
    if (!strcmp(argv[argNum], "-input"))
    {
      argNum++;
      cout << "parsing scene for file " << argv[argNum] << endl;
      sp = new SceneParser(argv[argNum]);
    }
    else if (!strcmp(argv[argNum], "-size"))
    {
      width = atoi(argv[++argNum]);
      height = atoi(argv[++argNum]);
      cout << "img width " << width << endl;
      cout << "img height " << height << endl;
    }
    else if (!strcmp(argv[argNum], "-output"))
    {
      Image *img = new Image(width, height);
      rs.push_back(RenderSetting{img, argv[++argNum], ImageType::FULL});
      cout << "set output filename to " << rs.back().filename << endl;
    }
    else if (!strcmp(argv[argNum], "-depth"))
    {
      Image *img = new Image(width, height);
      rs.push_back(RenderSetting{img, argv[++argNum], ImageType::DEPTH});
      cout << "set depth filename to " << rs.back().filename << endl;
    }
    else if (!strcmp(argv[argNum], "-normal"))
    {
      Image *img = new Image(width, height);
      rs.push_back(RenderSetting{img, argv[++argNum], ImageType::NORMAL});
      cout << "set normal filename to " << rs.back().filename << endl;
    }
    else
      throw invalid_argument(string("unsupported flag: ") + argv[argNum]);
  }
}