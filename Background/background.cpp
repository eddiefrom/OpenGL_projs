#include "GL/glut.h"
#include<math.h>
#include<stdio.h>
#include<time.h>

#define PI 3.14159




GLsizei winSz = 800, winM = 600;
typedef struct point2d { GLfloat x; GLfloat y; }Point;
typedef struct hom2d { GLfloat x; GLfloat y; GLfloat z; }HomPoint;
typedef struct circle { Point origo ; GLfloat rad ; GLint clicked ; Point move; GLfloat angle; Point velocity; GLint quad;}Circle;

Point verticalLine[2], horizontalLine[2], vertNormVect, horiNormVect, downNorm, downNormadd, velocityVect,
		leftLine[2] = {{0,0}, {0,winM}}, 
		rightLine[2] = {{winSz, 0}, {winSz,winM}}, 
		topLine[2] = {{0,winM}, {winSz,winM}},
		downLine[2] = {{0,0}, {winSz, 0}};
Circle circle;
Circle circles[4][20];
GLint keys[256], circleCount[4] = {0};
unsigned int updateTime = 10;



void init();
void drawCircle(GLfloat x, GLfloat y, GLfloat rad);
void drawLine(Point A, Point B);
void drawDisplay();
void drawBack();
GLfloat distanceLinePoint(Point A, Point B, Point C);
void eventMouse(GLint button, GLint state, GLint x, GLint y);
GLint myRand();
void keyUp(unsigned char key, int x, int y);
void keyPress(unsigned char key, int x, int y);
Point setcircleMove(Circle circle);
void update(GLint n);
Point normalVect(Point A, Point B);
Point metszPont(Point A, Point B, Point C, Point D);
GLfloat pointDistance(Point A, Point B);
void drawPolygon(GLdouble radius, Circle O, Point points[]);
GLfloat pointCircleDistance(Point A, Circle B);
HomPoint getHomfromPoint(Point A);
Point getPointfromHom(HomPoint A);
Point mirror(Point A, Point N);






int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(winSz, winM);
	glutCreateWindow("Background");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	init();
	glutDisplayFunc(drawDisplay);
	glutMouseFunc(eventMouse);
	glutTimerFunc(updateTime, update,0);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);
	glutMainLoop();

	return 0;
}



void init()
{
	glClearColor(1, 1, 1, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, winSz, 0, winM);
	glEnable(GL_POINT_SMOOTH);
	glMatrixMode(GL_MODELVIEW);
	
	verticalLine[0].x = winSz/2.0;
	verticalLine[0].y = 0.0 ;
	verticalLine[1].x = winSz/2.0;
	verticalLine[1].y =	winM ;
	horizontalLine[0].x = 0.0;
	horizontalLine[0].y = winM / 2.0;
	horizontalLine[1].x = winSz;
	horizontalLine[1].y = winM / 2.0;
}

void drawLine(Point A, Point B)
{
	glBegin(GL_LINES);
	glVertex2f(A.x, A.y);
	glVertex2f(B.x, B.y);
	glEnd();
}



void drawDisplay()
{
	Point points[100];
	
	glClear(GL_COLOR_BUFFER_BIT);
 	drawBack();
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2);
	drawLine(verticalLine[0], verticalLine[1]);
	drawLine(horizontalLine[0], horizontalLine[1]);
	for(GLint i = 0; i < 4; i++)
	{
		for(GLint j = 0; j < 20; j++)
		{
			if (circles[i][j].clicked)
			{
				drawCircle(circles[i][j].origo.x, circles[i][j].origo.y, circles[i][j].rad);
				drawPolygon(circles[i][j].rad,circles[i][j],points);
				drawLine(circles[i][j].origo,circles[i][j].velocity);
				
				downNorm = normalVect(horizontalLine[0],horizontalLine[1]);
				downNormadd.x = circles[i][j].origo.x + downNorm.x;
				downNormadd.y = circles[i][j].origo.y + downNorm.y;
				drawLine( downNormadd,circles[i][j].origo);
			}
		}
	}

	
	glutSwapBuffers();
}



