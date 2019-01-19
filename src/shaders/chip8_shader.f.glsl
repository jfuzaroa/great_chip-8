#version 330 core

out vec4 color;

uniform vec3 sprite_color;

void main()
{
	color = vec4(sprite_color, 1.0f);
}
