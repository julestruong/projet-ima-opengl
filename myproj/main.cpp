#include "stdafx.h"
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

#include "shaders.h"

#include "point3d.h"
#include "vector3d.h"
#include "util.h"
#include "object3d.h"
#include "light.h"

#define POINT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
#define SPOT_LIGHT 3

#define TAILLE_ORBIT 0.03

#define SOLEIL 0
#define MERCURE 1
#define VENUS 2
#define TERRE 3
#define MARS 4
#define JUPITER 5
#define SATURNE 6
#define URANUS 7
#define NEPTUNE 8
#define LUNE 9
#define LUNE2 10
#define LUNE3 11

#define RAYON 0
#define DISTANCE 1
#define VITESSE 2

int indice;

float planetInfo[20][3];


// width and height of the window.
int Glut_w = 600, Glut_h = 400; 

GLUquadric *orbite;
GLUquadric *sphereSoleil;
GLUquadric *sphereTerre;
GLUquadric *sphereLune;
GLUquadric *sphereJupiter;
GLUquadric *sphereEuropa;
GLUquadric *sphereCallisto;

int deltaT = 0;
int clockSave = 0;

float rotations[11], incr[11];

std::vector<light*> lightsVector;
int indiceLight = 0;

//Variables and their values for the camera setup.
myPoint3D camera_eye(0,0,30);
myVector3D camera_up(0,1,0);
myVector3D camera_forward (0,0,-1);

float time = 0;
GLuint mytime_loc;
GLuint shader_light_position;
GLuint shader_light_color;
GLuint shader_light_direction;
GLuint shader_light_type;
GLuint shader_silhouette;
GLuint shader_texture;
GLuint shader_bump;

bool silhouette = false;


GLuint vertexshader, fragmentshader, shaderprogram ; // shaders

float fovy = 90;
float zNear = 0.1;
float zFar = 120;//60;

int button_pressed = 0; // 1 if a button is currently being pressed.
int GLUTmouse[2] = { 0, 0 };

myObject3D *myobj1;
myObject3D *myobj2;

myObject3D* walls[4];
myObject3D* planet[20];
myObject3D *sol;
myObject3D *plafond;

//This function is called when a mouse button is pressed.
void mouse(int button, int state, int x, int y)
{
  // Remember button state 
  button_pressed = (state == GLUT_DOWN) ? 1 : 0;

   // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = Glut_h - y;
}

//This function is called when the mouse is dragged.
void mousedrag(int x, int y)
{
  // Invert y coordinate
  y = Glut_h - y;

  //change in the mouse position since last time
  int dx = x - GLUTmouse[0];
  int dy = y - GLUTmouse[1];

  GLUTmouse[0] = x;
  GLUTmouse[1] = y;

  if (dx == 0 && dy == 0) return;
  if (button_pressed == 0) return;

  /**ADD CODE TO ROTATE THE CAMERA AROUND THE CURRENT POSITION**/
  double vx = (double) dx / (double) Glut_w;
  double vy = (double) dy / (double) Glut_h;
  double theta = 4.0 * (fabs(vx) + fabs(vy));

  myVector3D camera_right;
  crossproduct(camera_forward, camera_up, camera_right);
  camera_right.normalize();

  //myVector3D tomove_direction( camera_right.dX*vx + camera_up.dX*vy,       camera_right.dY*vx + camera_up.dY*vy, camera_right.dZ*vx + camera_up.dZ*vy );
  myVector3D tomovein_direction = camera_right*vx + camera_up*vy;

  myVector3D rotation_axis;
  crossproduct(tomovein_direction, camera_forward, rotation_axis);
  rotation_axis.normalize();
  
  rotate2(camera_forward, rotation_axis, theta);
  rotate2(camera_up, rotation_axis, theta);
  rotate2(camera_eye, rotation_axis, theta);
 	  
  camera_up.normalize();
  camera_forward.normalize();

  glutPostRedisplay();
}

//This function is called when a key is pressed.
void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 27:  // Escape to quit
		exit(0) ;
        break ;
	case 'l':  
		indiceLight++;
        break ;
	case 's':
		silhouette = !silhouette;
		glUniform1i(shader_silhouette, silhouette);
		break;
	}
	glutPostRedisplay();
}

