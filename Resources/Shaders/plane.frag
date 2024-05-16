#version 410 core

in vec4 materialColor;

void main() {
	gl_FragColor = materialColor;
}