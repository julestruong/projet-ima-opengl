#include <math.h>

void crossproduct(myVector3D & v1, myVector3D & v2, myVector3D & result)
{
	result.dX = v1.dY * v2.dZ - v1.dZ * v2.dY;
	result.dY = v1.dZ * v2.dX - v1.dX * v2.dZ;
	result.dZ = v1.dX * v2.dY - v1.dY * v2.dX;
}

void rotate2(myVector3D & input, myVector3D &lp, double theta)
{
  //rotate point input around the line defined by lp through the origin by theta degrees.
  const double cos_theta = cos(theta);
  const double dot = input*lp;
  myVector3D cross; crossproduct(input, lp, cross);
  input.dX *= cos_theta; input.dY *= cos_theta; input.dZ *= cos_theta; 
  input.dX += lp.dX * dot * (1.0-cos_theta);input.dY += lp.dY * dot * (1.0-cos_theta);input.dZ += lp.dZ * dot * (1.0-cos_theta);
  input.dX -= cross.dX*sin(theta);input.dY -= cross.dY*sin(theta);input.dZ -= cross.dZ*sin(theta);
}

void rotate2(myPoint3D & input, myVector3D & lp, double theta)
{
	myVector3D tmp(input.X, input.Y, input.Z);
	rotate2(tmp, lp, theta);
	input.X = tmp.dX; input.Y = tmp.dY; input.Z = tmp.dZ;
}

