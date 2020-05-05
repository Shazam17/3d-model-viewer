#shader vertex
#version 330 core


layout(location = 0) in vec3 pos;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 fragPos;

void main() {
	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);
	fragPos = pos;
}


#shader fragment
#version 330 core

in vec3 fragPos;

out vec4 color;

void main() {

	vec3 mappedColor = fragPos * 2.0 + 1.0;

	color = vec4(mappedColor, 1);
}