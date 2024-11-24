#include<GL/glut.h>
#include<math.h>
#include<stdio.h>
#define PI 3.1415

GLsizei winSz = 800, winM = 600;
typedef struct point { float x; float y; }Point;
typedef struct circle { Point points; float rad = rand() % 10 + 5; int clicked; int novX, novY; }Circle;
Point edgePoints[4][4] = { {   {0,0}, {0,winM / 2 }, {winSz / 2, winM / 2}, {winSz / 2,0} },
							{   {0,winM / 2} , {0,winM} , {winSz / 2, winM} , {winSz / 2,winM / 2} },
							{   {winSz / 2,winM / 2} , {winSz / 2,winM} , {winSz,winM} , {winSz,winM / 2} },
							{	{winSz / 2,0} , {winSz / 2,winM / 2} , {winSz,winM / 2} , {winSz,0}} };
Circle circles[4][20];
float angle = 0.0;
int keys[256], quads[4] = { -1 };

void init();
void draw();
void drawBack();
void drawCircle();
void timer(int n);
void keyPress(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void getMouse(GLint button, GLint action, GLint xMouse, GLint yMouse);
int myRand();
Point closestPoint(Point A, Point B, Point Origo);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(winSz, winM);
	glutCreateWindow("Background");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	init();
	glutDisplayFunc(draw);
	glutMouseFunc(getMouse);
	glutTimerFunc(5, timer, 0);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);
	glutMainLoop();

	return 0;
}
void drawCircle()
{
	glColor3f(0.0, 0.4, 0.2);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (circles[i][j].clicked)
			{
				glBegin(GL_TRIANGLE_FAN);
				for (double t = 0; t < 2 * PI; t += 0.01)
				{
					glVertex2f(circles[i][j].points.x + circles[i][j].rad * cos(t),
						circles[i][j].points.y + circles[i][j].rad * sin(t));
				}
				glEnd();
			}
		}
	}
}
void drawBack()
{
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0: glColor3f(1.0f, 0.0f, 0.0f);break;
		case 1: glColor3f(0.0f, 1.0f, 0.0f);break;
		case 2: glColor3f(0.0f, 0.0f, 1.0f);break;
		case 3: glColor3f(1.0f, 1.0f, 0.0f);break;
		}
		for (int j = 0; j < 4; j++)
		{
			glVertex2d(edgePoints[i][j].x, edgePoints[i][j].y);
		}
	}
	glEnd();
}
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glPushMatrix();
	glTranslated(winSz / 2, winM / 2, 0);
	glRotated(angle, 0, 0, 1);
	glTranslated(-(winSz / 2), -(winM / 2), 0);
	drawBack();
	drawCircle();
	glPopMatrix();

	glutSwapBuffers();
}
void init()
{
	glClearColor(1, 1, 1, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, winSz, 0, winM);
	glShadeModel(GL_FLAT);
	glMatrixMode(GL_MODELVIEW);
}
void timer(int n)
{
	if (keys['d'])
		angle -= 0.5;

	if (keys['a'])
		angle += 0.5;

	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 20; i++)
		{
			circles[j][i].points.x += circles[j][i].novX;
			circles[j][i].points.y += circles[j][i].novY;

			if ((circles[j][i].points.x + circles[j][i].rad) > winSz || (circles[j][i].points.x - circles[j][i].rad) < 0)
				circles[j][i].novX *= -1;
			if ((circles[j][i].points.y + circles[j][i].rad) > winM || (circles[j][i].points.y - circles[j][i].rad) < 0)
				circles[j][i].novY *= -1;
			if ((circles[j][i].points.x + circles[j][i].rad) > winSz / 2)
				circles[j][i].novX *= -1;
			if ((circles[j][i].points.y + circles[j][i].rad) > winM / 2)
				circles[j][i].novY *= -1;

			//(y0-y1)x+(x1-x0)y+(y1-y0)x0-(x1-x0)y0 = 0		
		}
	}
	glutPostRedisplay();
	glutTimerFunc(5, timer, 0);
}
void keyPress(unsigned char key, int x, int y) {
	keys[key] = 1;
}
void keyUp(unsigned char key, int x, int y) {
	keys[key] = 0;
}
void getMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	/*xMouse = winSz + (xMouse - winSz)*cos(angle) - (yMouse - winM)*sin(angle);
	xMouse = winM + (xMouse - winSz)*sin(angle) + (yMouse - winM)*cos(angle);*/
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
	{

		int i;
		if (xMouse < edgePoints[0][1].y && (winM - yMouse) < edgePoints[0][2].x)	//1	
			i = 0;
		if (xMouse < edgePoints[1][0].y && (winM - yMouse)  > edgePoints[1][2].x)	//2
			i = 1;
		if (xMouse > winM / 2 && (winM - yMouse) > winSz / 2)	//3	
			i = 2;
		if (xMouse > winM / 2 && (winM - yMouse) < winSz / 2)	//4
			i = 3;
		if (quads[i] < 20)
		{
			int rand_value = 0;
			quads[i]++;
			circles[i][quads[i]].clicked = 1;
			circles[i][quads[i]].points.x = xMouse;
			circles[i][quads[i]].points.y = winM - yMouse;

			circles[i][quads[i]].novX = myRand();
			if (circles[i][quads[i]].novX == 0)
			{
				while (rand_value == 0)
				{
					rand_value = myRand();
				}
				circles[i][quads[i]].novY = rand_value;
			}
			else
			{
				circles[i][quads[i]].novY = myRand();
			}
		}
	}
	if (button == GLUT_RIGHT_BUTTON && action == GLUT_DOWN)
	{
		int i;
		if (xMouse < winM / 2 && (winM - yMouse) < winSz / 2)	//1
			i = 0;
		if (xMouse < winM / 2 && (winM - yMouse)  > winSz / 2)	//2
			i = 1;
		if (xMouse > winM / 2 && (winM - yMouse) > winSz / 2)	//3	
			i = 2;
		if (xMouse > winM / 2 && (winM - yMouse) < winSz / 2)	//4
			i = 3;
		if (quads[i] > -1)
		{
			circles[i][quads[i]].clicked = 0;
			quads[i]--;
		}
	}
	glutPostRedisplay();
}
int myRand()
{
	int value = rand() % 3 - 1;

	return value;
}
Point closestPoint(Point A, Point B, Point Origo) {
	Point C;
	float A1 = B.y - A.y;
	float B1 = A.x - B.x;
	float C1 = (B.y - A.y) * A.x + (A.x - B.x) * A.y;
	float C2 = -B1 * Origo.x + A1 * Origo.y;
	float det = A1 * A1 - (-B1 * B1);
	C.x = 0;
	C.y = 0;
	if (det != 0) {
		C.x = (A1 * C1 - B1 * C2) / det;
		C.y = (A1 * C2 - -B1 * C1) / det;
	}
	else {
		C.x = Origo.x;
		C.y = Origo.y;
	}
	return  C;
}