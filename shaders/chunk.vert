// chunk.vert
#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in float aMaterialID;

uniform mat4 uTransform;

out vec3 vColor;
out vec3 vWorldPos;
out vec3 vNormal;
out float vMaterialID;

void main()
{
    vWorldPos = aPosition;
    vNormal = aNormal;
    vMaterialID = aMaterialID;
    gl_Position = uTransform * vec4(aPosition, 1.0);
    vColor = aColor;
}