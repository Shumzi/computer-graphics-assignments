#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <vecmath.h>
using namespace std;

#define MAX_BUFFER_SIZE 100

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<Vector3f> vecfv;
vector<Vector3f> vecfn;


// You will need more global variables to implement color and position changes
// Here are some colors you might use - feel free to add more
GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                            {0.9, 0.5, 0.5, 1.0},
                            {0.5, 0.9, 0.3, 1.0},
                            {0.3, 0.8, 0.9, 1.0} };
GLfloat* objColors = diffColors[0];

// Light position
GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
// inline void glVertex(const Vector3f &a) 
// { glVertex3fv(a); }

// inline void glNormal(const Vector3f &a) 
// { glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    static int clr = 1;
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        objColors = diffColors[clr];
        clr = (clr + 1) % 4;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        Lt0pos[1] += 0.5f;
		// cout << "Light go up." << endl;
		break;
    case GLUT_KEY_DOWN:
        Lt0pos[1] -= 0.5f;
		// cout << "Light moving down." << endl;
		break;
    case GLUT_KEY_LEFT:
        Lt0pos[0] -= 0.5f;
		// cout << "Unhandled key press: left arrow." << endl;
		break;
    case GLUT_KEY_RIGHT:
        Lt0pos[0] += 0.5f;
		// cout << "Unhandled key press: right arrow." << endl;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

void drawInput()
{
    for(unsigned int i=0; i < vecfv.size(); i++) { 
        Vector3f &v = vecfv[i]; 
        Vector3f &n = vecfn[i]; 
        glBegin(GL_TRIANGLES); 
        glNormal3d(vecn[n[0]-1][0], vecn[n[0]-1][1], vecn[n[0]-1][2]);
        glVertex3d(vecv[v[0]-1][0], vecv[v[0]-1][1], vecv[v[0]-1][2]);
        glNormal3d(vecn[n[1]-1][0], vecn[n[1]-1][1], vecn[n[1]-1][2]);
        glVertex3d(vecv[v[1]-1][0], vecv[v[1]-1][1], vecv[v[1]-1][2]);
        glNormal3d(vecn[n[2]-1][0], vecn[n[2]-1][1], vecn[n[2]-1][2]);
        glVertex3d(vecv[v[2]-1][0], vecv[v[2]-1][1], vecv[v[2]-1][2]);
        glEnd();
        //do something with v[0], v[1], v[2] 
    }
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objColors);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	// glutSolidTeapot(1.0);
    drawInput();
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	char buffer[MAX_BUFFER_SIZE]; 
    cout << "loading input";
    while(cin.getline(buffer, MAX_BUFFER_SIZE))
    {
        stringstream ss(buffer);
        Vector3f v; 
        string s; 
        ss >> s;
        if(s == "v")
        {
            ss >> v[0] >> v[1] >> v[2];
            vecv.push_back(Vector3f(v[0],v[1],v[2]));
        }
        else if(s == "vn")
        {
            ss >> v[0] >> v[1] >> v[2];
            vecn.push_back(Vector3f(v[0],v[1],v[2]));
        }
        else if(s == "f")
        {
            string triplet;
            Vector3f vv;
            Vector3f vn;

            for(int i=0; i<3; ++i)
            {
                ss >> triplet;
                int start = 0;
                int end = triplet.find('/');
                // get idx of vertex
                vv[i] = stoi(triplet.substr(start,end));
                start = end + 1;

                // skip middle
                end = triplet.find('/', start);
                start = end + 1;

                // get idx of normal
                vn[i] = stoi(triplet.substr(start));
            }

            vecfv.push_back(vv);
            vecfn.push_back(vn);
        }
        
    }
    
     
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
