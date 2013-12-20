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
#define SPOT_LIGHT 

#define DIST_WALL 90

#define TAILLE_ORBIT 0.05

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
#define REVOLUTION 2
#define ROTATION 3
#define VITESSEREVOLUTION 4
#define VITESSEROTATION 5

int indice;

float planetInfo[20][6];


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
GLuint shader_light_position2;
GLuint shader_light_color2;
GLuint shader_light_direction2;
GLuint shader_light_type2;
GLuint shader_silhouette;
GLuint shader_texture;

bool silhouette = false;


GLuint vertexshader, fragmentshader, shaderprogram ; // shaders

float fovy = 90;
float zNear = 0.1;
float zFar = 300;//60;

int button_pressed = 0; // 1 if a button is currently being pressed.
int GLUTmouse[2] = { 0, 0 };

myObject3D *myobj1;
myObject3D *myobj2;

myObject3D* walls[6];
myObject3D* planet[20];
myObject3D* plane;
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

bool testWall()
{
	if(camera_eye.X > -DIST_WALL && camera_eye.X < DIST_WALL && camera_eye.Y > -DIST_WALL && camera_eye.Y < DIST_WALL  && camera_eye.Z > -DIST_WALL  && camera_eye.Z < DIST_WALL)
	{
		return true;
	}
	return false;
}

