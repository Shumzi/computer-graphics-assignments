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

struct RenderSettings
{
  SceneParser *sp;
  Image *img;
  Image *imgDepth;
  char outputFilename[MAX_PARSER_TOKEN_LENGTH];
  char depthFilename[MAX_PARSER_TOKEN_LENGTH];
  bool useDepth = false;
};
void parseArgs(int argc, char *argv[], RenderSettings &rs);

void renderImage(Group *g, Vector2f &imgDim, Camera *cam, Image *img, bool useDepth = false);

int main(int argc, char *argv[])
{
  // Fill in your implementation here.

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  RenderSettings rs;
  parseArgs(argc, argv, rs);
  Camera *cam = rs.sp->getCamera();
  // TODO: adjust aspect ratio if needed for PerspectiveCamera.
  Vector2f imgDim(rs.img->Width(), rs.img->Height());
  renderImage(rs.sp->getGroup(), imgDim, cam, rs.img);
  rs.img->SaveImage(rs.outputFilename);
  if (rs.useDepth)
  {
    Vector2f imgDimDepth(rs.imgDepth->Width(), rs.imgDepth->Height());
    renderImage(rs.sp->getGroup(), imgDimDepth, cam, rs.imgDepth, true);
    rs.imgDepth->SaveImage(rs.depthFilename);
  }
  // First, parse the scene using SceneParser.
  // Vector3f pixelColor(1.0f, 0, 0);
  // rs.img->SetPixel(rs.img->Width() / 2, rs.img->Height() / 2, pixelColor);
  // rs.img->SaveImage(rs.outputFilename);
  // if (rs.useDepth)
  // {
  //   Vector3f pixelColorDepth(0, 1.0f, 0);
  //   rs.imgDepth->SetPixel(rs.imgDepth->Width() / 2, rs.imgDepth->Height() / 2, pixelColorDepth);
  //   rs.imgDepth->SaveImage(rs.depthFilename);
  // }

  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene. Write the color at the intersection to that
  // pixel in your output image.

  return 0;
}

void renderImage(Group *g, Vector2f &imgDim, Camera *cam, Image *img, bool useDepth)
{
  // Group *g = rs.sp->getGroup();
  Vector2f imgCtr = imgDim / 2;
  for (int i = 0; i < imgDim.x(); ++i)
  {
    for (int j = 0; j < imgDim.y(); ++j)
    {
      // normalizing from -1 to 1.
      Vector2f point(2 * (i - imgCtr.x()) / imgDim.x(), 2 * (j - imgCtr.y()) / imgDim.y());
      Ray r = cam->generateRay(point);
      Hit h;
      bool intersected = g->intersect(r, h, cam->getTMin());
      if (intersected)
      {
        if (useDepth)
        {
          float depth = clampedDepth(h.getT() * r.getDirection().abs(), 0, 20);
          img->SetPixel(i, j, Vector3f(depth));
        }
        else
          img->SetPixel(i, j, Vector3f(1.f, 0, 0));
      }
      else
      {
        img->SetPixel(i, j, Vector3f(0, 0, 0));
      }
    }
  }
}

float clampedDepth(float depthInput, float depthMin, float depthMax)
{
  if (depthInput < depthMin)
    return depthMin;
  else if (depthInput > depthMax)
    return depthMax;
  return depthInput;
}

void parseArgs(int argc, char *argv[], RenderSettings &rs)
{
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
      rs.sp = new SceneParser(argv[argNum]);
    }
    else if (!strcmp(argv[argNum], "-size"))
    {
      int width = atoi(argv[++argNum]);
      int height = atoi(argv[++argNum]);
      cout << "img width " << width << endl;
      cout << "img height " << height << endl;
      rs.img = new Image(width, height);
    }
    else if (!strcmp(argv[argNum], "-output"))
    {
      strcpy(rs.outputFilename, argv[++argNum]);
      cout << "set output filename to " << rs.outputFilename << endl;
    }
    else if (!strcmp(argv[argNum], "-depth"))
    {
      int width = atoi(argv[++argNum]);
      int height = atoi(argv[++argNum]);
      rs.imgDepth = new Image(width, height);
      strcpy(rs.depthFilename, argv[++argNum]);
      rs.useDepth = true;
      cout << "using depth img with params" << endl;
      cout << "width " << width << endl;
      cout << "height " << height << endl;
      cout << "filename " << rs.depthFilename << endl;
    }
    else
      throw invalid_argument(string("unsupported flag: ") + argv[argNum]);
  }
}