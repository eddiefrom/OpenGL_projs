#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#define PI 3.1415

GLsizei SZ = 1000, M = 600;
typedef struct point3d {GLfloat x, y, z;} POINT3D;
typedef struct hom3d {GLfloat x, y, z, u;} HOM3D;
typedef struct area {GLint a, b, c, d, visible, color;} AREA;

void init();
void draw();
HOM3D pointToHom3D(POINT3D P);
POINT3D homToPoint3D(HOM3D H);
POINT3D initPOINT3D(GLfloat x, GLfloat y, GLfloat z);
void drawPoint(HOM3D P);
void drawLine(HOM3D H1, HOM3D H2);
void drawQuad(HOM3D H1,HOM3D H2,HOM3D H3, HOM3D H4);
void mulMatrix(GLfloat A[4][4], GLfloat B[4][4], GLfloat C[4][4]);
POINT3D getVectorSub(POINT3D A, POINT3D B);
GLfloat getScalarMult(POINT3D A, POINT3D B);
POINT3D getVectorMult(POINT3D A, POINT3D B);
void keyUp(unsigned char key, int x, int y);
void keyPress(unsigned char key, int x, int y);
void keyOperations();

GLfloat S = 300.0f, transX = 500.0f, transY = 300.0f, lamda = 200.0f, angleX = 10.0f, angleY = 30.0f;
GLint keys[256];	

POINT3D C0 = initPOINT3D(0.5,0.5,-0.5),   C1 = initPOINT3D(-0.5,0.5,-0.5),
		C2 = initPOINT3D(-0.5,-0.5,-0.5), C3 = initPOINT3D(0.5,-0.5,-0.5),
		C4 = initPOINT3D(0.5,0.5,0.5),    C5 = initPOINT3D(-0.5,0.5,0.5),
		C6 = initPOINT3D(-0.5,-0.5,0.5),  C7 = initPOINT3D(0.5,-0.5,0.5);
		
AREA cube[6] = {
				{0, 4, 7, 3, 0, 0}, {4, 5, 6, 7, 0, 1},
				{1, 5, 4, 0, 0, 2}, {0, 3, 2, 1, 0, 3},
				{7, 6, 2, 3, 0, 4}, {6, 5, 1, 2, 0, 5}
			};

HOM3D edgeHOM3D[8] = { pointToHom3D(C0), pointToHom3D(C1), pointToHom3D(C2), pointToHom3D(C3) , 
					   pointToHom3D(C4), pointToHom3D(C5), pointToHom3D(C6), pointToHom3D(C7) },
	  newEdge[8], newEdge2[8];

	  
int main (int argc, char** argv) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB);
    glutInitWindowPosition(200,50);
    glutInitWindowSize(SZ, M);
    glutCreateWindow("Cube");
    init();
    glutDisplayFunc(draw);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);

    glutMainLoop();
    return 0;
}
void init( ) 
{
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glMatrixMode (GL_PROJECTION);
    glOrtho(0.0f, SZ, 0.0f, M, 0.0f, 1.0f);
	glLineWidth(5.0);
}

