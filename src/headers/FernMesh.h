//
// Created by smith on 06.08.26.
//

#ifndef WAFFLE_FERNMESH_H
#define WAFFLE_FERNMESH_H

#include <vector>
#include <glm/glm.hpp>

#include "Mesh.h"

// Tunable shape/animation parameters for the recursive fern generator.
// stepsPerDepth and spawnsPerDepth must be the same length - that length
// is the recursion depth (index 0 = trunk); spawnsPerDepth's last entry is
// ignored since there's no depth+1 left to recurse into.
struct FernParams {
    std::vector<int> stepsPerDepth  = { 32, 8, 5, 3 };
    std::vector<int> spawnsPerDepth = { 12, 4, 2, 0 };

    float baseCurve = 0.0f;    // radians the stem bends per sub-segment, at rest
    float curveGrowth = 0.0f;  // depth-0 only: how much that curve intensifies each step - the tightening spiral/fiddlehead curl
    float baseTilt = 0.45f;      // radians a spawned leaflet tilts off its parent stem

    // A child branch's length is parentLength * depthShrink * (1 - tipTaperStrength * t),
    // where t is how far along the parent (0 = parent's own base, 1 = its tip)
    // this child sprouts from. depthShrink controls how much shorter each
    // recursion level is than its parent (keep this fairly close to 1 so
    // 2nd/3rd-order branches stay long and visibly green); tipTaperStrength
    // controls how much shorter branches get the closer they sprout to
    // their parent's own tip (push this toward 1 for a strong taper).
    float depthShrink = 0.32f;
    float tipTaperStrength = 1.2f;

    float tipTaper = 0.95f;     // stem sub-segments shorten slightly toward the tip

    // Angle animation: tilt/curve oscillate around their rest value,
    // phase-lagged per depth so the motion ripples from trunk to tips and
    // back (spiraling in/out) instead of the whole plant rocking as one body.
    float spiralSpeed = 0.5f;
    float spiralAmplitude = 0.3f;
    float curveSpiralAmplitude = 0.05f;
    float depthPhaseLag = 0.9f;

    glm::vec3 baseColor = glm::vec3(0.40f, 0.24f, 0.09f); // depth 0 (trunk)
    glm::vec3 tipColor  = glm::vec3(0.25f, 0.62f, 0.20f); // deepest depth

    int MaxDepth() const { return static_cast<int>(stepsPerDepth.size()) - 1; }
};

// A recursive fractal fern skeleton built from `params`: a curving trunk
// with leaflets sprouting along it, each leaflet a smaller self-similar
// copy of the same shape. Emits vertex pairs meant for GL_LINES (no
// indices - each consecutive pair *is* one line segment). Vertex.MaterialID
// carries the segment's recursion depth (0 = trunk) for depth-based LOD
// in the shader.
std::vector<Vertex> BuildFernVertices(const FernParams& params, float time);

// Convenience wrapper for the initial (time = 0) upload.
Mesh MakeFernMesh(const FernParams& params);

#endif //WAFFLE_FERNMESH_H
