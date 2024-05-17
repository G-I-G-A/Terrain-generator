#version 330 core

in vec4 materialColor;
out vec4 FragColor;

void main() {
    FragColor = materialColor;
}