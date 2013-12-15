#include <math.h>
#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>

#include "myTexture.h"

#define PI 3.14159265358979323846

class myObject3D
{
public:
	GLuint buffers[5];

	GLfloat *vertices;
	GLuint *indices;
	GLfloat *normals;
	GLfloat *textures;
	GLfloat *bumps;

	myTexture *mytex;
	myTexture *mybump;

	int n, m;
  
	myObject3D() {
	}

	void computeCylinderTexture()
	{

		//textures.resize(2*n);
		textures = new GLfloat[2*n];
		GLfloat x, y, z;
		for (int i=0;i<n;i++)
		{
			x = vertices[3*i]; y = vertices[3*i+1]; z = vertices[3*i+2];
	
			textures[2*i] = z;
			if ( y>=0.0f )     textures[2*i+1] = atan2(  y,  x ) / (PI) ;
			else if ( y<0.0f )  textures[2*i+1] = (- atan2(  y,  x )) / (PI) ;
			//this has problems at the seam, when 1->0 and so interpoltion results in the whole image squeezed between the two border vertices.
			//if ( y>=0.0f )     textures[2*i+1] = atan2(  y,  x ) / (2*PI) ;
			//else if ( y<0.0f )  textures[2*i+1] = (2*PI + atan2(  y,  x )) / (2*PI) ;
		}
	}

	void computeCylinderBump()
	{

		
		bumps = new GLfloat[2*n];
		GLfloat x, y, z;
		for (int i=0;i<n;i++)
		{
			x = vertices[3*i]; y = vertices[3*i+1]; z = vertices[3*i+2];
	
			bumps[2*i] = z;
			if ( y>=0.0f )     bumps[2*i+1] = atan2(  y,  x ) / (PI) ;
			else if ( y<0.0f )  bumps[2*i+1] = (- atan2(  y,  x )) / (PI) ;
		
		}
	}

	void normalize()
	{
		int i;
		int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

		for (i=0;i<n;i++) {
			if (vertices[3*i] < vertices[3*tmpxmin]) tmpxmin = i;
			if (vertices[3*i] > vertices[3*tmpxmax]) tmpxmax = i;

			if (vertices[3*i+1] < vertices[3*tmpymin+1]) tmpymin = i;
			if (vertices[3*i+1] > vertices[3*tmpymax+1]) tmpymax = i;

			if (vertices[3*i+2] < vertices[3*tmpzmin+2]) tmpzmin = i;
			if (vertices[3*i+2] > vertices[3*tmpzmax+2]) tmpzmax = i;
		}

		double xmin = vertices[3*tmpxmin], xmax = vertices[3*tmpxmax], 
			   ymin = vertices[3*tmpymin+1], ymax = vertices[3*tmpymax+1], 
			   zmin = vertices[3*tmpzmin+2], zmax = vertices[3*tmpzmax+2];

		double scale = (xmax-xmin) <= (ymax-ymin) ? (xmax-xmin) : (ymax-ymin);
		//double scale = fmin( (xmax-xmin), (ymax-ymin) );
		scale = scale >= (zmax-zmin) ? scale : (zmax-zmin);
		//scale = fmax(scale, (zmax-zmin));

		for (i=0;i<n;i++) {
			vertices[3*i] -= (xmax+xmin)/2;
			vertices[3*i+1] -= (ymax+ymin)/2;
			vertices[3*i+2] -= (zmax+zmin)/2;

			vertices[3*i] /= scale;
			vertices[3*i+1] /= scale;
			vertices[3*i+2] /= scale;
		}
	}


