#include "StdAfx.h"
#include "light.h"


light::light(float _couleur[4], int _type)
{
	for(int i=0;i<4;i++) couleur[i] = _couleur[i];
	type = _type;
}

void light::sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction)
{
	glUniform1i(shader_light_type, type);
	glUniform4f(shader_light_color, couleur[0], couleur[1], couleur[2], couleur[3]);
}



light::~light(void)
{
}


/*
* POINT LIGHT
*/
Point_light::Point_light(float _couleur[4], myPoint3D _point) : light(_couleur,POINT_LIGHT)
{
	point = _point;
}

void Point_light::sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction)
{
	light::sendSpecificUniform(shader_light_type, shader_light_color, shader_light_position, shader_light_direction);
	glUniform3f(shader_light_position, point.X, point.Y, point.Z);
}

Point_light::~Point_light(void)
{
}

/*
* DIRECTIONal LIGHT
*/
Directional_light::Directional_light(float _couleur[4], myVector3D _direction) : light(_couleur,DIRECTIONAL_LIGHT)
{
	direction = _direction;
}

void Directional_light::sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction)
{
	light::sendSpecificUniform(shader_light_type, shader_light_color, shader_light_position, shader_light_direction);
	glUniform3f(shader_light_direction, direction.dX, direction.dY, direction.dZ);

}

Directional_light::~Directional_light(void)
{
}

/*
* SPOT LIGHT
*/
Spot_light::Spot_light(float _couleur[4], myPoint3D _point, myVector3D _direction) : light(_couleur,SPOT_LIGHT)
{
	direction = _direction;
	point = _point;
}

void Spot_light::sendSpecificUniform(GLuint shader_light_type, GLuint shader_light_color, GLuint shader_light_position, GLuint shader_light_direction)
{
	light::sendSpecificUniform(shader_light_type, shader_light_color, shader_light_position, shader_light_direction);
	glUniform3f(shader_light_direction, direction.dX, direction.dY, direction.dZ);
	glUniform3f(shader_light_position, point.X, point.Y, point.Z);
}

Spot_light::~Spot_light(void)
{
}
