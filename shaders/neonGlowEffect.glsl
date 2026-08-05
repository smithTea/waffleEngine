// effects.glsl
vec3 neonGlow(vec3 color) {
    float brightness = max(max(color.r, color.g), color.b);
    vec3 glow = color * smoothstep(0.5, 1.0, brightness);
    return mix(color, glow * 1.5, 0.3);
}
