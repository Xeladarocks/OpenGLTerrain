#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aVertexColor;

out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vColor = aVertexColor;
    //vColor = vec3(255, 255, 0);
}