	void readMesh(char *filename)
	{
		FILE* fichier = NULL;

		fichier = fopen(filename, "r+");
		char ligne[100] = "";
		int nbVertex, nbFaces;
		int i;

		if(fichier != NULL)
		{
			fgets(ligne, 100, fichier);

			sscanf(ligne,"%d %d",&nbVertex, &nbFaces);
			n=nbVertex;
			m=nbFaces;

			vertices = new GLfloat[3*n];
			indices = new GLuint[3*m];

			for(i=0; i<n; i++)
			{
				fgets(ligne, 100, fichier);
				sscanf(ligne,"v %f %f %f",&vertices[3*i],&vertices[3*i + 1],&vertices[3*i + 2]);
			}

			for(i=0; i<m; i++)
			{
				fgets(ligne, 100, fichier);
				sscanf(ligne,"f %u %u %u",&indices[3*i],&indices[3*i + 1],&indices[3*i + 2]);
				indices[3*i]--;indices[3*i + 1]--;indices[3*i + 2]--;
			}



			/*
			for(i=m*3 - 6; i<m*3; i++)
			{
				printf("%u \n",indices[i]);
			}
			*/
			normalize();

			fclose(fichier);
		}
 
	}

	void computeNormal(int v1, int v2, int v3, float & x, float & y, float & z)
	{
		double dx1 = vertices[v2*3] - vertices[v1*3];
		double dx2 = vertices[v3*3] - vertices[v2*3];
		double dy1 = vertices[v2*3+1] - vertices[v1*3+1];
		double dy2 = vertices[v3*3+1] - vertices[v2*3+1];
		double dz1 = vertices[v2*3+2] - vertices[v1*3+2];
		double dz2 = vertices[v3*3+2] - vertices[v2*3+2];


		double dx = dy1 * dz2 - dz1 * dy2;
		double dy = dz1 * dx2 - dx1 * dz2;
		double dz = dx1 * dy2 - dy1 * dx2;

		double length = sqrt(dx*dx + dy*dy + dz*dz);

		x = dx/length;
		y = dy/length;
		z = dz/length;
	}

	void computeNormals( )
	{
		int i, j, k;
		float x1, y1, z1;

		normals = new GLfloat[3*n];
		for (i=0;i<n;i++) {
			k = 0;
			normals[3*i] = 0;
			normals[3*i+1] = 0;
			normals[3*i+2] = 0;

			for (j=0;j<m;j++) {
				if (indices[3*j] == i || indices[3*j+1] == i || indices[3*j+2] == i )
				{
					computeNormal(indices[3*j], indices[3*j+1], indices[3*j+2], x1, y1, z1);
					normals[3*i] += x1; normals[3*i+1] += y1; normals[3*i+2] += z1;
					k++;
				}
			}	
			normals[3*i] /= k; normals[3*i+1] /= k; normals[3*i+2] /= k;
		}
	}

	void createObjectBuffers()
	{
		glGenBuffers(5, buffers);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, n*3*4, vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m*3*4, indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]) ; 
		glBufferData(GL_ARRAY_BUFFER, n*3*4, normals, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, n*2*4, textures, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
		glBufferData(GL_ARRAY_BUFFER, n*2*4, bumps, GL_STATIC_DRAW);
	}

	void displayObject(GLuint shader_texture,GLuint shader_bump)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
		glNormalPointer(GL_FLOAT, 0, 0) ; 
		glEnableClientState(GL_NORMAL_ARRAY) ;


		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glTexCoordPointer(2,GL_FLOAT,0,0) ;
		glEnableClientState(GL_TEXTURE_COORD_ARRAY) ;
		glUniform1i(shader_texture, 1); 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mytex->texName) ;

		glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
		glTexCoordPointer(2,GL_FLOAT,0,0) ;
		glEnableClientState(GL_TEXTURE_COORD_ARRAY) ;
		glUniform1i(shader_bump, 2); 
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mybump->texName) ;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);

		glDrawElements(GL_TRIANGLES, m*3, GL_UNSIGNED_INT, 0) ; 
	}

	void displayNormals()
	{
		for (int i=0;i<n;i++)
		{
			glBegin(GL_LINES);
			glColor3f(0,1,0);
			glVertex3f(vertices[3*i], vertices[3*i+1], vertices[3*i+2]);
			glVertex3f(vertices[3*i] + normals[3*i]/10, vertices[3*i+1]+ normals[3*i+1]/10, vertices[3*i+2]+ normals[3*i+2]/10);
			glEnd();
		}
	}
};