void drawBack()
{
	glPointSize(4);
	for (GLfloat i = 0; i < winSz; i+= 6)
	{
		for (GLfloat j = 0; j < winM; j+= 6)
		{
			Point origo ;
			origo.x = i;
			origo.y = j;
			GLfloat distanceH = distanceLinePoint(horizontalLine[0],horizontalLine[1],origo);
			GLfloat distanceV = distanceLinePoint(verticalLine[0],verticalLine[1],origo);

			if(distanceH < 0 && distanceV > 0 )
				glColor3f(1.0,0.0,0.0);
			else if(distanceH > 0 && distanceV > 0 )
				glColor3f(0.0,1.0,0.0);
			else if(distanceH > 0 && distanceV < 0 )
				glColor3f(0.0,0.0,1.0);
			else if(distanceH < 0 && distanceV < 0 )
				glColor3f(1.0,1.0,0.0);
			glBegin(GL_POINTS);
				glVertex2f(origo.x,origo.y);
			glEnd();
		}
	}
}



void drawCircle(GLfloat x, GLfloat y, GLfloat rad)
{
	glBegin(GL_LINE_LOOP);
	for (GLdouble t = 0; t < 2 * PI; t += 0.01)
	{
		glVertex2f(x + rad * cos(t), y + rad * sin(t));
	}
	glEnd();
}



GLfloat distanceLinePoint(Point A, Point B, Point Origo)
{
	return ((A.y - B.y)*Origo.x + (B.x - A.x)*Origo.y + (B.y - A.y)*A.x - (B.x - A.x)*A.y) / pointDistance(A,B) ;
}



void eventMouse(GLint button, GLint state, GLint xMouse, GLint yMouse)
{
	Point mouse;
	GLint quad = 0;
	mouse.x = xMouse;
	mouse.y = winM - yMouse;
	
	if(distanceLinePoint(horizontalLine[0],horizontalLine[1],mouse) < 0 
					&& distanceLinePoint(verticalLine[0],verticalLine[1],mouse) > 0)
		quad = 1;
	if(distanceLinePoint(horizontalLine[0],horizontalLine[1],mouse) > 0 
					&& distanceLinePoint(verticalLine[0],verticalLine[1],mouse) > 0)
		quad = 2;
	if(distanceLinePoint(horizontalLine[0],horizontalLine[1],mouse) > 0 
					&& distanceLinePoint(verticalLine[0],verticalLine[1],mouse) < 0)
		quad = 3;
	if(distanceLinePoint(horizontalLine[0],horizontalLine[1],mouse) < 0 
					&& distanceLinePoint(verticalLine[0],verticalLine[1],mouse) < 0)
		quad = 4;
				
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN 
					&& fabs(distanceLinePoint(horizontalLine[0],horizontalLine[1],mouse)) > 30 
					&& fabs(distanceLinePoint(verticalLine[0],verticalLine[1],mouse)) > 30 
					&& fabs(distanceLinePoint(rightLine[0],rightLine[1],mouse)) > 30  
					&& fabs(distanceLinePoint(leftLine[0],leftLine[1],mouse)) > 30 
					&& fabs(distanceLinePoint(topLine[0],topLine[1],mouse)) > 30
					&& fabs(distanceLinePoint(downLine[0],downLine[1],mouse)) > 30)
	{		
		if( circleCount[quad - 1] < 20 )
		{
			circleCount[quad - 1]++;
			circles[quad - 1][circleCount[quad - 1]].clicked = 1;
			circles[quad - 1][circleCount[quad - 1]].origo.x = mouse.x;
			circles[quad - 1][circleCount[quad - 1]].origo.y = mouse.y;
			circles[quad - 1][circleCount[quad - 1]].rad = rand() % 21 + 9;
			circles[quad - 1][circleCount[quad - 1]].move = setcircleMove(circles[quad - 1][circleCount[quad - 1]]);
			circles[quad - 1][circleCount[quad - 1]].quad = quad;
			circles[quad - 1][circleCount[quad - 1]].angle = 0.0;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN 
					&& pointCircleDistance(mouse,circles[quad - 1][circleCount[quad -1]]) < 0 )
	{
		if( circleCount[quad - 1] > 0)
		{
			circles[quad - 1][circleCount[quad - 1]].clicked = 0;
			circleCount[quad - 1]--; 
		}
	}	
}



