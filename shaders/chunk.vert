// chunk.vert
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 uTransform;

out vec3 vColor;
out vec3 vWorldPos;

void main()
{
    vWorldPos = aPosition;
    gl_Position = uTransform * vec4(aPosition, 1.0);
    vColor = aColor;
}