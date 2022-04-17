#version 400

uniform mat4 u_mvp;

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
//layout(location = 2) in vec3 in_color;

out vec3 o_color;
out vec3 o_normal;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
	o_normal = in_normal;
}