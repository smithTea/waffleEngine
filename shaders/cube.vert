#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in mat4 aInstanceTransform;

uniform mat4 uTransform;

out vec3 vColor;

void main()
{
    gl_Position = uTransform * aInstanceTransform * vec4(aPosition, 1.0);
    vColor = aColor;
}