//This function is called when an arrow key is pressed.
void keyboard2(int key, int x, int y) {
	float speed = 1.0;
	switch(key) {
	case GLUT_KEY_UP:
		/**ADD CODE TO MOVE FORWARD**/
		camera_eye.X += speed*camera_forward.dX; camera_eye.Y += speed*camera_forward.dY;camera_eye.Z += speed*camera_forward.dZ;
		//camera_eye += camera_forward;
		break;
	case GLUT_KEY_DOWN:
		/**ADD CODE TO MOVE BACK**/
		camera_eye.X -= speed*camera_forward.dX; camera_eye.Y -= speed*camera_forward.dY;camera_eye.Z -= speed*camera_forward.dZ;
		//camera_eye += -camera_forward;
		break;
	case GLUT_KEY_LEFT:
		/**ADD CODE TO TURN LEFT**/
		camera_up.normalize();
		rotate2(camera_forward, camera_up, 0.1);
		camera_forward.normalize();
		break;
	case GLUT_KEY_RIGHT:
		/**ADD CODE TO TURN RIGHT**/
		camera_up.normalize();
		rotate2(camera_forward, camera_up, -0.1);
		camera_forward.normalize();
		break;
	}
	glutPostRedisplay();
}

void reshape(int width, int height){
	Glut_w = width;
	Glut_h = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, Glut_w/(float)Glut_h, zNear, zFar);
	glViewport(0, 0, Glut_w, Glut_h);
}

void setLight()
{
	float couleur[4] = { 1.0, 1.0, 1.0, 1.0 };
	lightsVector.push_back(new Point_light(couleur, myPoint3D(-1.0, 0.0, 0.0)));
	lightsVector.push_back(new Directional_light(couleur, myVector3D(-1.0, 0.0, 0.0)));
	lightsVector.push_back(new Spot_light(couleur, myPoint3D(0.0, 0.0, 0.0), myVector3D(0.0, 0.0, -1.0)));

	(lightsVector.at(indiceLight % (lightsVector.size()) ) )->sendSpecificUniform(shader_light_type, shader_light_color, shader_light_position, shader_light_direction);

}

//This function is called to display objects on screen.
void display() 
{
	int t;
	t=time;
	deltaT=t-clockSave;
	clockSave=t;
	deltaT *= 0.4;

	rotations[0] += incr[0]*deltaT;
	rotations[1] += incr[1]*deltaT;
	rotations[2] += incr[2]*deltaT;
	rotations[3] += incr[3]*deltaT;
	rotations[4] += incr[4]*deltaT;
	rotations[5] += incr[5]*deltaT;
	rotations[6] += incr[6]*deltaT;
	rotations[7] += incr[7]*deltaT;
	rotations[8] += incr[8]*deltaT;
	rotations[9] += incr[9]*deltaT;
	rotations[10] += incr[10]*deltaT;


	//Clearing the color on the screen.
	glClearColor(0.0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Setting up the projection matrix.
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, Glut_w/(float)Glut_h, zNear, zFar);
	glViewport(0, 0, Glut_w, Glut_h);

	//Setting up the modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_eye.X, camera_eye.Y, camera_eye.Z, camera_eye.X + camera_forward.dX, camera_eye.Y + camera_forward.dY, camera_eye.Z + camera_forward.dZ, camera_up.dX, camera_up.dY, camera_up.dZ);

	glUniform1fv(mytime_loc,1,&time) ; 
	setLight();

	glUniform1i(shader_light_position, 1);

	 glTranslatef( 0, 0, -8 );

	/**ADD CODE TO DRAW THE OBJ MODEL, INSTEAD OF THE CUBE**/

	glPushMatrix();
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(6,6,6);
		glTranslatef(0.0,0.0,-5.0);

		glPushMatrix();
			glScalef(10,2,0.1);
			walls[0]->displayObject(shader_texture,shader_bump);	
		glPopMatrix();

		glPushMatrix();
			glTranslatef(5,0,5);
			glScalef(0.1,2,10);
			walls[1]->displayObject(shader_texture,shader_bump);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0,0,10);
			glScalef(10,2,0.1);
			walls[2]->displayObject(shader_texture,shader_bump);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-5,0,5);
			glScalef(0.1,2,10);
			walls[3]->displayObject(shader_texture,shader_bump);
		glPopMatrix();
	glPopMatrix();

	//glTranslatef(2,0,0);
	//myobj2->displayObject(shader_texture,shader_bump);
