#include "stdafx.h"
#include "vector3d.h"
#include "point3d.h"
#include <iostream>
 
myVector3D::myVector3D() {}

myVector3D::myVector3D(double dx, double dy, double dz)
{
    dX = dx;
    dY = dy;
    dZ = dz;
}

double myVector3D::operator*(myVector3D & v1)
{
    return (v1.dX*dX + v1.dY*dY + v1.dZ*dZ);
}

myVector3D myVector3D::operator+(myVector3D & v1)
{
	return myVector3D(dX+v1.dX, dY+v1.dY, dZ+v1.dZ);
}

myVector3D myVector3D::operator-()
{
	return myVector3D(-dX, -dY, -dZ);
}

myVector3D myVector3D::operator*(double & s)
{
	return myVector3D(dX*s, dY*s, dZ*s);
}

double myVector3D::length( )
{
    return sqrt( dX*dX + dY*dY + dZ*dZ ); 
}

void myVector3D::normalize( )
{
    double l = length();
    dX = dX/l;
    dY = dY/l;
    dZ = dZ/l;
}

void myVector3D::print(char *s)
{
	  std::cout << s << dX << ", " << dY << ", " << dZ << "\n";
}
