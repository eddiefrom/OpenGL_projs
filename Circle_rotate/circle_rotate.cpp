#include<GL/glut.h>
#include<math.h>
#define PI 3.14159265359

GLsizei SZ = 700.0, M = 500.0;
typedef struct point { GLfloat x; GLfloat y; }Point;
GLdouble angle, rad = 150.0;
GLint polygon_edges = 3;
Point Origo, point_A, point_B;
GLint irany = 1, start_click = 0;
unsigned int time = 12;

void init();
void drawCircle(Point O, GLdouble rad);
void drawLine(Point A, Point B);
void drawPolygon(GLint n, GLdouble radius, Point O, Point points[]);
void draw();
void update(GLint n);
void mouseClick(GLint button, GLint state, GLint x, GLint y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(SZ, M);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("Circle_rotate");
	init();
	glutDisplayFunc(draw);
	glutMouseFunc(mouseClick);
	glutTimerFunc(time, update, 0);
	glutMainLoop();

	return 0;
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SZ, 0, M);
	glMatrixMode(GL_MODELVIEW);
	Origo.x = rad; 
	Origo.y = M / 2.0 ; 
	point_A.x = 0.0; 
	point_A.y = M / 2.0 - rad ; 
	point_B.x =  SZ; 
	point_B.y = M / 2.0 - rad ;
}
void drawCircle(Point O, GLdouble rad)
{
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	for (GLdouble i = 0; i <= 2 * PI; i += 0.01)
		glVertex2f(O.x + rad * cos(i), O.y + rad * sin(i));
	glEnd();
}
void drawLine(Point A, Point B)
{
	glBegin(GL_LINES);
	glVertex2f(A.x, A.y);
	glVertex2f(B.x, B.y);
	glEnd();
}
void drawPolygon(GLint n, GLdouble radius, Point O, Point points[])
{
	GLint size = -1;
	glBegin(GL_LINE_LOOP);	
	for (GLint i = 0; i < n; i++, angle += 2.0 * PI / n)
	{
		glVertex2f(O.x + radius * cos(angle), O.y + radius * sin(angle));
		size++;
		points[i].x = O.x + radius * cos(angle);
		points[i].y = O.y + radius * sin(angle);
	}
	glEnd();
	glBegin(GL_LINES);
	for (GLint i = 0; i <= size; i++)
		for (GLint j = 1; j <= size - 1; j++)
		{
			drawLine(points[i], points[j]);
		}
	glEnd();
	glutPostRedisplay();
}
void draw()
{
	Point points[100];

	glClear(GL_COLOR_BUFFER_BIT);
	drawCircle(Origo, rad);
	drawPolygon(polygon_edges, rad, Origo, points);
	drawLine(point_A, point_B);
	glutSwapBuffers();
}
void update(GLint n)
{
	if (start_click)
	{
		if (angle == 180)
			angle = 0.0;
		if (irany)
		{
			angle -= 0.01;
			Origo.x++;
		}
		else
		{
			angle += 0.01;
			Origo.x--;
		}
		if (Origo.x + rad == SZ)
			irany = 0;
		if (Origo.x - rad == 0)
			irany = 1;
	}
	glutPostRedisplay();
	glutTimerFunc(time, update, 0);

}
void mouseClick(GLint button, GLint state, GLint x, GLint y)
{
	switch (button) {
		case GLUT_LEFT_BUTTON :	    start_click = 1; break;
		case GLUT_RIGHT_BUTTON :	if(state == GLUT_DOWN) polygon_edges++; break;
		case GLUT_MIDDLE_BUTTON:    if (state == GLUT_DOWN && polygon_edges > 3) polygon_edges--; break;
	}
}