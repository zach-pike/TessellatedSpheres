#version 410 core

layout(location=0) in vec3 vertexPos;

layout(location=1) in vec3 spherePosition;
layout(location=2) in float sphereScale;

uniform mat4 MVP;

out vec4 sphereData;

void main() {
    gl_Position = vec4(vertexPos, 1);

    sphereData = vec4(spherePosition, sphereScale);
}