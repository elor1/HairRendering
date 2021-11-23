#version 440

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 colour;

void main() {
    colour = vec3(texCoords, 0);
    gl_Position = projection * view * model * vec4(position, 1);
}