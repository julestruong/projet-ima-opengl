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

// width and height of the window.
int Glut_w = 600, Glut_h = 400; 

std::vector<light*> lightsVector;
int indiceLight = 0;

//Variables and their values for the camera setup.
myPoint3D camera_eye(0,0,5);
myVector3D camera_up(0,1,0);
myVector3D camera_forward (0,0,-1);

float time = 0;
GLuint mytime_loc;
GLuint shader_light_position;
GLuint shader_light_color;
GLuint shader_light_direction;
GLuint shader_light_type;
GLuint shader_silhouette;

bool silhouette = false;


GLuint vertexshader, fragmentshader, shaderprogram ; // shaders

float fovy = 90;
float zNear = 0.1;
float zFar = 60;

int button_pressed = 0; // 1 if a button is currently being pressed.
int GLUTmouse[2] = { 0, 0 };

myObject3D *myobj1;
myObject3D *myobj2;

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
	switch(key) {
	case GLUT_KEY_UP:
		/**ADD CODE TO MOVE FORWARD**/
		camera_eye.X += 0.1*camera_forward.dX; camera_eye.Y += 0.1*camera_forward.dY;camera_eye.Z += 0.1*camera_forward.dZ;
		//camera_eye += camera_forward;
		break;
	case GLUT_KEY_DOWN:
		/**ADD CODE TO MOVE BACK**/
		camera_eye.X -= 0.1*camera_forward.dX; camera_eye.Y -= 0.1*camera_forward.dY;camera_eye.Z -= 0.1*camera_forward.dZ;
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

	/**ADD CODE TO DRAW THE OBJ MODEL, INSTEAD OF THE CUBE**/
	myobj1->displayObject();
//	myobj1->displayNormals();

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
    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    shaderprogram = initprogram(vertexshader, fragmentshader) ; 

	mytime_loc = glGetUniformLocation(shaderprogram, "mytime") ;
	shader_light_position = glGetUniformLocation(shaderprogram, "mylight_position");
	shader_light_color = glGetUniformLocation(shaderprogram, "mylight_color");
	shader_light_direction = glGetUniformLocation(shaderprogram, "mylight_direction");
	shader_light_type = glGetUniformLocation(shaderprogram, "mylight_type");
	shader_silhouette = glGetUniformLocation(shaderprogram, "silhouette");

	myobj1 = new myObject3D();
	myobj1->readMesh("hand.obj");
	myobj1->computeNormals();
	myobj1->createObjectBuffers();
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