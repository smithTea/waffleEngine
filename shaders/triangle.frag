// chunk.frag
#version 460 core

#include "arcadeLightingShader.glsl"
#include "neonGlowEffect.glsl"
#include "chladni.glsl"

#define MAX_LIGHTS 8
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define MAX_MATERIALS 8

struct Light {
    int type;
    vec3 direction; // Directional
    vec3 position;  // Point
    vec3 color;
    float intensity;
    float radius;   // Point falloff distance
};

// Per-material Chladni look: each material (wall, floor, roof, ...) has
// its own colors and its own mode numbers, so different surfaces can be
// tuned to feel calm or violent independently of one another. (nA,mA) is
// the figure being transitioned FROM, (nB,mB) the one being transitioned
// TO, and blend crossfades between them - see ChladniController for why
// that's crossfaded rather than lerping the mode numbers directly.
struct ChladniMaterial {
    vec3 baseColor;
    vec3 nodalColor;
    float bumpStrength;
    float lineWidth;
    float scale;
    float nA;
    float mA;
    float nB;
    float mB;
    float blend;
};

in vec3 vColor;
in vec3 vWorldPos;
in vec3 vNormal;
in float vMaterialID;

uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;
uniform float uAmbientStrength;
uniform ChladniMaterial uMaterials[MAX_MATERIALS];
uniform vec3 uAccentColor;

out vec4 FragColor;

// Voxel faces are always axis-aligned, so the dominant normal axis
// picks out the other two world axes to use as a flat 2D UV + tangent
// basis for the face - exact, no blending needed like general triplanar.
void chladniFaceBasis(vec3 normal, vec3 worldPos, out vec2 uv, out vec3 tangentU, out vec3 tangentV) {
    vec3 n = abs(normal);
    if (n.x >= n.y && n.x >= n.z) {
        uv = worldPos.yz;
        tangentU = vec3(0.0, 1.0, 0.0);
        tangentV = vec3(0.0, 0.0, 1.0);
    } else if (n.y >= n.z) {
        uv = worldPos.xz;
        tangentU = vec3(1.0, 0.0, 0.0);
        tangentV = vec3(0.0, 0.0, 1.0);
    } else {
        uv = worldPos.xy;
        tangentU = vec3(1.0, 0.0, 0.0);
        tangentV = vec3(0.0, 1.0, 0.0);
    }
}

vec3 shadeLights(vec3 normal, vec3 worldPos) {
    vec3 total = vec3(0.0);

    for (int i = 0; i < uLightCount; i++) {
        Light light = uLights[i];

        vec3 lightDir;
        float attenuation = 1.0;

        if (light.type == LIGHT_POINT) {
            vec3 toLight = light.position - worldPos;
            float dist = length(toLight);
            lightDir = toLight / max(dist, 0.0001);
            attenuation = clamp(1.0 - dist / max(light.radius, 0.0001), 0.0, 1.0);
            attenuation *= attenuation; // soften the falloff curve
        } else {
            lightDir = light.direction;
        }

        float facing = arcadeLighting(normal, lightDir);
        total += facing * light.intensity * light.color * attenuation;
    }

    return total;
}

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
    ChladniMaterial mat = uMaterials[int(vMaterialID)];

    vec2 chladniUV;
    vec3 tangentU, tangentV;
    chladniFaceBasis(vNormal, vWorldPos, chladniUV, tangentU, tangentV);
    chladniUV *= mat.scale;

    // Ease-in-out the blend weight (rather than using it linearly) so the
    // crossfade itself doesn't feel like it snaps on/off at either end.
    float t = smoothstep(0.0, 1.0, mat.blend);

    float fieldA = chladniField(chladniUV, mat.nA, mat.mA);
    float fieldB = chladniField(chladniUV, mat.nB, mat.mB);
    float field = mix(fieldA, fieldB, t);

    vec2 gradA = chladniGradient(chladniUV, mat.nA, mat.mA);
    vec2 gradB = chladniGradient(chladniUV, mat.nB, mat.mB);
    vec2 grad = mix(gradA, gradB, t);

    // Screen-space rate of change of the field. When a fragment's pixel
    // footprint spans many oscillations of the pattern (far away, or a
    // grazing angle), this spikes - that's exactly the "many periods
    // collapsed into one pixel" condition that causes aliasing. Fading
    // detail out smoothly there (rather than letting it flicker) is the
    // same idea as texture mipmapping, done analytically since this is
    // a procedural field rather than a sampled texture.
    float fieldChangeRate = fwidth(field);
    float detailFade = 1.0 - smoothstep(0.4, 1.6, fieldChangeRate);

    vec3 bumpedNormal = normalize(vNormal - mat.bumpStrength * detailFade * (grad.x * tangentU + grad.y * tangentV));

    // Nodal line threshold widens with the local screen-space derivative
    // (same technique gridLine already uses below) so the line stays
    // roughly pixel-width instead of shrinking into a flickering,
    // sub-pixel sliver at distance.
    float lineThreshold = max(mat.lineWidth, fieldChangeRate * 1.5);
    float nodalLines = (1.0 - smoothstep(0.0, lineThreshold, abs(field))) * detailFade;

    // Lines (both nodal and grid) blend the material's own accent with a
    // shared accent color, so every surface's linework reads as part of
    // one connected palette instead of each material feeling isolated.
    vec3 lineColor = mix(mat.nodalColor, uAccentColor, 0.5) * 1.2;

    vec3 lightAmount = vec3(uAmbientStrength) + shadeLights(bumpedNormal, vWorldPos);
    vec3 litColor = mat.baseColor * lightAmount;
    litColor = mix(litColor, lineColor * lightAmount, nodalLines);

    float grid = gridLine(vWorldPos);
    const float gridOpacity = 0.6;
    vec3 finalColor = mix(litColor, lineColor * lightAmount, grid * gridOpacity);
    finalColor = neonGlow(finalColor);
    FragColor = vec4(finalColor, 1.0);
}