//
// Created by smith on 06.08.26.
//

#include "../headers/FernMesh.h"

#include <cmath>
#include <glm/gtc/constants.hpp>

namespace {
    void GenerateFrond(
        const glm::vec2 base,
        const float dirAngle,
        const float length,
        const int depth,
        const FernParams& params,
        const float time,
        std::vector<Vertex>& out)
    {
        const int maxDepth = params.MaxDepth();
        const int steps = params.stepsPerDepth[depth];
        const int spawnCount = params.spawnsPerDepth[depth];

        const float wave = std::sin(time * params.spiralSpeed - static_cast<float>(depth) * params.depthPhaseLag);
        const float tilt = params.baseTilt + params.spiralAmplitude * wave;
        const float curveSpiral = params.curveSpiralAmplitude * wave;
        // Only the trunk tightens into a curl as it grows - sub-branches
        // just follow whatever direction the trunk gave them at their
        // spawn point, so the fern reads as "one curling base, branches
        // fanned off it" rather than every level spiraling independently.
        const float curveGrowthThisDepth = depth == 0 ? params.curveGrowth : 0.0f;

        const glm::vec3 color = glm::mix(
            params.baseColor,
            params.tipColor,
            static_cast<float>(depth) / static_cast<float>(maxDepth));

        glm::vec2 pos = base;
        float dir = dirAngle;
        float segLen = length / static_cast<float>(steps);
        int prevSlot = -1;

        for (int i = 0; i < steps; ++i) {
            const glm::vec2 next = pos + glm::vec2(std::cos(dir), std::sin(dir)) * segLen;

            out.push_back({ glm::vec3(pos, 0.0f), color, {}, static_cast<float>(depth) });
            out.push_back({ glm::vec3(next, 0.0f), color, {}, static_cast<float>(depth) });

            // Evenly spread spawnCount leaflet-pairs across the stem's
            // sub-segments (skip i=0, right at this frond's own root).
            if (spawnCount > 0 && depth < maxDepth) {
                const int slot = (i * spawnCount) / steps;
                if (i > 0 && slot != prevSlot) {
                    const float t =
                        static_cast<float>(i + 1) /
                        static_cast<float>(steps);

                    // Strong nonlinear taper near the tip.
                    const float taper = std::pow(1.0f - t, params.tipTaperStrength);

                    const float leafletLength =
                        length * params.depthShrink * taper;
                    GenerateFrond(next, dir - tilt, leafletLength, depth + 1, params, time, out);
                    GenerateFrond(next, dir + tilt, leafletLength, depth + 1, params, time, out);
                }
                prevSlot = slot;
            }

            pos = next;
            dir += params.baseCurve + curveGrowthThisDepth * static_cast<float>(i) + curveSpiral;
            segLen *= params.tipTaper;
        }
    }
}

std::vector<Vertex> BuildFernVertices(const FernParams& params, const float time)
{
    std::vector<Vertex> vertices;
    GenerateFrond(glm::vec2(0.0f), glm::half_pi<float>(), 1.0f, 0, params, time, vertices);
    return vertices;
}

Mesh MakeFernMesh(const FernParams& params)
{
    return Mesh(BuildFernVertices(params, 0.0f));
}
