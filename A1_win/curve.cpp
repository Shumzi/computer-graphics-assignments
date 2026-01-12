#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}

Vector3f qBez(float t, Vector3f p_0, Vector3f  p_1, Vector3f  p_2, Vector3f  p_3)
{     
    // remembering that
    // q(t) = (1 - t) ^ 3 * P_0 + 3(1 - t) ^ 2 * tP_1 + 3(1 - t)t ^ 2 * P_2 + t ^ 3 * P_3
    Vector3f p_0_comp = pow(1 - t, 3) * p_0;
    Vector3f p_1_comp = 3 * pow(1 - t, 2) * t * p_1;
    Vector3f p_2_comp = 3 * (1 - t) * pow(t, 2) * p_2;
    Vector3f p_3_comp = pow(t, 3) * p_3;
    return p_0_comp + p_1_comp + p_2_comp + p_3_comp;
}

Vector3f qBezFirstDerivative(float t, Vector3f p_0, Vector3f  p_1, Vector3f  p_2, Vector3f  p_3)
{
    // remember that
    // q'(t) = 3*(1-t)^2(P_1-P_0) + 6(1-t)t(P_2-P_1) + 3t^2(P_3-P_2)
    Vector3f p_01_comp = 3 * pow(1 - t, 2) * (p_1 - p_0);
    Vector3f p_12_comp = 6 * (1 - t) * t * (p_2 - p_1);
    Vector3f p_23_comp = 3 * pow(t, 2) * (p_3 - p_2);
    return p_01_comp + p_12_comp + p_23_comp;
}

Vector3f qBezSecondDerivative(float t, Vector3f p_0, Vector3f  p_1, Vector3f  p_2, Vector3f  p_3)
{
    // q"(t) = 6(1-t)(P_2 - 2P_1 - P_0) + 6t(P_3 - 2P_2 + P_1)
    Vector3f p_012_comp = 6 * (1 - t) * (p_2 - 2 * p_1 - p_0);
    Vector3f p_123_comp = 6 * t * (p_3 - 2 * p_2 - p_1);
    return p_012_comp + p_123_comp;
}

CurvePoint getPointOn2DBezCurve(float t, Vector3f p_0, Vector3f  p_1, Vector3f  p_2, Vector3f  p_3)
{
    CurvePoint cp;
    cp.V = qBez(t, p_0, p_1, p_2, p_3);
    // cp.V = Vector3f::cubicInterpolate(P[p], P[p + 1], P[p + 2], P[p + 3], t);
    // implementing 2.
    cp.T = qBezFirstDerivative(t, p_0, p_1, p_2, p_3).normalized(); // Tangent  (unit)
    cp.B = Vector3f(0, 0, 1); // Binormal (unit)
    cp.N = Vector3f::cross(cp.B, cp.T); // Normal   (unit)
    return cp;
}

Vector3f getInitialB(Vector3f p_0, Vector3f  p_1, Vector3f  p_2, Vector3f  p_3)
{
    Vector3f t = qBezFirstDerivative(0, p_0, p_1, p_2, p_3).normalized(); // Tangent  (unit)
    // graphics trick - find min elem and return that unit vector. 
    // returns the most non-parallel vector out of the three unit-vectors.
    // also retains original 2d bez result, 
    // as in that case z will be smallest, and B=(0,0,1).
    float x = abs(t.x());
    float y = abs(t.y());
    float z = abs(t.z());
    if (x < y && x < z)
        return Vector3f(1, 0, 0);
    else if (y < x && y < z)
        return Vector3f(0, 1, 0);
    return Vector3f(0, 0, 1);
}

CurvePoint getPointOn3dBezCurve(float t, Vector3f p_0, Vector3f  p_1, Vector3f  p_2, Vector3f  p_3, Vector3f b_i_prev)
{
    CurvePoint cp;
    cp.V = qBez(t, p_0, p_1, p_2, p_3);
    // cp.V = Vector3f::cubicInterpolate(P[p], P[p + 1], P[p + 2], P[p + 3], t);
    // implementing 2.
    cp.T = qBezFirstDerivative(t, p_0, p_1, p_2, p_3).normalized(); // Tangent  (unit)
    cp.N = Vector3f::cross(b_i_prev, cp.T).normalized(); // Normal   (unit). 
    cp.B = Vector3f::cross(cp.T,cp.N).normalized(); // Binormal (unit)
    
    return cp;
}

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps: " << steps << endl;

	Curve result;

    /* we need to calc n,b,t,v i.e. the normal, binormal, tangent and vertex location of <steps> points on the curve
    * defined by the control points P of a piecewise polynomial Bezier curve.
    * steps:
    * 1. assume P is on the xy plane (i.e. z is always 0) to get a feel of how to implement this.
    * 2. per step, calculate the value at that location + tangent and normal (assuming binormal is just (0,0,1) since we assume xy plane).
    * 3. figure out how to do this for non xy-plane thingies.
    * remembering that 
    * v = q(t) where q(t) = sum(b[1...4](t)*p[1...4])
    * t = q'(t)
    * */
    Vector3f prev_b = getInitialB(P[0], P[1], P[2], P[3]);
    for (unsigned i = 0; i < (P.size() - 1) / 3; ++i) // loop over piecewise polynomial group. 
    {
        unsigned p = i * 3; // since each bezier piece adds 3 new anchor points (1: 0,1,2,3. 2: 3,4,5,6. etc.)
        for (unsigned j = 0; j < steps; j++) // calc <steps> points on current piece.
        {
            float t = float(j) / float(steps); // relative location that we are on the bez, since t runs from 0 to 1 on a bez.
            // calcing the curvepoint for the current point in piece #i.
            // implementing 1. Multiplication and addition for vector3f is already implemented.
            CurvePoint cp = getPointOn3dBezCurve(t, P[p], P[p + 1], P[p + 2], P[p + 3], prev_b);
            result.push_back(cp);
            prev_b = cp.B;
        }
    }
    // we need to add another point for the last dot...
    CurvePoint lastpoint = getPointOn3dBezCurve(1.0f,
        P[P.size() - 4],
        P[P.size() - 3],
        P[P.size() - 2],
        P[P.size() - 1], prev_b);
    result.push_back(lastpoint);
    cout << "resulting curve has " << result.size() << " points" << endl;

    return result;
}

