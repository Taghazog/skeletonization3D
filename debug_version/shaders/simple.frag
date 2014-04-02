#version 330 core

uniform vec4 Ka; // ambient coefficient
uniform vec4 Kd; // diffuse coefficient
uniform vec4 Ld; // diffuse light color
uniform float cam_dist;

in vec4 vpeye; // fragment position in eye coords
in vec3 vneye; // surface normal in eye coords

out vec4 out_color;

void main()
{
  	vec4 fn_eye = vec4(normalize(vneye), 1.0);
  	vec4 light_dir_eye = normalize(- vpeye); // light to v

  	vec4 Ia = Ka;
  	vec4 Id = Ld * Kd * max(dot(light_dir_eye, fn_eye), 0.0);
  	out_color = Ia + Id;

 	float dist = max(cam_dist - 154.0/2.0, 0.0);
 	float x = 1.0 - (abs(vpeye.z) - dist)/154.0;
 	x = x*x*x*x;
 	out_color.xyz *= x;  //out_color.a = 0.1;


}