GLint myRand()
{
	srand(time(NULL));
	return (rand() % 3 - 1);
}



void update(GLint n)
{
	for(GLint i = 0; i < 4; i++)
	{
		for(GLint j = 0; j < 20; j++)
		{
			if(circles[i][j].clicked)
			{
				circles[i][j].origo.x += circles[i][j].move.x;
				circles[i][j].origo.y += circles[i][j].move.y;
				circles[i][j].velocity.x = circles[i][j].origo.x + circles[i][j].move.x * circles[i][j].rad ;
				circles[i][j].velocity.y = circles[i][j].origo.y + circles[i][j].move.y * circles[i][j].rad;
				
				GLfloat distanceDown = distanceLinePoint(downLine[0],downLine[1],circles[i][j].origo);
				GLfloat distanceLeft = distanceLinePoint(leftLine[0],leftLine[1],circles[i][j].origo);
				GLfloat distanceRight = distanceLinePoint(rightLine[0],rightLine[1],circles[i][j].origo);
				GLfloat distanceTop = distanceLinePoint(topLine[0],topLine[1],circles[i][j].origo);
				GLfloat distanceVert= distanceLinePoint(verticalLine[0],verticalLine[1],circles[i][j].origo);
				GLfloat distanceHori = distanceLinePoint(horizontalLine[0],horizontalLine[1],circles[i][j].origo);
				
				if(distanceDown - circles[i][j].rad < 0)
					circles[i][j].move.y *= -1;
				if(distanceLeft + circles[i][j].rad > 0)
					circles[i][j].move.x *= -1;
				if(distanceRight - circles[i][j].rad < 0)
					circles[i][j].move.x *= -1;
				if(distanceTop + circles[i][j].rad > 0)
					circles[i][j].move.y *= -1;
				
				switch(circles[i][j].quad)
				{
					case (1) : {if(distanceHori + circles[i][j].rad > 0) 
								{
									Point newOrigo;
									Point metsz = metszPont(horizontalLine[0],horizontalLine[1],downNormadd,circles[i][j].origo);
									//printf("metsz x: %f metsz y: %f\n",metsz.x,metsz.y);
									newOrigo = mirror(circles[i][j].origo,metsz);
									printf("regiX %f regiY %f ujX %f ujY %f\n",circles[i][j].origo.x,circles[i][j].origo.y,newOrigo.x,newOrigo.y);
									//circle.origo.x = newOrigo.x;
									//circle.origo.y = newOrigo.y * -1;
									circles[i][j].move.y *= -1; 
								}
								
								if(distanceVert - circles[i][j].rad < 0) 
									circles[i][j].move.x *= -1; 
								break;}
					case (2) : {if(distanceHori - circles[i][j].rad < 0) 
									circles[i][j].move.y *= -1; 
								if(distanceVert - circles[i][j].rad < 0) 
									circles[i][j].move.x *= -1; 
								break;}
					case (3) : {if(distanceHori - circles[i][j].rad < 0) 
									circles[i][j].move.y *= -1; 
								if(distanceVert + circles[i][j].rad > 0) 
									circles[i][j].move.x *= -1; 
								break;}
					case (4) : {if(distanceHori + circles[i][j].rad > 0) 
									circles[i][j].move.y *= -1; 
								if(distanceVert + circles[i][j].rad > 0) 
									circles[i][j].move.x *= -1; 
								break;}
								
					default : break;
				}
			}

		}
				
	}


	if(keys['w'])
	{   
		for(GLint i = 0; i < 20; i++)
		{
			if( verticalLine[1].x != winSz )
				//&& fabs(distanceLinePoint(verticalLine[0],verticalLine[1],circles[2][i].origo)+ circles[2][i].rad) > 0 )
			{
				verticalLine[1].x += 5;
				verticalLine[0].x -= 5;
			}
		}		
	}
	if(keys['s'])
	{
		if (verticalLine[1].x != 0)
		{	
			verticalLine[1].x -= 5;
			verticalLine[0].x += 5;
		}
	}
	if (keys['d'])
	{

		if (horizontalLine[1].y != 0)
		{
			horizontalLine[1].y -= 5;
			horizontalLine[0].y += 5;
		}
	}
	if (keys['a'])
	{
		if (horizontalLine[1].y != winM)
		{
			horizontalLine[1].y += 5;
			horizontalLine[0].y -= 5;
		}
	}
	glutTimerFunc(updateTime, update, n);
	glutPostRedisplay();
}
Point setcircleMove(Circle circle)
{
	int random_number = 0;
	circle.move.x = myRand();
	if (circle.move.x == 0)
	{
		while(!random_number)
			random_number = myRand();
		circle.move.y = random_number;
	}
	else
		circle.move.y = myRand();
	
	return circle.move;
}
void keyPress(unsigned char key, int x, int y) {
	keys[key] = 1;
}
void keyUp(unsigned char key, int x, int y) {
	keys[key] = 0;
}
Point normalVect(Point A, Point B)
{
	Point N;
	GLfloat temp;

	N.x = B.x - A.x;
	N.y = B.y - A.y;

	temp = N.x;
	N.x = N.y;
	N.y = temp;
	
	if(N.x == 0)
		N.y *= -1;
	else
		N.x *= -1;

	return N;
}
GLfloat pointDistance(Point A, Point B)
{
	return sqrtf(pow(B.x - A.x,2) + pow(B.y - A.y,2)); 
}
void drawPolygon(GLdouble radius, Circle O, Point points[])
{
	GLint size = -1;
	GLint n = radius / 3;
	glBegin(GL_LINE_LOOP);	
	for (GLint i = 0; i < n; i++, O.angle += 2.0 * PI / n)
	{
		glVertex2f(O.origo.x + radius * cos(O.angle), O.origo.y + radius * sin(O.angle));
		points[i].x = O.origo.x + radius * cos(O.angle);
		points[i].y = O.origo.y + radius * sin(O.angle);
		size++;
	}
	glEnd();
	glBegin(GL_LINES);
	for (GLint i = 0; i <= size; i++)
		for (GLint j = i + 1; j <= size ; j++)
		{
			drawLine(points[i], points[j]);
		}
	glEnd();
	glutPostRedisplay();
}
GLfloat pointCircleDistance(Point A, Circle circle)
{
	return (pow((A.x-circle.origo.x),2) + pow((A.y-circle.origo.y),2) - pow(circle.rad,2));
}
Point metszPont(Point Ap, Point Bp, Point Cp, Point Dp)
{
	HomPoint e,f,M,A,B,C,D;
	Point P;
	
	A = getHomfromPoint(Ap);
	B = getHomfromPoint(Bp);
	C = getHomfromPoint(Cp);
	D = getHomfromPoint(Dp);
	
	e.x = A.y*B.z - A.z*B.y;
	e.y = A.z*B.x - A.x*B.z;
	e.z = A.x*B.y - A.y*B.x;
	
	f.x = C.y*D.z - C.z*D.y;
	f.y = C.z*D.x - C.x*D.z;
	f.z = C.x*D.y - C.y*D.x;

	M.x = e.y*f.z - e.z*f.y;
	M.y = e.z*f.x - e.x*f.z;
	M.z = e.x*f.y - e.y*f.x;
	
	P = getPointfromHom(M);
	
	return P;
	
}
HomPoint getHomfromPoint(Point A)
{
	HomPoint H;
	H.x = A.x;
	H.y = A.y;
	H.z = 1.0;
	
	return  H;
}
Point getPointfromHom(HomPoint A)
{
	Point P;
	P.x = A.x / A.z;
	P.y = A.y / A.z;
	
	return P;
}
Point mirror(Point A, Point N)
{
	Point C;
	Point n;
	n.x = N.x / sqrt(pow(N.x,2) + pow(N.x,2));
	n.y = N.y / sqrt(pow(N.y,2) + pow(N.y,2));
	
	C.x = -A.x + 2 * (A.x*n.x + A.y*n.y) / (n.x+n.y) * n.x;
	C.y = -A.y + 2 * (A.x*n.x + A.y*n.y) / (n.x+n.y) * n.y;
	
	return C;
}


