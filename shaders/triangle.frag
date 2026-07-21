// chunk.frag
#version 460 core

in vec3 vColor;
in vec3 vWorldPos;

out vec4 FragColor;

float gridLine(vec3 p) {
    vec3 grid = abs(fract(p) - 0.5); // 0 = střed buňky, 0.5 = hrana
    vec3 deriv = fwidth(p);
    vec3 lineWidth = deriv * 3.0;
    // chceme 1 (svítí) když grid je BLÍZKO 0.5 (hrana), tedy grid > (0.5 - lineWidth)
    vec3 lines = smoothstep(0.5 - lineWidth, vec3(0.5), grid);
    return max(max(lines.x, lines.y), lines.z);
}

void main()
{
    float grid = gridLine(vWorldPos);
    vec3 neonColor = vec3(0.0, 1.0, 0.85);
    vec3 finalColor = mix(vColor, neonColor, grid);
    FragColor = vec4(finalColor, 1.0);
}