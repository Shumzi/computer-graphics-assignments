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

int main(int argc, char *argv[])
{
  // Fill in your implementation here.

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  RenderSettings rs;
  parseArgs(argc, argv, rs);
  // First, parse the scene using SceneParser.
  Vector3f pixelColor(1.0f, 0, 0);
  rs.img->SetPixel(rs.img->Width() / 2, rs.img->Height() / 2, pixelColor);
  rs.img->SaveImage(rs.outputFilename);
  if (rs.useDepth)
  {
    Vector3f pixelColorDepth(0, 1.0f, 0);
    rs.imgDepth->SetPixel(rs.imgDepth->Width() / 2, rs.imgDepth->Height() / 2, pixelColorDepth);
    rs.imgDepth->SaveImage(rs.depthFilename);
  }

  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene. Write the color at the intersection to that
  // pixel in your output image.

  return 0;
}

void parseArgs(int argc, char *argv[], RenderSettings &rs)
{
  for (int argNum = 1; argNum < argc; ++argNum)
  {
    std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
  }

  for (int argNum = 1; argNum < argc; ++argNum)
  {
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
  }
}