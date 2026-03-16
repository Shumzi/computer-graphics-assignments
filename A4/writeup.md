## what we've done so far & what's the plan
we can read the args from the input, and the sceneparser will try to generate all the objects for the scene (incl. camera, lights, and Object3Ds)
we started impl. intersection funcs for primitives, 
had a bug where 
1. looked like a triangle was missing.
2. the final result was shit

1. first - the missing trig was an error of reading the file in the mesh obj reader.
2. looked at a specific pixel in the cube that should have been good. turns out the problem was with how i recasted the ray to test for shades on the light. namely - that the dir is just the dir, no need to do dir-hitPoint from the illumination, don't know why i'd even do that.
lesson learned - if the depth and normals seem ok, must be something else. so didn't have to waste time looking at the normal calculation etc (even though i found a bug that the triangle normals weren't normalized). also, the ray was suppose to be created only after we found the dir to the light obviously, how did it even work last time??? i used an uninitialized float dir variable?? 
lesson learned - look at warnings!!! or don't compile if uninitialized param is used.