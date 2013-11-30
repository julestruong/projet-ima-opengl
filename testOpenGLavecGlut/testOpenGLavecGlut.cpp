// testOpenGLavecGlut.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
// testOpenGLwithGLUT.cpp : définit le point d'entrée pour l'application console.
//


int WindowName;


void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);	//Efface le frame buffer et le Z-buffer
	glMatrixMode(GL_MODELVIEW);	//Choisit la matrice MODELVIEW
	glLoadIdentity();	//Réinitialise la matrice
	gluLookAt(0, 0, -10, 0, 0, 0, 0, 1, 0);
	glBegin(GL_TRIANGLES);
	glColor3ub(255, 0, 0);    glVertex2d(-0.75, -0.75);
	glColor3ub(0, 255, 0);    glVertex2d(0, 0.75);
	glColor3ub(0, 0, 255);    glVertex2d(0.75, -0.75);
	glEnd(); // Pour les explications, lire le tutorial sur OGL et win
	glutSwapBuffers();

	//Attention : pas SwapBuffers(DC) !
	glutPostRedisplay();

	//Demande de recalculer la scène
}

void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,float(width) / float(height),0.1,100);	
	glMatrixMode(GL_MODELVIEW);	//Optionnel
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);	//Optionnel
	WindowName = glutCreateWindow("Ma première fenêtre OpenGL !");
	//glutFullScreen();
	glutReshapeFunc(Reshape);

	glutDisplayFunc(Draw);
	
	glutMainLoop();

	system("PAUSE");
	return 0;
}

