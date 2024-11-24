#include<stdio.h>
#include<math.h>
#include<GL/glut.h>

GLsizei H = 800, W = 600;
typedef struct vector2d { GLfloat x, y; } Vector;
typedef struct point2d { GLfloat x, y; } Point;

Vector initVector2D(GLfloat x, GLfloat y)   {	Vector V;	V.x = x;	V.y = y; return V;	}
Point initPoint2D(GLfloat x, GLfloat y) 	{	Point P;	P.x = x;	P.y = y; return P;	}
GLint draggedPoint = 1,dragged = -1;

Point P1 = initPoint2D(100,250);
Point P2 = initPoint2D(100,450);
Point P3 = initPoint2D(500,450);
Point P4 = initPoint2D(600,150);
Point points[4] = {P1, P2, P3, P4};
Point Q0, Q1, Q2 = initPoint2D(500,150), Q3 = initPoint2D(150,200), Q4, Q5;
Point Qpoints[6] = { Q0, Q1, Q2, Q3, Q4, Q5 };
Point QpointsDraw[6];
Vector R1, R3;

GLfloat t[3] = {-2, 0.3, 1.5};
GLfloat M[4][4], invers_M[4][4], G[2][4];

void setInversMatrix_M(GLfloat M[4][4], GLfloat inv_M[4][4]);
void setMatrix_M(GLfloat t[3], GLfloat m[4][4]);
GLfloat getMatrixDeterminant_3x3(GLfloat m[3][3]);
GLfloat getMatrixDeterminant_4x4(GLfloat m[4][4]);
void setMinorMatrix(GLfloat m[4][4], GLfloat minor[3][3], GLint row, GLint column);
void setTransparentMatrix(GLfloat m[4][4], GLfloat t[4][4]);
void setMatrixMultiple(GLfloat G[2][4], GLfloat M[4][4], GLfloat T[4], GLfloat value[2]);
void draw();
void drawHerimte(GLfloat G[4][4], GLfloat invM[4][4]);
void drawBezie(Point points[6],GLfloat G[4][4],GLfloat invM[4][4],GLfloat T[4]);
void init();
void drawLine(Point A, Point B);
void drawPoint(Point A);
void mouseClick(GLint button, GLint state, GLint x, GLint y);
void mouseMotion(GLint x, GLint y);
GLfloat getDistPoints(Point A, Point B);
GLint getActPoint(Point P[4], GLint sens, GLint x, GLint y, GLint i,GLint j);


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(H, W);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("Task3");
	init();
	glutDisplayFunc(draw);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);

	glutMainLoop();

	return 0;
}
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0f, H, 0.0f, W, 0.0f, 1.0f);
	glLineWidth(3.0);
	glEnable( GL_POINT_SMOOTH );
	
}
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	GLfloat G[2][4] = { {points[0].x,points[1].x,points[2].x,points[3].x}, 
						{points[0].y,points[1].y,points[2].y,points[3].y} 
					  };
	GLfloat valueMatrix[2] = {0.0}, params[4];	
	GLfloat startPointParams[4] = { 3.0 * pow(t[0],2), 2.0 * t[0], 1.0, 0.0};
	
	setMatrix_M(t,M);	
	
	setInversMatrix_M(M,invers_M);
	
	drawHerimte(G,invers_M);
            
	drawBezie(Qpoints,G,invers_M,startPointParams);
	
	
	glutSwapBuffers();
}
void drawHerimte(GLfloat G[4][4], GLfloat invM[4][4])
{
	GLfloat params[4], valueMatrix[2];
	
	glPointSize(10.0);
	glColor3f(1.0f,0.0f,0.0f);
	for(int i = 0; i < 4 ; i++)
		drawPoint(points[i]);
	drawLine(points[2],points[3]);
	
	glBegin(GL_LINE_STRIP);
	for(GLfloat t = -2.0; t <= 1.5; t += 0.01)
	{	
		params[0] = pow(t,3);
		params[1] = pow(t,2);
		params[2] = t;
		params[3] = 1;
		
		valueMatrix[0] = 0.0;
		valueMatrix[1] = 0.0;
		
		setMatrixMultiple(G,invers_M,params,valueMatrix);
		glVertex2f(valueMatrix[0],valueMatrix[1]);	
	}
	glEnd();
	
}
void drawBezie(Point Qpoints[6],GLfloat G[4][4],GLfloat invers_M[4][4],GLfloat T[4])
{
	GLfloat valueMatrix[2];
	R3 = initVector2D( points[3].x - points[2].x, points[3].y - points[2].y );	
	setMatrixMultiple(G,invers_M,T,valueMatrix);
	R1 = initVector2D( valueMatrix[0], valueMatrix[1]);	

	Qpoints[0] = points[2];
	Qpoints[5] = points[0]; 
	Qpoints[1] = initPoint2D(Qpoints[0].x + R3.x / 5, Qpoints[0].y + R3.y / 5 );
	Qpoints[4] = initPoint2D(Qpoints[5].x - R1.x / 5, Qpoints[5].y - R1.y / 5);
	
	glColor3f(0.0f, 0.0f, 1.0f);	
	glBegin(GL_LINE_STRIP);	
	for(GLfloat t = 0.0; t < 1.0; t += 0.001)
	{
		for(int i = 0; i < 6; i++){
			QpointsDraw[i] = Qpoints[i];
		}
		for(GLint j = 1; j < 6 ; j++)
		{
			for(GLint i = 0; i < 6 - j; i++)
			{	  	
				QpointsDraw[i].x = (QpointsDraw[i].x * (1 - t) + QpointsDraw[i + 1].x * t);
				QpointsDraw[i].y = (QpointsDraw[i].y * (1 - t) + QpointsDraw[i + 1].y * t);
			}	
		}
		glVertex2f(QpointsDraw[0].x, QpointsDraw[0].y);
	}
	glEnd();
	
	glPointSize(5.0);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0xFF00);
	for(int i = 0; i < 6; i++)
	{
		drawPoint(Qpoints[i]);
		if(i < 5)
			drawLine(Qpoints[i],Qpoints[i + 1]);
	}
	glDisable(GL_LINE_STIPPLE);
}
void setMatrix_M(GLfloat t[4], GLfloat M[4][4])
{
	for(GLint i = 0; i < 4; i++)
	{
		for(GLint j = 0; j < 4; j++)
		{
			if(j != 3)
			{
				if(i == 0)
					M[i][j] = t[j] * t[j] * t[j]; 
				if(i == 1)
					M[i][j] = t[j] * t[j];
				if(i == 2)
					M[i][j] = t[j];
				if(i == 3)
					M[i][j] = 1;
			}
			else
			{ 	
				if(i == 0)
					M[i][j] = t[2] * t[2] * 3;
				if(i == 1)
					M[i][j] = t[2] * 2;
				if(i == 2)
					M[i][j] = 1;
				if(i == 3)
					M[i][j] = 0;
			}
		}
	}
}
void setInversMatrix_M(GLfloat M[4][4], GLfloat inv_M[4][4])
{
	GLfloat minor[3][3], adjung[4][4];
	GLfloat det = getMatrixDeterminant_4x4(M);
	GLint multi = -1;
	
	for (GLint i = 0; i < 4; i++)
	{
		multi *= -1;
		for (GLint j = 0; j < 4; j++)
		{
			setMinorMatrix(M,minor,i,j);
			adjung[i][j] = getMatrixDeterminant_3x3(minor) * multi / det;
			multi *= -1;
		}
	}
	setTransparentMatrix(adjung,inv_M);
}
void setMinorMatrix(GLfloat m[4][4], GLfloat minor[3][3], GLint row, GLint column)
{
	GLint i = 0, j = 0, k = 0, l = 0;
	
	while(i < 4)
	{		
		j = 0;
		while(j < 4)
		{
			if(i == row)
				break;
			if(j != column)
			{
				minor[k][l] = m[i][j];
				if(l == 2)
				{
					k++;
					l = 0;					
				}
				else
					l++;
			}
			j++;
		}
		i++;
	}
}
GLfloat getMatrixDeterminant_4x4(GLfloat m[4][4])
{
	GLfloat m1[3][3], m2[3][3], m3[3][3], m4[3][3];
	
	setMinorMatrix(m,m1,0,0);
	setMinorMatrix(m,m2,0,1);
	setMinorMatrix(m,m3,0,2);
	setMinorMatrix(m,m4,0,3);
	
	return ( m[0][0] * getMatrixDeterminant_3x3(m1) - m[0][1] * getMatrixDeterminant_3x3(m2) + m[0][2] * getMatrixDeterminant_3x3(m3) 
				- m[0][3] * getMatrixDeterminant_3x3(m4) );
}
GLfloat getMatrixDeterminant_3x3(GLfloat m[3][3])
{
	return ( m[0][0] * m[1][1] * m[2][2] + m[2][0] * m[0][1] * m[1][2] + m[1][0] * m[2][1] *  m[0][2] 
			- m[2][0] * m[1][1] * m[0][2] - m[2][1] * m[1][2] * m[0][0] - m[1][0] * m[0][1] * m[2][2] );
}

