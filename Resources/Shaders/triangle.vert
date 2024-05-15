#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 MVP;

out vec2 fragTexCoord;

void main()
{
	gl_Position = MVP * position;

	fragTexCoord = texCoord;
}