//This function is called when an arrow key is pressed.
void keyboard2(int key, int x, int y) {
	float speed = 1.0;
	switch(key) {
	case GLUT_KEY_UP:
		/**ADD CODE TO MOVE FORWARD**/
		camera_eye.X += speed*camera_forward.dX; camera_eye.Y += speed*camera_forward.dY;camera_eye.Z += speed*camera_forward.dZ;
		if(!testWall())
		{
			camera_eye.X -= speed*camera_forward.dX; camera_eye.Y -= speed*camera_forward.dY;camera_eye.Z -= speed*camera_forward.dZ;
		}
		//camera_eye += camera_forward;
		break;
	case GLUT_KEY_DOWN:
		/**ADD CODE TO MOVE BACK**/
		camera_eye.X -= speed*camera_forward.dX; camera_eye.Y -= speed*camera_forward.dY;camera_eye.Z -= speed*camera_forward.dZ;
		if(!testWall())
		{
			camera_eye.X += speed*camera_forward.dX; camera_eye.Y += speed*camera_forward.dY;camera_eye.Z += speed*camera_forward.dZ;
		}
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
	
	lightsVector.push_back(new Spot_light(couleur, myPoint3D(0.0, 0.0, 0.0), myVector3D(0.0, 0.0, -1.0)));
	lightsVector.push_back(new Point_light(couleur, myPoint3D(-1.0, 0.0, 0.0)));
	lightsVector.push_back(new Directional_light(couleur, myVector3D(-1.0, 0.0, 0.0)));

	(lightsVector.at(indiceLight % (lightsVector.size()) ) )->sendSpecificUniform(shader_light_type, shader_light_color, shader_light_position, shader_light_direction);

	Point_light *pt_ligth = new Point_light(couleur, myPoint3D(0.0, 0.0, -1.0));
		pt_ligth->sendSpecificUniform(shader_light_type2, shader_light_color2, shader_light_position2, shader_light_direction2);
}

//This function is called to display objects on screen.
void display() 
{
	int t;
	t=time;
	deltaT=t-clockSave;
	clockSave=t;
	deltaT *= 0.4;

	for(int i=0; i<20; i++)
	{
		planetInfo[i][REVOLUTION] += planetInfo[i][VITESSEREVOLUTION]*deltaT;
		planetInfo[i][ROTATION] += planetInfo[i][VITESSEROTATION]*deltaT;
	}
	
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

	glPushMatrix();
		glTranslatef( 0, -0.2, -0.5 );
		glRotatef(-90, 0.0, 0.0, 1.0);
		glRotatef(-80, 0.0, 1.0, 0.0);
		glScalef(0.3,0.3,0.3);
		plane->displayObject(shader_texture);
	glPopMatrix();  

	gluLookAt(camera_eye.X, camera_eye.Y, camera_eye.Z, camera_eye.X + camera_forward.dX, camera_eye.Y + camera_forward.dY, camera_eye.Z + camera_forward.dZ, camera_up.dX, camera_up.dY, camera_up.dZ);

	glUniform1fv(mytime_loc,1,&time) ; 
	setLight();


	glUniform1i(shader_light_position, 1);

	 glTranslatef( 0, 0, -8 );

	//MURS
	glPushMatrix();
	glScalef(200,200,200);
		glPushMatrix();
			glTranslatef(0,0,-0.5);
			walls[0]->displayObject(shader_texture);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0,0.5);
			walls[1]->displayObject(shader_texture);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,-0.5,0);
			glRotatef(90,1,0,0);
			walls[2]->displayObject(shader_texture);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0.5,0);
			glRotatef(90,1,0,0);
			walls[3]->displayObject(shader_texture);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-0.5,0,0);
			glRotatef(90,0,1,0);
			walls[4]->displayObject(shader_texture);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.5,0,0);
			glRotatef(90,0,1,0);
			walls[5]->displayObject(shader_texture);
		glPopMatrix();
	glPopMatrix();
	


	 glPushMatrix();

	   //MERCURE
		glPushMatrix();
			glRotatef( planetInfo[MERCURE][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[MERCURE][DISTANCE], 0, 0);
			glRotatef( planetInfo[MERCURE][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[MERCURE][RAYON], planetInfo[MERCURE][RAYON], planetInfo[MERCURE][RAYON]);
				planet[MERCURE]->displayObject(shader_texture);
			glPopMatrix();
		glPopMatrix();

		//VENUS
		glPushMatrix();
			glRotatef( planetInfo[VENUS][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[VENUS][DISTANCE], 0, 0);
			glRotatef(planetInfo[VENUS][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[VENUS][RAYON], planetInfo[VENUS][RAYON], planetInfo[VENUS][RAYON]);
				planet[VENUS]->displayObject(shader_texture);
			glPopMatrix();
		glPopMatrix();

	   //TERRE
	   glPushMatrix();

			glRotatef(planetInfo[TERRE][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[TERRE][DISTANCE], 0, 0);
			glRotatef(planetInfo[TERRE][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[TERRE][RAYON], planetInfo[TERRE][RAYON], planetInfo[TERRE][RAYON]);
				planet[TERRE]->displayObject(shader_texture);
			glPopMatrix();

		   glPushMatrix();
				glRotatef(planetInfo[LUNE][REVOLUTION],0.0,0.0,1.0);
				glTranslatef( 1.0, 0, 0 ); 
				glRotatef(planetInfo[LUNE][ROTATION],0.0,0.0,1.0);
				glPushMatrix();
					glScalef(planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON], planetInfo[LUNE][RAYON]);
					planet[LUNE]->displayObject(shader_texture);
				glPopMatrix();
			glPopMatrix();

		glPopMatrix();

		//MARS
		glPushMatrix();

			glRotatef(planetInfo[MARS][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[MARS][DISTANCE], 0, 0);
			glRotatef(planetInfo[MARS][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[MARS][RAYON], planetInfo[MARS][RAYON], planetInfo[MARS][RAYON]);
				planet[MARS]->displayObject(shader_texture);
			glPopMatrix();

		glPopMatrix();

		//JUPITER
		glPushMatrix();

			glRotatef(planetInfo[JUPITER][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[JUPITER][DISTANCE], 0, 0);
			glRotatef(planetInfo[JUPITER][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[JUPITER][RAYON], planetInfo[JUPITER][RAYON], planetInfo[JUPITER][RAYON]);
				planet[JUPITER]->displayObject(shader_texture);
			glPopMatrix();

		glPopMatrix();

		//SATURNE
		glPushMatrix();
			glRotatef(planetInfo[SATURNE][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[SATURNE][DISTANCE], 0, 0);
			glRotatef(planetInfo[SATURNE][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glPushMatrix();
					glScalef(planetInfo[SATURNE][RAYON], planetInfo[SATURNE][RAYON], planetInfo[SATURNE][RAYON]);
					planet[SATURNE]->displayObject(shader_texture);
					glPopMatrix();
				glRotatef(25,0,1,0);
				gluDisk(orbite,planetInfo[SATURNE][RAYON]-1.5,planetInfo[SATURNE][RAYON]-0.2,50,1); //Orbite NEPTUNE
			glPopMatrix();
		glPopMatrix();

		//URANUS
		glPushMatrix();
			glRotatef(planetInfo[URANUS][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[URANUS][DISTANCE], 0, 0);
			glRotatef(planetInfo[URANUS][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[URANUS][RAYON], planetInfo[URANUS][RAYON], planetInfo[URANUS][RAYON]);
				planet[URANUS]->displayObject(shader_texture);
			glPopMatrix();
		glPopMatrix();

		//NEPTUNE
		glPushMatrix();
			glRotatef(planetInfo[NEPTUNE][REVOLUTION],0.0,0.0,1.0);
			glTranslatef( planetInfo[NEPTUNE][DISTANCE], 0, 0);
			glRotatef(planetInfo[NEPTUNE][ROTATION],0.0,0.0,1.0);
			glPushMatrix();
				glScalef(planetInfo[NEPTUNE][RAYON], planetInfo[NEPTUNE][RAYON], planetInfo[NEPTUNE][RAYON]);
				planet[NEPTUNE]->displayObject(shader_texture);
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		   glTranslatef( 0, 0, 0 );
		   glScalef(planetInfo[SOLEIL][RAYON], planetInfo[SOLEIL][RAYON], planetInfo[SOLEIL][RAYON]);
		   planet[0]->displayObject(shader_texture);

		glPopMatrix();

   glPopMatrix();

		//orbites
	   gluDisk(orbite,planetInfo[MERCURE][DISTANCE]-TAILLE_ORBIT,planetInfo[MERCURE][DISTANCE],50,1); //Orbite MERCURE
	   gluDisk(orbite,planetInfo[VENUS][DISTANCE]-TAILLE_ORBIT,planetInfo[VENUS][DISTANCE],50,1); //VENUS
	   gluDisk(orbite,planetInfo[TERRE][DISTANCE]-TAILLE_ORBIT,planetInfo[TERRE][DISTANCE],50,1); //Orbite terre
	   gluDisk(orbite,planetInfo[MARS][DISTANCE]-TAILLE_ORBIT,planetInfo[MARS][DISTANCE],50,1); //Mars
	   gluDisk(orbite,planetInfo[JUPITER][DISTANCE]-TAILLE_ORBIT,planetInfo[JUPITER][DISTANCE],50,1); //Orbite Jupiter
	   gluDisk(orbite,planetInfo[SATURNE][DISTANCE]-TAILLE_ORBIT,planetInfo[SATURNE][DISTANCE],50,1); //Orbite SATURNE
	   gluDisk(orbite,planetInfo[URANUS][DISTANCE]-TAILLE_ORBIT,planetInfo[URANUS][DISTANCE],50,1); //URANUS
	   gluDisk(orbite,planetInfo[NEPTUNE][DISTANCE]-TAILLE_ORBIT,planetInfo[NEPTUNE][DISTANCE],50,1); //Orbite NEPTUNE
	
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


	planetInfo[SOLEIL][RAYON] = 12;//34.7;
	planetInfo[MERCURE][RAYON] = 2.38;
	planetInfo[VENUS][RAYON] = 2.95;
	planetInfo[TERRE][RAYON] = 3;
	planetInfo[MARS][RAYON] = 2.5;
	planetInfo[JUPITER][RAYON] = 7.5;//11.2;
	planetInfo[SATURNE][RAYON] = 6.5;//9.4;
	planetInfo[URANUS][RAYON] = 4;//4;
	planetInfo[NEPTUNE][RAYON] = 4.0;//3.8;
	

	planetInfo[LUNE][RAYON] = 0.2;

	planetInfo[MERCURE][DISTANCE] = 15;
	planetInfo[VENUS][DISTANCE] = 20;
	planetInfo[TERRE][DISTANCE] = 25;//149;
	planetInfo[MARS][DISTANCE] = 30;
	planetInfo[JUPITER][DISTANCE] = 45;//778 ;
	planetInfo[SATURNE][DISTANCE] = 65;//149;
	planetInfo[URANUS][DISTANCE] = 80;
	planetInfo[NEPTUNE][DISTANCE] = 95;//778 ;

	planetInfo[SOLEIL][REVOLUTION] = 0.0;
	planetInfo[LUNE][REVOLUTION] = 0.2;

	planetInfo[MERCURE][REVOLUTION] = 10;
	planetInfo[VENUS][REVOLUTION] = 120;
	planetInfo[TERRE][REVOLUTION] = 17.5;//149;
	planetInfo[MARS][REVOLUTION] = 90;
	planetInfo[JUPITER][REVOLUTION] = 180;//778 ;
	planetInfo[SATURNE][REVOLUTION] = 60;//149;
	planetInfo[URANUS][REVOLUTION] = 30;
	planetInfo[NEPTUNE][REVOLUTION] = 92;//778 ;

	planetInfo[SOLEIL][ROTATION] = 0.000;
	planetInfo[LUNE][ROTATION] = 20;

	planetInfo[MERCURE][ROTATION] = 2;
	planetInfo[VENUS][ROTATION] = 30;
	planetInfo[TERRE][ROTATION] = 15;//149;
	planetInfo[MARS][ROTATION] = 10;
	planetInfo[JUPITER][ROTATION] = 5;//778 ;
	planetInfo[SATURNE][ROTATION] = 10;//149;
	planetInfo[URANUS][ROTATION] = 15;
	planetInfo[NEPTUNE][ROTATION] = 2;//778 ;

	planetInfo[SOLEIL][VITESSEREVOLUTION] = 0.0;
	planetInfo[LUNE][VITESSEREVOLUTION] = 0.0;

	planetInfo[MERCURE][VITESSEREVOLUTION] = 0.09;
	planetInfo[VENUS][VITESSEREVOLUTION] = 0.08;
	planetInfo[TERRE][VITESSEREVOLUTION] = 0.075;//149;
	planetInfo[MARS][VITESSEREVOLUTION] = 0.05;
	planetInfo[JUPITER][VITESSEREVOLUTION] = 0.025;//778 ;
	planetInfo[SATURNE][VITESSEREVOLUTION] = 0.015;//149;
	planetInfo[URANUS][VITESSEREVOLUTION] = 0.01;
	planetInfo[NEPTUNE][VITESSEREVOLUTION] = 0.01;//778 ;

	planetInfo[SOLEIL][VITESSEROTATION] = 0.00;
	planetInfo[LUNE][VITESSEROTATION] = 0.01;

	planetInfo[MERCURE][VITESSEROTATION] = 0.05;
	planetInfo[VENUS][VITESSEROTATION] = 0.05;
	planetInfo[TERRE][VITESSEROTATION] = 0.07;//149;
	planetInfo[MARS][VITESSEROTATION] = 0.03;
	planetInfo[JUPITER][VITESSEROTATION] = 0.05;//778 ;
	planetInfo[SATURNE][VITESSEROTATION] = 0.09;//149;
	planetInfo[URANUS][VITESSEROTATION] = 0.01;
	planetInfo[NEPTUNE][VITESSEROTATION] = 0.02;//778 ;



	orbite = gluNewQuadric();
	sphereSoleil = gluNewQuadric();
   sphereLune = gluNewQuadric();
   sphereTerre = gluNewQuadric();
   sphereJupiter = gluNewQuadric();
   sphereEuropa = gluNewQuadric();
   sphereCallisto = gluNewQuadric();

   /*rotations[0]=0.0; rotations[1]=0.0; rotations[2]=45.0; rotations[3]=0.0; rotations[4]=30.0; rotations[5]=0.0;
   rotations[6]=30.0; rotations[7]=0.0; rotations[8]=15.0; rotations[9]=0.0; rotations[10]=2.0;

   incr[0]=0.001; incr[1]=0.5; incr[2]=0.005; incr[3]=0.5; incr[4]=0.07; incr[5]=0.01; incr[6]=0.003; incr[7]=0.7;
   incr[8]=0.09; incr[9]=0.8; incr[10]=0.7;*/

    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    shaderprogram = initprogram(vertexshader, fragmentshader) ; 

	mytime_loc = glGetUniformLocation(shaderprogram, "mytime") ;
	shader_light_position = glGetUniformLocation(shaderprogram, "mylight_position");
	shader_light_color = glGetUniformLocation(shaderprogram, "mylight_color");
	shader_light_direction = glGetUniformLocation(shaderprogram, "mylight_direction");
	shader_light_type = glGetUniformLocation(shaderprogram, "mylight_type");
		shader_light_position2 = glGetUniformLocation(shaderprogram, "mylight_position2");
	shader_light_color2 = glGetUniformLocation(shaderprogram, "mylight_color2");
	shader_light_direction2 = glGetUniformLocation(shaderprogram, "mylight_direction2");
	shader_light_type2 = glGetUniformLocation(shaderprogram, "mylight_type2");
	shader_silhouette = glGetUniformLocation(shaderprogram, "silhouette");
	shader_texture = glGetUniformLocation(shaderprogram, "tex");

	myTexture *t = new myTexture();
	t->readTexture("ppm/ppm/space.ppm");
	//t->readTexture("ppm/ppm/sunmap.ppm");

	myTexture *soleil = new myTexture();
	soleil->readTexture("Textures/sunmap.ppm");
	myTexture *lune = new myTexture();
	lune->readTexture("Textures/moonmap1k.ppm");

	myTexture *mercure = new myTexture();
	mercure->readTexture("Textures/mercurymap.ppm");
	myTexture *venus = new myTexture();
	venus->readTexture("Textures/venusmap.ppm");
	myTexture *terre = new myTexture();
	terre->readTexture("Textures/earthmap1k.ppm");
	myTexture *mars = new myTexture();
	mars->readTexture("Textures/mars_1k_color.ppm");
	myTexture *jupiter = new myTexture();
	jupiter->readTexture("Textures/jupitermap.ppm");
	myTexture *saturne = new myTexture();
	saturne->readTexture("Textures/saturnmap.ppm");
	myTexture *uranus = new myTexture();
	uranus->readTexture("Textures/uranusmap.ppm");
	myTexture *neptune = new myTexture();
	neptune->readTexture("Textures/neptunemap.ppm");



	myTexture *b = new myTexture();
	b->readTexture("ppm/4241-diffuse.ppm");
	
	int i;
	for(i=0; i<6; i++)
	{
		walls[i] = new myObject3D();
		walls[i]->readMesh("carree.obj");
		walls[i]->computeNormals();
		walls[i]->computeSquareTexture();
		walls[i]->createObjectBuffers();
		walls[i]->mytex = t;
	}

	for(i=0; i<20; i++)
	{
		planet[i] = new myObject3D();
		//planet[i]->readMesh("TriangularObjs/ball.objtri.obj");
		planet[i]->readMesh("TriangularObjs/sphere.obj");
		planet[i]->computeNormals();
		planet[i]->computeCylinderTexture();
		planet[i]->createObjectBuffers();
	}
	planet[SOLEIL]->mytex = soleil;
	planet[MERCURE]->mytex = mercure;
	planet[VENUS]->mytex = venus;
	planet[TERRE]->mytex = terre;
	planet[MARS]->mytex = mars;
	planet[JUPITER]->mytex = jupiter;
	planet[SATURNE]->mytex = saturne;
	planet[URANUS]->mytex = uranus;
	planet[NEPTUNE]->mytex = neptune;
	planet[LUNE]->mytex = lune;


	plane = new myObject3D();
	plane->readMesh("TriangularObjs/shuttle.objtri.obj");
	plane->computeNormals();
	plane->computeCylinderTexture();
	plane->createObjectBuffers();
	plane->mytex = b;
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