//	myobj1->displayNormals();

	/*glPushMatrix();
	glTranslatef(0,-0.5,5);
	glScalef(10,0.1,10);
	sol->displayObject(shader_texture,shader_bump);
	glPopMatrix();*/
	//myobj2->displayObject(shader_texture,shader_bump);
	
	 glPushMatrix();

	   gluDisk(orbite,planetInfo[TERRE][DISTANCE]-TAILLE_ORBIT,planetInfo[TERRE][DISTANCE],50,1); //Orbite terre
	   gluDisk(orbite,planetInfo[MARS][DISTANCE]-TAILLE_ORBIT,planetInfo[MARS][DISTANCE],50,1); //Mars
	   gluDisk(orbite,planetInfo[JUPITER][DISTANCE]-TAILLE_ORBIT,planetInfo[JUPITER][DISTANCE],50,1); //Orbite Jupiter

	   //TERRE
	   glPushMatrix();

			glRotatef(rotations[2],0.0,0.0,1.0);
			//glTranslatef( -8, 0, 0 ); // translation bidon
			glTranslatef( planetInfo[TERRE][DISTANCE], 0, 0);
			glRotatef(rotations[1],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[TERRE][RAYON], planetInfo[TERRE][RAYON], planetInfo[TERRE][RAYON]);
				planet[TERRE]->displayObject(shader_texture,shader_bump);
			glPopMatrix();
		   //gluSphere( sphereTerre, 0.3, 32, 32 );

		   glPushMatrix();
				glRotatef(rotations[4],0.0,0.0,1.0);
				glTranslatef( 1.0, 0, 0 ); // translation bidon
				glRotatef(rotations[3],0.0,0.0,1.0);
				glPushMatrix();
					glScalef(planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON]);
					planet[LUNE]->displayObject(shader_texture,shader_bump);
				glPopMatrix();
				//gluSphere( sphereLune, 0.1, 32, 32 );
			glPopMatrix();

		glPopMatrix();

		//MARS
		glPushMatrix();

			glRotatef(rotations[2],0.0,0.0,1.0);
			//glTranslatef( -8, 0, 0 ); // translation bidon
			glTranslatef( planetInfo[MARS][DISTANCE], 0, 0);
			glRotatef(rotations[1],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[MARS][RAYON], planetInfo[MARS][RAYON], planetInfo[MARS][RAYON]);
				planet[MARS]->displayObject(shader_texture,shader_bump);
			glPopMatrix();
		   //gluSphere( sphereTerre, 0.3, 32, 32 );

		glPopMatrix();


		glPushMatrix();

			glRotatef(rotations[6],0.0,0.0,1.0);
			//glTranslatef( -16, 0, 0 ); // translation bidon
			glTranslatef( planetInfo[JUPITER][DISTANCE], 0, 0);
			glRotatef(rotations[5],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[JUPITER][RAYON], planetInfo[JUPITER][RAYON], planetInfo[JUPITER][RAYON]);
				planet[JUPITER]->displayObject(shader_texture,shader_bump);
			glPopMatrix();
			//gluSphere( sphereJupiter, 0.8, 32, 32 );

		   glPushMatrix();
				glRotatef(rotations[8],0.0,0.0,1.0);
				glTranslatef( 3.5, 0, 0 ); // translation bidon
				glRotatef(rotations[7],0.0,0.0,1.0);
				glPushMatrix();
					glScalef(planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON]);
					planet[LUNE]->displayObject(shader_texture,shader_bump);
				glPopMatrix();
		   glPopMatrix();

			glPushMatrix();
				glRotatef(rotations[10],0.0,0.0,1.0);
				glTranslatef( 3.5, 0, 0 ); // translation bidon
				glRotatef(rotations[9],0.0,0.0,1.0);
				glPushMatrix();
					glScalef(planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON]);
					planet[LUNE]->displayObject(shader_texture,shader_bump);
				glPopMatrix();
				//gluSphere( sphereCallisto, 0.1, 32, 32 );
		   glPopMatrix();

		glPopMatrix();

		glPushMatrix();

		   glTranslatef( 0, 0, 0 ); // translation bidon
		   //glRotatef(rotations[0],0.0,0.0,1.0);
		   glScalef(planetInfo[SOLEIL][RAYON], planetInfo[SOLEIL][RAYON], planetInfo[SOLEIL][RAYON]);
		   planet[0]->displayObject(shader_texture,shader_bump);
		   //gluSphere( sphereSoleil, 2, 32, 32 );

		glPopMatrix();


   glPopMatrix();

  
	
	glFlush();
}


void animation(void)
{
	time += 2.5;
	glutPostRedisplay();
}

