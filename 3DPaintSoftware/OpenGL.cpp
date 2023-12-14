#include "OpenGL.h"

OpenGL::OpenGL(int argc, char *argv[]){
	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(2000, 2000);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("3DPaint.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);
	glutMouseFunc(mouseControl);
	glutMotionFunc(mouseMotion);
	glutMouseWheelFunc(mouseWheel);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();
	GraphicsSetFlag = true;
	glutTimerFunc(2, glutTimer, 2);

	glutMainLoop();
}

void OpenGL::DrawPoint(point p){
	if (points.empty()){
		points.push_back(p);
	}
	else if (abs(points.back().x - p.x) >= 1 || abs(points.back().y - p.y) >= 1 || abs(points.back().z - p.z) >= 1){
		points.push_back(p);
	}
}

void OpenGL::drawScene(){
	double R = 60.0, sum;
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(Xvalue, Yvalue, translate);
	glRotatef(Anglex, 1.0, 0.0, 0.0);
	glRotatef(Angley, 0.0, 1.0, 0.0);
	glRotatef(Anglez, 0.0, 0.0, 1.0);

	glLineWidth(2.5);

	glBegin(GL_LINE_STRIP);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0);
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(200, 0, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 0);
	glColor3f(0.0, 1.0, 1.0);
	glVertex3f(0, 200, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, 0);
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(0, 0, 200);
	glEnd();

	//glColor3f(0.0, 0.0, 1.0);
	glLineWidth(pointer);

	glBegin(GL_LINE_STRIP);
	glVertex3f(cursure.x, cursure.y, cursure.z);
	glVertex3f(cursure.x + 1, cursure.y + 1, cursure.z + 1);
	glEnd();

	glBegin(GL_LINE_STRIP);

	for (int i = 0; i < points.size(); i++){
		sum = points[i].x + points[i].y + points[i].z;
		glColor3f(points[i].x / sum, points[i].y / sum, points[i].z / sum);
		if (points[i].x == -1 && points[i].y == -1 && points[i].z == -1){
			glEnd();
			glBegin(GL_LINE_STRIP);
		}
		else{
			glVertex3f(points[i].x*scale, points[i].y*scale, points[i].z*scale);
		}
	}


	/*for (double t = -10 * PI; t <= 10 * PI; t += 0.1){
		glVertex3f(R*cos(t), R*sin(t), -t * 20.0 - 150.0);
		}*/

	glEnd();

	glutSwapBuffers();
}

void OpenGL::setup(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void OpenGL::resize(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-100.0, 100.0, -100.0, 100.0, 200.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
}

void OpenGL::keyInput(unsigned char key, int x, int y){
	switch (key)
	{
	case '[':
		translate += 10;
		glutPostRedisplay();
		break;
	case ']':
		translate -= 10;
		glutPostRedisplay();
		break;
	case 'r':
		Anglex = Angley = Anglez = 0.0;
		glutPostRedisplay();
		break;
	case 'e':
		points.clear();
		break;
	case 'z':
		Anglex += 3.0;
		glutPostRedisplay();
		break;
	case 'x':
		Angley += 3.0;
		glutPostRedisplay();
		break;
	case 'c':
		Anglez += 3.0;
		glutPostRedisplay();
		break;
	case 'a':
		Anglex -= 3.0;
		glutPostRedisplay();
		break;
	case 's':
		Angley -= 3.0;
		glutPostRedisplay();
		break;
	case 'd':
		Anglez -= 3.0;
		glutPostRedisplay();
		break;
	case 'p':
		pointer += 0.1;
		glutPostRedisplay();
		break;
	case 'o':
		pointer -= 0.1;
		glutPostRedisplay();
		break;
	case 'l':
		scale += 0.1;
		glutPostRedisplay();
		break;
	case 'k':
		scale -= 0.1;
		glutPostRedisplay();
		break;
	case 'q':
		CalibreSetFlag = true;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	if (Anglex > 720){
		Anglex -= 720;
	}
	if (Angley > 720){
		Angley -= 720;
	}
	if (Anglez > 720){
		Anglez -= 720;
	}
}

void OpenGL::specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) Yvalue += 10.0;
	if (key == GLUT_KEY_DOWN) Yvalue -= 10.0;
	if (key == GLUT_KEY_LEFT) Xvalue -= 10.0;
	if (key == GLUT_KEY_RIGHT) Xvalue += 10.0;
	glutPostRedisplay();
}

void OpenGL::mouseControl(int button, int state, int x, int y)
{
	// Store the clicked point in the currentPoint variable when left button is pressed.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		Anglex_B = x;
		Angley_B = y;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		Anglex_B = x;
		Angley_B = y;
		points.clear();
	}
	glutPostRedisplay();
}

void OpenGL::mouseMotion(int x, int y)
{
	// Update the location of the current point as the mouse moves with button pressed.
	Anglex += x - Anglex_B;
	Angley += y - Angley_B;
	Anglex_B = x;
	Angley_B = y;
	glutPostRedisplay();
}

void OpenGL::mouseWheel(int wheel, int direction, int x, int y)
{
	// Increment/decrement the global pointSize depending on the direction 
	// of rotation of the mouse wheel.
	(direction > 0) ? translate += 10.0 : translate -= 10.0;

	glutPostRedisplay();
}

void OpenGL::glutTimer(int t){
	glutPostRedisplay();
	glutTimerFunc(t, glutTimer, t);
}