#version 410 core

in vec2 fragTexCoord;

uniform sampler2D tex1;

void main()
{
	gl_FragColor = texture(tex1, fragTexCoord);
}
