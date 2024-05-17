#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 MVP;

out vec4 materialColor;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    materialColor = vec4(position.xz, 0.0, 1.0);
}