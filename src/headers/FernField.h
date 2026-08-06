//
// Created by smith on 06.08.26.
//

#ifndef WAFFLE_FERNFIELD_H
#define WAFFLE_FERNFIELD_H

#include <vector>
#include <glm/glm.hpp>

#include "FernMesh.h"
#include "InstanceBuffer.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"

// A handful of Barnsley ferns sharing one recursive line-skeleton Mesh
// (built once), each planted at its own fixed base position. Instead of
// orbiting, each fern sways by rotating about its own root - like a bone
// hinged at its base - so the base never moves while the frond swings
// left/right above it. Distance-based LOD (branch-depth culling) happens
// in the vertex shader - see shaders/fern.vert - not here.
class FernField {
public:
    struct FernInstance {
        float height;         // vertical scale of the fern, world units
        glm::vec3 baseOffset; // fixed position offset from the field center; this point never moves
        float swayAmplitude;  // radians the fern rotates about its base, peak-to-center
        float angularSpeed;   // radians/sec of the sway oscillation
        float phase;          // radians, staggers instances so they don't sway in sync
    };

    FernField(glm::vec3 center, std::vector<FernInstance> instances, FernParams params = FernParams());

    void Update(float time);
    void Draw(Renderer& renderer, Shader& shader, const glm::mat4& viewProjection, const glm::vec3& cameraPosition);

    // Rendering knobs, separate from FernParams since they're about how the
    // shared line geometry gets drawn, not its shape.
    float lineWidth = 1.5f;
    float outlineWidth = 4.0f;
    glm::vec3 outlineColor = glm::vec3(0.02f, 0.02f, 0.02f);

private:
    std::vector<InstanceData> ComputeTransforms(float time) const;

    glm::vec3 m_Center;
    std::vector<FernInstance> m_Instances;
    FernParams m_Params;
    Mesh m_Mesh;
    InstanceBuffer m_InstanceBuffer;
};

#endif //WAFFLE_FERNFIELD_H
