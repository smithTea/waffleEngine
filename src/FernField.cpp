//
// Created by smith on 06.08.26.
//

#include "headers/FernField.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    // Below this distance ferns render at full recursion depth; above the
    // far one they're cut back to just the trunk + first branch level. See
    // shaders/fern.vert. Picked a bit wider than the chunk's own
    // detail-fade band (10/22, Application.cpp) since thin lines stay
    // legible farther out than solid geometry.
    constexpr float kLodNearDist = 12.0f;
    constexpr float kLodFarDist = 30.0f;
}

FernField::FernField(const glm::vec3 center, std::vector<FernInstance> instances, FernParams params)
    : m_Center(center),
      m_Instances(std::move(instances)),
      m_Params(std::move(params)),
      m_Mesh(MakeFernMesh(m_Params))
{
    m_Mesh.Upload();

    m_InstanceBuffer.SetTransforms(ComputeTransforms(0.0f));
    m_InstanceBuffer.Upload();

    m_Mesh.AttachInstanceBuffer(m_InstanceBuffer);
}

std::vector<InstanceData> FernField::ComputeTransforms(const float time) const {
    std::vector<InstanceData> data;
    data.reserve(m_Instances.size());

    for (const auto& inst : m_Instances) {
        const float swayAngle = inst.swayAmplitude * std::sin(time * inst.angularSpeed + inst.phase);

        // The fern's root sits at local (0,0,0), so rotating before
        // translating pivots the whole frond around that root - then the
        // translate places the still-fixed root at its world position.
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Center + inst.baseOffset);
        transform = glm::rotate(transform, swayAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::scale(transform, glm::vec3(inst.height));
        data.push_back({ transform });
    }

    return data;
}

void FernField::Update(const float time) {
    // Every fern shares this one buffer, so the branch-angle animation
    // plays back identically across all of them; each instance still reads
    // as independent because of its own base-sway phase/position/scale.
    // Regenerating a few thousand vertices on the CPU per frame is well
    // under a millisecond, cheap enough to just re-upload it whole rather
    // than building any kind of GPU skinning path for it.
    m_Mesh.UpdateData(BuildFernVertices(m_Params, time), {});

    m_InstanceBuffer.SetTransforms(ComputeTransforms(time));
    m_InstanceBuffer.Update();
}

void FernField::Draw(Renderer& renderer, Shader& shader, const glm::mat4& viewProjection, const glm::vec3& cameraPosition) {
    shader.Bind();
    shader.SetMat4("uTransform", viewProjection);
    shader.SetVec3("uCameraPosition", cameraPosition);
    shader.SetFloat("uLodNearDist", kLodNearDist);
    shader.SetFloat("uLodFarDist", kLodFarDist);
    shader.SetFloat("uMaxDepth", static_cast<float>(m_Params.MaxDepth()));

    // Outline pass: thicker, solid dark lines with depth writes turned off
    // (still depth-*tested*, so it's correctly hidden behind walls/floor)
    // so the color pass drawn right after it on top is never blocked by
    // its own outline - avoids z-fighting without needing polygon offset,
    // which doesn't apply to GL_LINES anyway.
    shader.SetInt("uOutlineMode", 1);
    shader.SetVec3("uOutlineColor", outlineColor);
    glDepthMask(GL_FALSE);
    glLineWidth(outlineWidth);
    renderer.DrawInstancedArrays(m_Mesh, shader, m_InstanceBuffer, GL_LINES);
    glDepthMask(GL_TRUE);

    shader.SetInt("uOutlineMode", 0);
    glLineWidth(lineWidth);
    renderer.DrawInstancedArrays(m_Mesh, shader, m_InstanceBuffer, GL_LINES);
    glLineWidth(1.0f);
}
