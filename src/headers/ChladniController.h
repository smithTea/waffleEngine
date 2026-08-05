//
// Created by smith on 05.08.26.
//

#ifndef WAFFLE_CHLADNICONTROLLER_H
#define WAFFLE_CHLADNICONTROLLER_H

#include <random>

// Drives a Chladni pattern as a hold-then-crossfade state machine: sits on
// a clean integer (n, m) pair (the only pairs that form crisp nodal
// figures) for holdDuration seconds, then crossfades to a newly picked
// pair over transitionDuration seconds.
//
// Note this blends the *field*, not the mode numbers: lerping n/m directly
// sweeps through non-integer frequencies, which makes the nodal lines
// swim chaotically mid-transition. Crossfading two fixed, clean fields by
// weight has no such artifact - both endpoints stay crisp throughout.
class ChladniController {
public:
    void Update(float deltaTime);

    [[nodiscard]] float GetFromN() const { return m_FromN; }
    [[nodiscard]] float GetFromM() const { return m_FromM; }
    [[nodiscard]] float GetToN() const { return m_ToN; }
    [[nodiscard]] float GetToM() const { return m_ToM; }
    [[nodiscard]] float GetBlend() const { return m_Blend; }

    float holdDuration = 5.0f;
    float transitionDuration = 2.5f;
    int minMode = 1;
    int maxMode = 6;

private:
    void PickNewTarget();

    float m_FromN = 3.0f;
    float m_FromM = 4.0f;
    float m_ToN = 3.0f;
    float m_ToM = 4.0f;
    float m_Blend = 1.0f; // 1 = fully settled on (to)
    float m_HoldTimer = 0.0f;

    std::mt19937 m_Rng{std::random_device{}()};
};

#endif //WAFFLE_CHLADNICONTROLLER_H
