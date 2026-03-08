#include <cassert>
#include <cstdio>
#include <iostream>
#include <cmath>

// Include whatever you need to test, e.g.:
#include "Sphere.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "main_tests.h"

using namespace std;

bool nearlyEqual(float a, float b, float eps = 1e-1f)
{
    return std::abs(a - b) < eps;
}

bool nearlyEqual(Vector3f a, Vector3f b, float eps = 1e-1f)
{
    return (a - b).abs() < eps;
}

int main()
{
    cout << "Running tests..." << endl;
    testSphereIntersection();
    testGroupIntersection();
    testPerspectiveCamera();
    // Add your tests here, e.g.:
    // Ray r(Vector3f(0, 0, -5), Vector3f(0, 0, 1));
    // Hit h;
    // Sphere s(Vector3f(0, 0, 0), 1.0f, nullptr);
    // assert(s.intersect(r, h, 0.0f));

    cout << "All tests passed." << endl;
    return 0;
}

void testEasyIntersection(Sphere s, Vector3f r0)
{
    Vector3f rdEasy(0, 1, 0);
    Ray rEasy(r0, rdEasy);
    Hit hEasy;
    Vector3f correctNormalEasy(0, -2, 0);
    correctNormalEasy.normalize();

    bool closerEasy = s.intersect(rEasy, hEasy, 0);
    assert(closerEasy);
    bool correctNormal = nearlyEqual(hEasy.getNormal(), correctNormalEasy);
    assert(correctNormal);
}

void testHardIntersection(Sphere s, Vector3f r0)
{
    Vector3f endline(1, 0, 0);
    Vector3f rdHard(endline - r0);
    Ray rHard(r0, rdHard);
    Hit hHard;
    Vector3f correctNormalHard(0.73, -1.86, 0);
    correctNormalHard.normalize();

    bool closer = s.intersect(rHard, hHard, 0);
    assert(closer);
    bool gotCorrectNormal = nearlyEqual(hHard.getNormal(), correctNormalHard);
    assert(gotCorrectNormal);
}

void testFailImpossibleIntersection(Sphere s, Vector3f r0)
{
    Vector3f rdImpossible(0, -1, 0);
    Ray rImpossible(r0, rdImpossible);
    Hit hImpossible;
    bool closer = s.intersect(rImpossible, hImpossible, 0);
    assert(!closer);
}

void testSphereIntersection()
{
    cout << "testing Sphere intersection" << endl;
    Sphere s(Vector3f::ZERO, 2.0f, NULL);
    Vector3f r0(0, -7, 0);
    testEasyIntersection(s, r0);
    testHardIntersection(s, r0);
    testFailImpossibleIntersection(s, r0);
}

void testGroupIntersection()
{
    Group g(3);
    Vector3f c = Vector3f::ZERO;
    Sphere *s1 = new Sphere(c, 1.f, NULL);
    Sphere *s2 = new Sphere(c, 2.f, NULL);
    Sphere *s3 = new Sphere(c, 3.f, NULL);
    Vector3f r0(-4, 0, 0);
    Vector3f rd(1, 0, 0);
    Vector3f rdImpossible(-1,0,0);
    Ray r(r0, rd);
    Hit h;

    Ray rImpossible(r0, rdImpossible);
    Hit hImpossible;

    g.addObject(0, s1);
    g.addObject(1, s3);
    g.addObject(2, s2);
    bool intersected = g.intersect(r, h, 0);
    assert(intersected);
    assert(h.getT() == 1.f); // the 3 radius sphere should be there.
    
    bool intersectedImp = g.intersect(rImpossible, hImpossible, 0);
    assert(!intersectedImp);
    
}

void testPerspectiveCamera()
{
    /**
     * test:
     * 1. aspect ratio thing
     * 2. some basic image w some colors.
     */
    return;
}