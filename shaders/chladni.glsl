// chladni.glsl
// Chladni plate nodal patterns: https://paulbourke.net/geometry/chladni/
// Classic two-mode superposition. Nodal lines (where the field is ~0) are
// where sand collects on a real vibrating plate.

const float CHLADNI_PI = 3.14159265359;

float chladniField(vec2 uv, float n, float m) {
    return sin(CHLADNI_PI * n * uv.x) * sin(CHLADNI_PI * m * uv.y)
         - sin(CHLADNI_PI * m * uv.x) * sin(CHLADNI_PI * n * uv.y);
}

// Analytic gradient of chladniField. Treating the field as a fake height
// map, this lets a caller tilt a flat surface normal along the pattern's
// slope (bump mapping) so lighting reads the nodal lines as relief rather
// than flat paint, with no real geometry or texture involved.
vec2 chladniGradient(vec2 uv, float n, float m) {
    float du = CHLADNI_PI * n * cos(CHLADNI_PI * n * uv.x) * sin(CHLADNI_PI * m * uv.y)
             - CHLADNI_PI * m * cos(CHLADNI_PI * m * uv.x) * sin(CHLADNI_PI * n * uv.y);
    float dv = CHLADNI_PI * m * sin(CHLADNI_PI * n * uv.x) * cos(CHLADNI_PI * m * uv.y)
             - CHLADNI_PI * n * sin(CHLADNI_PI * m * uv.x) * cos(CHLADNI_PI * n * uv.y);
    return vec2(du, dv);
}
