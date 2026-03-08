## what we've done so far & what's the plan
we can read the args from the input, and the sceneparser will try to generate all the objects for the scene (incl. camera, lights, and Object3Ds)
we started impl. intersection funcs for primitives, still need to:

1. Camera - define how the camera is setup
2. Main - then be able to shoot rays from cam through img
3. then use intersection func to get closest t to ray.
4. draw pixel accordingly, starting with no light, just the default color of each object (i.e. flat scene).
5. save.