void setTransparentMatrix(GLfloat M[4][4], GLfloat T[4][4])
{
	for (GLint i = 0; i < 4; i++)
	{
		for (GLint j = 0; j < 4; j++)
		{
			T[j][i] = M[i][j];
		}
	}
}
void drawLine(Point A, Point B)
{
	glBegin(GL_LINES);
	glVertex2f(A.x,A.y);
	glVertex2f(B.x,B.y);
	glEnd();
}
void setMatrixMultiple(GLfloat G[2][4], GLfloat M[4][4], GLfloat t[4], GLfloat value[2])
{
	GLfloat sum, temp[2][4], sumX = 0, sumY = 0;
	for(GLint i = 0; i < 2 ; i++)
	{
		for(GLint j = 0; j < 4; j++)
		{
			sum = 0;
			for(GLint k = 0; k < 4; k++ )
			{
				sum += G[i][k] * M[k][j];
			}
			temp[i][j] = sum;
		}
	}
	for(GLint i = 0; i < 2; i++)
	{
		for(GLint j = 0; j < 4; j++)
		{
			if(i == 0)
				sumX += temp[i][j] * t[j];
			else
				sumY += temp[i][j] * t[j];
		}			
	}
	value[0] = sumX;
	value[1] = sumY;
}
void mouseClick(GLint button, GLint state, GLint xMouse, GLint yMouse)
{
	GLint i ;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
		if(( i = getActPoint(points,8,xMouse,W - yMouse,0,4)) != -1)
		{
			dragged = i ;
			draggedPoint = 1;
		}
		
		if(dragged == -1)
			if(( i = getActPoint(Qpoints,8,xMouse,W - yMouse,2,4)) != -1)
			{
				dragged = i ;
				draggedPoint = 2;				
			}	
	}
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		dragged = -1;
	}

}
void mouseMotion(GLint xMouse, GLint yMouse)
{
	if(dragged >= 0)
	{	
		if(draggedPoint == 1)
		{
			points[dragged].x = xMouse;
			points[dragged].y = W - yMouse;		
		}
		else if(draggedPoint == 2)
		{
			Qpoints[dragged].x = xMouse;
			Qpoints[dragged].y = W - yMouse;
		}
	}

	glutPostRedisplay();
}
GLfloat getDistPoints(Point A, Point B)
{
	GLfloat d1 = A.x - B.x;
	GLfloat d2 = B.y - A.y;
	
	return d1 * d1 + d2 * d2;
}
GLint getActPoint(Point points[], GLint sens, GLint x, GLint y,GLint i, GLint j)
{
	Point P = initPoint2D(x,y);
	
	for(GLint k = i; k < j; k++)
		if(getDistPoints(points[k],P) < (sens * sens))
			return k;
	return -1;
}
void drawPoint(Point A)
{
	glBegin(GL_POINTS);
	glVertex2f(A.x, A.y);
	glEnd();
}

