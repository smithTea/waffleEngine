//
// Created by smith on 05.08.26.
//

#include "headers/ChladniController.h"

#include <algorithm>

void ChladniController::PickNewTarget() {
    std::uniform_int_distribution<int> dist(minMode, maxMode);

    int n = dist(m_Rng);
    int m = dist(m_Rng);
    while (m == n) {
        // n == m collapses the field to zero everywhere (solid nodal
        // line, no figure), so re-roll m until they differ - same
        // guard the reference sketch uses.
        m = dist(m_Rng);
    }

    m_ToN = static_cast<float>(n);
    m_ToM = static_cast<float>(m);
}

void ChladniController::Update(const float deltaTime) {
    m_HoldTimer -= deltaTime;
    if (m_HoldTimer <= 0.0f) {
        m_FromN = m_ToN;
        m_FromM = m_ToM;
        PickNewTarget();
        m_Blend = 0.0f;
        m_HoldTimer = holdDuration;
    }

    if (m_Blend < 1.0f) {
        const float step = deltaTime / std::max(transitionDuration, 0.0001f);
        m_Blend = std::min(1.0f, m_Blend + step);
    }
}
