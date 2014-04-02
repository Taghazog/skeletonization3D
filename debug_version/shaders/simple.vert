#version 330 core

uniform mat4 view_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;

in vec3 vertexPosition;
in vec3 vtx_normal;


out vec4 vpeye;
out vec3 vneye;

void main()
{
  	vpeye = view_matrix * vec4(vertexPosition, 1.0);
  	
	vneye = normal_matrix * vtx_normal;
	gl_Position = projection_matrix * vpeye;
}