vector<Vector3f> bspToBez(Vector3f p_0, Vector3f p_1, Vector3f p_2, Vector3f p_3)
{
    /*
    * remembering that we use the following to convert between bases:
    * (G*B_0*B_1^-1)*B_1*T
    * we'll use the mat we found in findBspToBezMat for the B_0*B_1^-1.
    * now all that's left is to multiply it by our cps G to get their bez equivalents.
    */
    vector<Vector3f> res;
    Matrix4f bsp_to_bez_mat = Matrix4f(1, 0, 0, 0, 4, 4, 2, 1, 1, 2, 4, 4, 0, 0, 0, 1);
    bsp_to_bez_mat /= 6;
    // sadly no matrix3x4 exists so we'll use what we've got...
    Matrix4f cps = Matrix4f(Vector4f(p_0, 0), Vector4f(p_1, 0), Vector4f(p_2, 0), Vector4f(p_3, 0));
    Matrix4f bez_cps = cps * bsp_to_bez_mat;
    // now we'll extract the relevant vectors, disregarding the last row of 0's
    res.push_back(bez_cps.getCol(0).xyz());
    res.push_back(bez_cps.getCol(1).xyz());
    res.push_back(bez_cps.getCol(2).xyz());
    res.push_back(bez_cps.getCol(3).xyz());
    return res;
}

void findBspToBezMat()
{
    Matrix4f bez(1, -3, 3, -1, 0, 3, -6, 3, 0, 0, 3, -3, 0, 0, 0, 1);
    cerr << "bez mat:" << endl;
    bez.print();
    Matrix4f bsp(1, -3, 3, -1, 4, 0, -6, 3, 1, 3, 3, -3, 0, 0, 0, 1);
    bsp /= 6;
    cerr << "bsp mat:" << endl;
    bsp.print();
    Matrix4f inv_bez = bez.inverse();
    cerr << "bsp^-1 mat:" << endl;
    inv_bez.print();
    Matrix4f conversion_mat = bsp * inv_bez;
    cerr << "conversion mat:" << endl;
    conversion_mat.print();
    /* result:
    * [ 0.1667 0.0000 0.0000 0.0000 ]
    * [ 0.6667 0.6667 0.3333 0.1667 ]
    * [ 0.1667 0.3333 0.6667 0.6667 ]
    * [ 0.0000 0.0000 0.0000 0.1667 ]
    * bez-bsp (accidentally did the wrong way at first):
    * [ 6.0000 -0.0000 0.0000 0.0000 ]
    * [ -7.0000 2.0000 -1.0000 2.0000 ]
    * [ 2.0000 -1.0000 2.0000 -7.0000 ]
    * [ 0.0000 0.0000 0.0000 6.0000 ]
    */
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.
//    cerr << "\t>>> just printing the transition mat for once:" << endl;
//    findBspToBezMat();
    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }
    Curve result;
    vector<Vector3f> bez_cps;
    // converting to bez
    for (unsigned i = 0; i < P.size() - 3; ++i)
    {
        vector<Vector3f> equivalent_bez_cps = bspToBez(P[i], P[i + 1], P[i + 2], P[i + 3]);
        bez_cps.push_back(equivalent_bez_cps[0]);
        bez_cps.push_back(equivalent_bez_cps[1]);
        bez_cps.push_back(equivalent_bez_cps[2]);
        if (i == P.size() - 4) // on last piece we add the final cp, otherwise the next piece's first point will be the prev last one.
            bez_cps.push_back(equivalent_bez_cps[3]);
    }
    cerr << "calling evalBez w " << bez_cps.size() << " cps" << endl;
    result = evalBezier(bez_cps, steps);

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    return result;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