void draw( ) 
{ 
	keyOperations();

    glClear(GL_COLOR_BUFFER_BIT);
	
	POINT3D normal, aVect, bVect, viewPoint;
	GLfloat tmp1[4][4], tmp2[4][4], tmp3[4][4], M[4][4];
	GLfloat scalar = 0.0;
	POINT3D Centrum = initPOINT3D(0.0f,0.0f,S);
	
	GLfloat scale[4][4] = { 
		{ lamda, 0.0, 0.0, 0.0 },
		{ 0.0, lamda, 0.0, 0.0 },
		{ 0.0, 0.0, lamda, 0.0},
		{ 0.0, 0.0, 0.0, 1.0 }    
	};	
    GLfloat Ry[4][4] = {
        {cos(angleY), 0.0, sin(angleY), 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {-sin(angleY), 0.0, cos(angleY), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
    GLfloat Rx[4][4] = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, cos(angleX), -sin(angleX), 0.0},
        {0.0, sin(angleX), cos(angleX), 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
    GLfloat Vc[4][4] = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, (-1.0 / S), 1.0}
    };
	GLfloat T[4][4] = {
		{1.0, 0.0, 0.0, transX},
		{0.0, 1.0, 0.0, transY},
		{0.0, 0.0, 1.0, 0.0},
		{0.0, 0.0, 0.0, 1.0}
	};

	mulMatrix(Ry,scale,tmp1);
	mulMatrix(Rx,tmp1,tmp2);
	
	for(GLint i = 0; i < 8; i++)
	{
		newEdge[i].x = tmp2[0][0] * edgeHOM3D[i].x + tmp2[0][1] * edgeHOM3D[i].y + tmp2[0][2] * edgeHOM3D[i].z + tmp2[0][3] * edgeHOM3D[i].u;
		newEdge[i].y = tmp2[1][0] * edgeHOM3D[i].x + tmp2[1][1] * edgeHOM3D[i].y + tmp2[1][2] * edgeHOM3D[i].z + tmp2[1][3] * edgeHOM3D[i].u;
		newEdge[i].z = tmp2[2][0] * edgeHOM3D[i].x + tmp2[2][1] * edgeHOM3D[i].y + tmp2[2][2] * edgeHOM3D[i].z + tmp2[2][3] * edgeHOM3D[i].u;
		newEdge[i].u = tmp2[3][0] * edgeHOM3D[i].x + tmp2[3][1] * edgeHOM3D[i].y + tmp2[3][2] * edgeHOM3D[i].z + tmp2[3][3] * edgeHOM3D[i].u;
	}
	for(GLint i = 0; i < 6; i++)
	{
		aVect = getVectorSub(homToPoint3D(newEdge[cube[i].a]), homToPoint3D(newEdge[cube[i].b]));
		bVect = getVectorSub(homToPoint3D(newEdge[cube[i].a]), homToPoint3D(newEdge[cube[i].c]));
	
		normal = getVectorMult(aVect, bVect);
		viewPoint  = getVectorSub(homToPoint3D(newEdge[cube[i].a]),Centrum);

		scalar = getScalarMult(normal,viewPoint);
		//printf("s: %f\n",Centrum.z);
		
		if( scalar > 0)
		{
			cube[i].visible = 1;
		}
		else
		{
			cube[i].visible = 0;
		}	
	}	

	mulMatrix(T,Vc,M);
	
	glColor3f(1.0,0.0,0.0);
	for(GLint i = 0; i < 8; i++)
	{
		newEdge2[i].x = M[0][0] * newEdge[i].x + M[0][1] * newEdge[i].y + M[0][2] * newEdge[i].z + M[0][3] * newEdge[i].u;
		newEdge2[i].y = M[1][0] * newEdge[i].x + M[1][1] * newEdge[i].y + M[1][2] * newEdge[i].z + M[1][3] * newEdge[i].u;
		newEdge2[i].z = M[2][0] * newEdge[i].x + M[2][1] * newEdge[i].y + M[2][2] * newEdge[i].z + M[2][3] * newEdge[i].u;
		newEdge2[i].u = M[3][0] * newEdge[i].x + M[3][1] * newEdge[i].y + M[3][2] * newEdge[i].z + M[3][3] * newEdge[i].u;
	}
	
	for(GLint  i = 0; i < 6; i++)
	{
		switch(cube[i].color)
		{
			case 0 : glColor3f(1.0,0.0,0.0);break;
			case 1 : glColor3f(0.0,1.0,0.0);break;
			case 2 : glColor3f(0.0,0.0,1.0);break;
			case 3 : glColor3f(1.0,1.0,0.0);break;
			case 4 : glColor3f(1.0,0.0,1.0);break;
			case 5 : glColor3f(0.0,1.0,1.0);break;	
			default: break;
		}
		if(cube[i].visible )
		{
			drawQuad(newEdge2[cube[i].a],newEdge2[cube[i].b],newEdge2[cube[i].c],newEdge2[cube[i].d]);
		
			glColor3f(0.0,0.0,0.0);
			drawLine(newEdge2[cube[i].a],newEdge2[cube[i].b]);
			drawLine(newEdge2[cube[i].b],newEdge2[cube[i].c]);
			drawLine(newEdge2[cube[i].c],newEdge2[cube[i].d]);
			drawLine(newEdge2[cube[i].d],newEdge2[cube[i].a]);
		}
	}
	
	glutSwapBuffers();

}

HOM3D pointToHom3D(POINT3D P)
{
	HOM3D H;
	H.x = P.x;
	H.y = P.y;
	H.z = P.z;
	H.u = 1.0;
	return H;
}
POINT3D homToPoint3D(HOM3D H)
{
	POINT3D P;
	P.x = H.x / H.u;
	P.y = H.y / H.u;
	P.z = H.z / H.u;
	return P;
}
POINT3D initPOINT3D(GLfloat x, GLfloat y, GLfloat z)
{
	POINT3D P;
	P.x = x;
	P.y = y;
	P.z = z;
	return P;
}
void drawPoint(HOM3D H)
{
	POINT3D P = homToPoint3D(H);
	glBegin(GL_POINTS);
		glVertex3f(P.x,P.y,P.z);
	glEnd();
}
void drawLine(HOM3D H1, HOM3D H2)
{
	POINT3D P1 = homToPoint3D(H1);
	POINT3D P2 = homToPoint3D(H2);
	glBegin(GL_LINES);
		glVertex3f(P1.x, P1.y, P1.z);
		glVertex3f(P2.x, P2.y, P2.z);
	glEnd();
}
void mulMatrix( GLfloat A[][4], GLfloat B[][4], GLfloat C[][4])
{  
	for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            float sum = 0;
            for(int k = 0; k < 4; k++){
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
POINT3D getVectorSub(POINT3D A, POINT3D B)
{
	POINT3D P;
	P.x = B.x - A.x;
	P.y = B.y - A.y;
	P.z = B.z - A.z;	
	return P;
}
GLfloat getScalarMult(POINT3D A, POINT3D B)
{
	return ( A.x*B.x + A.y*B.y + A.z*B.z );
}
POINT3D getVectorMult(POINT3D A, POINT3D B)
{
	POINT3D C;
	C.x = A.y*B.z - A.z*B.y;
	C.y = A.z*B.x - A.x*B.z;
	C.z = A.x*B.y - A.y*B.x;
	return C;
}
void drawQuad(HOM3D H1,HOM3D H2,HOM3D H3, HOM3D H4)
{
	POINT3D P1,P2,P3,P4;
	P1 = homToPoint3D(H1);
	P2 = homToPoint3D(H2);
	P3 = homToPoint3D(H3);
	P4 = homToPoint3D(H4);
	
	glBegin(GL_QUADS);	
		glVertex3f(P1.x,P1.y,P1.z);
		glVertex3f(P2.x,P2.y,P2.z);
		glVertex3f(P3.x,P3.y,P3.z);
		glVertex3f(P4.x,P4.y,P4.z);
	glEnd();
}
void keyPress(unsigned char key, int x, int y) {
	keys[key] = 1;
}
void keyUp(unsigned char key, int x, int y) {
	keys[key] = 0;
}
void keyOperations() {
    if (keys['a']) 
	{
		if(angleY  != 180.0)
			angleY += 0.001;
		else
			angleY = 0.0;

	}
    if (keys['d']) 
	{
		if(angleY  != 180.0)
			angleY -= 0.001;
		else
			angleY = 0.0;
	}
	if (keys['w']) 
	{
		if(angleX  != 180.0)
			angleX -= 0.001;
		else
			angleX = 0.0;
	}
	
    if (keys['s']) 
	{
		if(angleX != 180.0)
			angleX += 0.001;
		else
			angleX = 0.0;

	}
    if (keys['+']) 
	{
		S += 0.1;
	}
    if (keys['-']) 
	{
		S -= 0.1;
	}
		
	 glutPostRedisplay();
}

