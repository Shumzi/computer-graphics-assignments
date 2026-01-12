#include "surf.h"
#include "extra.h"
#include <cmath>
#include <algorithm>
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

CurvePoint rotateCurvePoint(const CurvePoint &c, const  Matrix3f &m)
{
    CurvePoint cp;
    cp.V = m * c.V;
    cp.T = m * c.T;
    cp.N = m * c.N;
    cp.B = m * c.B;
    return cp;
}

Curve getRotatedCurve(const Curve &c, const  Matrix3f &rot)
{
    Curve rotated(c.size());
    transform(c.begin(), c.end(), rotated.begin(), 
        [&rot](CurvePoint c) {return rotateCurvePoint(c, rot); });
    return rotated;
}

vector<Vector3f> getAllVs(const Curve& c)
{
    vector<Vector3f> result(c.size());
    transform(c.begin(), c.end(), result.begin(), [](const CurvePoint& cp) {return cp.V; });
    return result;
}
vector<Vector3f> getAllNs(const Curve& c)
{
    vector<Vector3f> result(c.size());
    // reverse direction because assignment implicitly assumes we're going top to bottom on the left side of the xy plane.
    transform(c.begin(), c.end(), result.begin(), [](const CurvePoint& cp) {return (-1)*cp.N; });
    return result;
}

vector<Tup3u> createFaces(int curve_length, int steps)
/*
* creates faces of a revolved profile by connecting indexes of two curves at a time.
* */
{
    vector<Tup3u> faces;
    faces.reserve(curve_length * 2);
    for (int step = 0; step < steps; ++step)
    {
        int cur_rev_start = step * curve_length;
        int next_rev_start = (step + 1) * curve_length;
        if (step == steps - 1) // loop back to first profile.
            next_rev_start = 0;

        for (int i = 0; i < curve_length - 1; ++i)
        {
            int cur_idx = cur_rev_start + i;
            int next_idx = next_rev_start + i;
            /*
            * cur_idx+1------next_idx+1
            * note that in OpenGL the triangle's normal 
            * is via hand rule of the order of verticies
            * in ccw direction (i.e. right hand).
            * 
            * cur_idx--------next_idx
            * */
            Tup3u bottom_triangle = Tup3u(cur_idx, cur_idx + 1, next_idx + 1);
            Tup3u top_triangle = Tup3u(next_idx + 1, next_idx, cur_idx);
            // i tried this but it gave a triangle pattern bc the direction isn't consistent. needs to be ccw!!
//            Tup3u bottom_triangle = Tup3u(cur_idx + 1, next_idx, next_idx + 1);
//            Tup3u top_triangle = Tup3u(cur_idx, cur_idx + 1, next_idx);
            faces.push_back(bottom_triangle);
            faces.push_back(top_triangle);
        }
    }
    return faces;
}

void addVertAndNormalToSurface(const Curve& curve, Surface& surface)
{
    vector< Vector3f > VV = getAllVs(curve);
    vector< Vector3f > VN = getAllNs(curve);
    surface.VV.insert(surface.VV.end(), VV.begin(), VV.end());
    surface.VN.insert(surface.VN.end(), VN.begin(), VN.end());
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    /*
    * method: we want to 
    * 1. rotate the profile curve around the Y axis in 2pi/<steps> angles each time, 
    * then create faces to connect it all up somehow
    * */
    double angle_step = 2 * M_PI / steps;
    Matrix3f rot = Matrix3f::rotateY(angle_step);
    Curve prev_curve = profile;
    addVertAndNormalToSurface(profile, surface);
    for (int i = 1; i < steps; ++i) // already inserted the original profile.
    {
        Curve curve = getRotatedCurve(prev_curve, rot);
        // connect the dots yay
        addVertAndNormalToSurface(curve, surface);
        prev_curve = curve;
    }
    vector<Tup3u> VF = createFaces(profile.size(), steps);
    surface.VF = VF;
    return surface;
}

CurvePoint getCurvePointAtPlane(const CurvePoint& cp, const CurvePoint& new_base)
{
    Matrix3f tfm = Matrix3f(new_base.N, new_base.B, new_base.T);
    CurvePoint res;
    res.V = tfm * cp.V + new_base.V;
    res.B = tfm * cp.B; // note that the z is only 1 here, in which case it'll be t.
    res.T = tfm * cp.T;
    res.N = tfm * cp.N;
    return res;
}

Curve getCurveAtPlane(const Curve& profile, const CurvePoint& plane)
{
    /* cyl is supposed to be in the coord system of 
    1 0 0
    0 1 0
    0 0 1
    * and we want to move it to 
    * N B? i guess.
    * */
    Curve res(profile.size());
    transform(profile.begin(), 
        profile.end(), 
        res.begin(), 
        [plane](CurvePoint c) {return getCurvePointAtPlane(c, plane); });
    return res;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    /*
    * we need to:
    2. align the profile around the points of the sweep.
    3. connect the points to the previous ones in a triangle fashion like the revolve.
    * */

    for (int i = 0; i < sweep.size(); ++i)
    {
        // move profile to plane defined by the tangent to the vector.
        // alt. defined by the normal and binormal.
        Curve curve = getCurveAtPlane(profile, sweep[i]);
        addVertAndNormalToSurface(curve, surface);
    }
    // loop ends on another point at the same point as the start so we get a closed-surface as a byproduct.
    vector<Tup3u> VF = createFaces(profile.size(), sweep.size()); 
    surface.VF = VF;
    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
