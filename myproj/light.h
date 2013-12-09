#pragma once

#include <GL/glew.h>

#include "vector3d.h"
#include "point3d.h"

#define POINT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
#define SPOT_LIGHT 3

class light
{
public:
	light(float _couleur[4], int _type);
	~light(void);

	virtual void sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction) = 0;

	float couleur[4];
	GLuint type;
};

class Point_light : public light
{
public:
	Point_light(float _couleur[4], myPoint3D _point);
	~Point_light(void);

	void sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction);

	myPoint3D point;
};

class Directional_light : public light
{
public:
	Directional_light(float _couleur[4], myVector3D _direction);
	~Directional_light(void);

	void sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction);

	myVector3D direction;
};

class Spot_light : public light
{
public:
	Spot_light(float _couleur[4], myPoint3D _point, myVector3D _direction);
	~Spot_light(void);

	void sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction);

	myVector3D direction;
	myPoint3D point;
};