//This function is called from the main to initalize everything.
void init()
{

	myVector3D rotation_axis(1,0,0);
	
	rotate2(camera_forward, rotation_axis, 90);
  rotate2(camera_up, rotation_axis, 90);
  rotate2(camera_eye, rotation_axis, 90);


	planetInfo[SOLEIL][RAYON] = 5;//34.7;
	planetInfo[TERRE][RAYON] = 1;
	planetInfo[MARS][RAYON] = 0.5;
	planetInfo[JUPITER][RAYON] = 3;//11.2;

	planetInfo[LUNE][RAYON] = 0.2;

	planetInfo[TERRE][DISTANCE] = 8;//149;
	planetInfo[MARS][DISTANCE] = 11;
	planetInfo[JUPITER][DISTANCE] = 16;//778 ;


	orbite = gluNewQuadric();
	sphereSoleil = gluNewQuadric();
   sphereLune = gluNewQuadric();
   sphereTerre = gluNewQuadric();
   sphereJupiter = gluNewQuadric();
   sphereEuropa = gluNewQuadric();
   sphereCallisto = gluNewQuadric();

   rotations[0]=0.0; rotations[1]=0.0; rotations[2]=45.0; rotations[3]=0.0; rotations[4]=30.0; rotations[5]=0.0;
   rotations[6]=30.0; rotations[7]=0.0; rotations[8]=15.0; rotations[9]=0.0; rotations[10]=2.0;

   incr[0]=0.001; incr[1]=0.5; incr[2]=0.005; incr[3]=0.5; incr[4]=0.07; incr[5]=0.01; incr[6]=0.003; incr[7]=0.7;
   incr[8]=0.09; incr[9]=0.8; incr[10]=0.7;

    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    shaderprogram = initprogram(vertexshader, fragmentshader) ; 

	mytime_loc = glGetUniformLocation(shaderprogram, "mytime") ;
	shader_light_position = glGetUniformLocation(shaderprogram, "mylight_position");
	shader_light_color = glGetUniformLocation(shaderprogram, "mylight_color");
	shader_light_direction = glGetUniformLocation(shaderprogram, "mylight_direction");
	shader_light_type = glGetUniformLocation(shaderprogram, "mylight_type");
	shader_silhouette = glGetUniformLocation(shaderprogram, "silhouette");
	shader_texture = glGetUniformLocation(shaderprogram, "tex");
	shader_bump = glGetUniformLocation(shaderprogram, "bump");

	myTexture *t = new myTexture();
	t->readTexture("ppm/ppm/br_color.ppm");
	//t->readTexture("ppm/ppm/sunmap.ppm");

	myTexture *soleil = new myTexture();
	soleil->readTexture("ppm/ppm/sunmap.ppm");
	myTexture *terre = new myTexture();
	terre->readTexture("ppm/ppm/earthmap1k.ppm");

	myTexture *b = new myTexture();
	b->readTexture("ppm/ppm/br_normal.ppm");
	
	int i;
	for(i=0; i<4; i++)
	{
		walls[i] = new myObject3D();
		walls[i]->readMesh("cube.obj");
		walls[i]->computeNormals();
		walls[i]->computeCylinderTexture();
		walls[i]->computeCylinderBump();
		walls[i]->createObjectBuffers();
		walls[i]->mytex = t;
		walls[i]->mybump = b;
	}

	for(i=0; i<20; i++)
	{
		planet[i] = new myObject3D();
		planet[i]->readMesh("TriangularObjs/ball.objtri.obj");
		planet[i]->computeNormals();
		planet[i]->computeCylinderTexture();
		planet[i]->computeCylinderBump();
		planet[i]->createObjectBuffers();
	}
	planet[SOLEIL]->mytex = soleil;
	planet[SOLEIL]->mybump = b;
	for(i=1; i<20; i++)
	{
		planet[i]->mytex = terre;
		planet[i]->mybump = b;
	}

	myTexture *t2 = new myTexture();
	t2->readTexture("ppm/ppm/4351-diffuse.ppm");

	/*sol = new myObject3D();
	sol->readMesh("cube.obj");
	sol->computeNormals();
	sol->computeCylinderTexture();
	sol->computeCylinderBump();
	sol->createObjectBuffers();
	sol->mytex = t2;
	sol->mybump = b;*/

	/*myobj2 = new myObject3D();
	myobj2->readMesh("TriangularObjs/zune_120.objtri.obj");
	myobj2->computeNormals();
	myobj2->computeCylinderTexture();
	myobj2->computeCylinderBump();
	myobj2->createObjectBuffers();
	myobj2->mytex = t;
	myobj2->mybump = b;*/
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("My OpenGL Application");
	   
	glewInit() ; 
	glutReshapeWindow(Glut_w, Glut_h);
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard2);
	glutMotionFunc(mousedrag) ;
	glutIdleFunc(animation);
	glutMouseFunc(mouse) ;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc (GL_LESS) ;



	init();

	glutMainLoop();
	return 0;
}