// lighting.glsl
// Toon/arcade facing test: 1.0 when the surface faces the light, 0.0 otherwise.
float arcadeLighting(vec3 normal, vec3 lightDir) {
    float facing = dot(normalize(normal), normalize(lightDir));
    return step(0.3, facing);
}
