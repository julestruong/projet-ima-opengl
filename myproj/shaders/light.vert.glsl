out vec4 myvertex;
out vec3 mynormal;
uniform float mytime;

void main() {
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex; 

	myvertex = gl_Vertex;
	mynormal = gl_Normal;
}