#ifndef OPENGL_H
#define OPENGL_H

#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 
#define PI 3.1415

using namespace std;

extern bool GraphicsSetFlag, CalibreSetFlag;
static float Anglex = 0.0, Angley = 0.0, Anglez = 0.0, translate = -500.0;
static float Xvalue = 0.0, Yvalue = 0.0;
static float Anglex_B = 0.0, Angley_B = 0.0, pointer = 1.5, scale = 1.0;
struct point {
	double x, y, z;
};
extern point cursure;
static vector <point> points;

class OpenGL{
public:
	OpenGL(int argc, char *argv[]);
	void DrawPoint(point p);

public:
	static void drawScene();
	static void setup();
	static void resize(int w, int h);
	static void keyInput(unsigned char key, int x, int y);
	static void specialKeyInput(int key, int x, int y);
	static void mouseControl(int button, int state, int x, int y);
	static void mouseMotion(int x, int y);
	static void mouseWheel(int wheel, int direction, int x, int y);
	static void glutTimer(int t);
};

#endif // OPENGL_H
