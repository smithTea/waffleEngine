// highlight.frag
#version 460 core

in vec3 vColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 0.6);
}
