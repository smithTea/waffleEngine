#version 460 core

in vec3 vColor;
out vec4 FragColor;

uniform int uOutlineMode;
uniform vec3 uOutlineColor;

void main()
{
    FragColor = uOutlineMode == 1 ? vec4(uOutlineColor, 1.0) : vec4(vColor, 1.0);
}
